// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/textdocument.h"
#include "typewriter/private/textdocument_p.h"

#include "typewriter/textblock.h"
#include "typewriter/textcursor.h"
#include "typewriter/textdiff.h"

#include <unicode/utf8.h>

#include <iostream>

namespace typewriter
{

TextDocumentImpl::TextDocumentImpl(TextDocument *doc)
  : document(doc)
  , lineCount(1)
  , firstBlock(new TextBlockImpl())
  , lastBlock(firstBlock)
  , idgen(0)
{
  firstBlock.get()->id = idgen++;
}

TextDocumentImpl::~TextDocumentImpl()
{
  if (!this->cursors.empty())
  {
    std::cerr << "Warning: TextDocument destroyed but some cursors are still active" << std::endl;
  }
}

int TextDocumentImpl::blockNumber(TextBlockImpl *block) const
{
  int n = 0;
  TextBlockImpl *it = firstBlock.get();
  while (it != nullptr && block != it)
  {
    ++n;
    it = it->next.get();
  }

  if (it != nullptr)
    return n;
  return -1;
}

int TextDocumentImpl::blockOffset(TextBlockImpl *block) const
{
  int n = 0;
  TextBlockImpl *it = firstBlock.get();
  while (it != nullptr && block != it)
  {
    n += it->content.length() + 1;
    it = it->next.get();
  }

  if (it != nullptr)
    return n;
  return -1;
}

void TextDocumentImpl::register_cursor(TextCursor* c)
{
  assert(c != nullptr);
  this->cursors.push_back(c);
}

void TextDocumentImpl::swap_cursor(TextCursor* existing_cursor, TextCursor* new_cursor) noexcept
{
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    if (this->cursors[i] == existing_cursor)
    {
      this->cursors[i] = new_cursor;
      return;
    }
  }
}

void TextDocumentImpl::deregister_cursor(TextCursor* c) noexcept
{
  auto it = std::find(this->cursors.begin(), this->cursors.end(), c);

  if (it != this->cursors.end())
    this->cursors.erase(it);
}

void TextDocumentImpl::insertBlock(Position pos, const TextBlock & block)
{
  TextBlockImpl *newblock = new TextBlockImpl{ block.text().substr(pos.column) };
  newblock->id = idgen++;

  newblock->previous = block.impl();

  if (this->lastBlock == block.impl())
  {
    this->lastBlock = newblock;
    block.impl()->next = newblock;
    /* newblock->next = nullptr; */
  }
  else
  {
    block.next().impl()->previous = newblock;
    newblock->next = block.next().impl();
    block.impl()->next = newblock;
  }

  this->lineCount += 1;

  block.impl()->content.erase(block.impl()->content.begin() + pos.column, block.impl()->content.end());
  block.impl()->revision += 1;

  if (this->transaction.is_active())
    this->transaction.delta << diff::insert(pos, "\n");

  // Update cursors
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    TextCursor *c = this->cursors[i];

    if (c->m_pos.line == pos.line && c->m_pos.column >= pos.column)
    {
      c->m_pos.line += 1;
      c->m_pos.column -= block.length();
      c->m_block = c->m_block.next();
    }
    else if (c->m_pos.line > pos.line)
    {
      c->m_pos.line += 1;
    }

    if (c->m_anchor.line == pos.line && c->m_anchor.column >= pos.column)
    {
      c->m_anchor.line += 1;
      c->m_anchor.column -= block.length();
    }
    else if (c->m_anchor.line > pos.line)
    {
      c->m_anchor.line += 1;
    }
  }

  for (const auto& l : listeners)
  {
    l->blockInserted(pos, TextBlock{ document, newblock });
  }
}

void TextDocumentImpl::insertChar(Position pos, const TextBlock & block, unicode::Character c)
{
  // TODO: not correct, we need to take into account the 1 character != 1 char
  unicode::Utf8Char u8c{ c };
  block.impl()->content.insert(pos.column, u8c.data());
  block.impl()->revision += 1;

  if (this->transaction.is_active())
    this->transaction.delta << diff::insert(pos, u8c.data());

  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    auto *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->m_pos, block, pos, 0, 1);
    TextDocument::updatePositionOnContentsChange(c->m_anchor, block, pos, 0, 1);
  }

  for (const auto& l : listeners)
  {
    l->contentsChange(block, pos, 0, 1);
  }
}

void TextDocumentImpl::insertText(Position pos, const TextBlock & block, const std::string& str)
{
  // @TODO: try to make it a precondition
  if (str.empty())
    return;

  // TODO: not correct, we need to take into account the 1 character != 1 char
  block.impl()->content.insert(pos.column, str);
  block.impl()->revision += 1;

  if (this->transaction.is_active())
    this->transaction.delta << diff::insert(pos, str);

  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    auto *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->m_pos, block, pos, 0, str.length());
    TextDocument::updatePositionOnContentsChange(c->m_anchor, block, pos, 0, str.length());
  }

  for (const auto& l : listeners)
  {
    l->contentsChange(block, pos, 0, str.length());
  }
}

void TextDocumentImpl::deleteChar(Position pos, const TextBlock & block)
{
  if (pos.column == block.length())
  {
    if (block == document->lastBlock())
      return;

    remove_block(pos.line + 1, block.next());
  }
  else
  {
    remove_selection_singleline(pos, block, 1);
  }
}

void TextDocumentImpl::deletePreviousChar(Position pos, const TextBlock & block)
{
  if (pos.column == 0)
  {
    if (block == document->firstBlock())
      return;

    remove_block(pos.line, block);
  }
  else
  {
    remove_selection_singleline(Position{ pos.line, pos.column - 1 }, block, 1);
  }
}

void TextDocumentImpl::removeSelection(const Position begin, const TextBlock & beginBlock, const Position end)
{
  if (begin == end)
    return;

  if (begin.line == end.line)
  {
    if (begin.column < end.column)
      remove_selection_singleline(begin, beginBlock, end.column - begin.column);
    else
      remove_selection_singleline(end, beginBlock, begin.column - end.column);
  }
  else if (begin < end)
  {
    remove_selection_multiline(begin, beginBlock, end);
  }
  else
  {
    /// TODO: write another overload to handle this case without having to iterate over the blocks twice.
    remove_selection_multiline(end, prev(beginBlock, begin.line - end.line), begin);
  }
}

void TextDocumentImpl::beginTransaction(Author author)
{
  if (transaction.is_active() && transaction.author != author)
    throw std::runtime_error{ "A transaction is already active" };

  transaction.author = author;
  transaction.depth += 1;
}

void TextDocumentImpl::endTransaction(Author author)
{
  if (!transaction.is_active())
    throw std::runtime_error{ "No active transaction" };

  if (transaction.author != author)
    throw std::runtime_error{ "Do not own active transaction" };

  transaction.depth -= 1;

  if (transaction.depth != 0)
    return;

  m_redo_stack.clear();

  Contribution contrib;
  contrib.author = author;
  contrib.delta = std::move(transaction.delta);

  transaction.delta.clear();

  m_undo_stack.push_back(std::move(contrib));
  m_redo_stack.clear();
}

void TextDocumentImpl::undo(Author author)
{
  if (m_undo_stack.empty())
    throw std::runtime_error{ "Undo stack is empty" };

  if (m_undo_stack.back().author == author)
  {
    revert(m_undo_stack.back().delta);
    m_redo_stack.push_back(std::move(m_undo_stack.back()));
    m_undo_stack.pop_back();
  }
  else
  {
    // @TODO: try to solve undo
    throw std::runtime_error{ "Do not own last undo action" };
  }
}

void TextDocumentImpl::redo(Author author)
{
  if (m_redo_stack.empty())
    throw std::runtime_error{ "Redo stack is empty" };

  if (m_redo_stack.back().author == author)
  {
    apply(m_redo_stack.back().delta);
    m_undo_stack.push_back(std::move(m_redo_stack.back()));
    m_redo_stack.pop_back();
  }
  else
  {
    // @TODO: try to solve undo
    throw std::runtime_error{ "Do not own last redo action" };
  }
}

void TextDocumentImpl::remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count)
{
  // @TODO: try to make it a precondition
  if (count == 0)
    return;

  if (this->transaction.is_active())
  {
    std::string removed = beginBlock.impl()->content.substr(begin.column, count);
    this->transaction.delta << diff::remove(begin, std::move(removed));
  }

  beginBlock.impl()->content.erase(begin.column, count);

  // update cursors
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    TextCursor *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->m_pos, beginBlock, begin, count, 0);
    TextDocument::updatePositionOnContentsChange(c->m_anchor, beginBlock, begin, count, 0);
  }

  for (const auto& l : listeners)
  {
    l->contentsChange(beginBlock, begin, count, 0);
  }
}

void TextDocumentImpl::remove_selection_multiline(const Position begin, const TextBlock & beginBlock, const Position end)
{
  assert(begin.line < end.line);

  const int count = end.line - begin.line;

  // erase content on the first line
  int charsRemoved = beginBlock.length() - begin.column;
  remove_selection_singleline(begin, beginBlock, charsRemoved);

  // erase all middle lines
  for (int i(1); i < count; ++i)
  {
    TextBlock nextBlock = beginBlock.next();
    charsRemoved = nextBlock.text().length();
    remove_selection_singleline(Position{ begin.line + 1, 0 }, nextBlock, nextBlock.length());
    remove_block(begin.line + 1, nextBlock);
  }

  // erase content on the last line
  TextBlock endBlock = beginBlock.next();
  charsRemoved = end.column;
  remove_selection_singleline(Position{ begin.line + 1, 0 }, endBlock, charsRemoved);
  remove_block(begin.line + 1, endBlock);
}

void TextDocumentImpl::remove_block(int blocknum, TextBlock block)
{
  TextBlock prev = block.previous();
  prev.impl()->content.append(block.text());

  if (this->transaction.is_active())
    this->transaction.delta << diff::remove(Position{ blocknum, prev.length() }, "\n");

  if (this->lastBlock == block.impl())
  {
    this->lastBlock = prev.impl();
    prev.impl()->next = nullptr;
  }
  else
  {
    TextBlock next = block.next();
    prev.impl()->next = next.impl();
    next.impl()->previous = prev.impl();
  }

  // update cursors
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    TextCursor *c = this->cursors[i];
    TextDocument::updatePositionOnBlockDestroyed(c->m_pos, blocknum, block);
    TextDocument::updatePositionOnBlockDestroyed(c->m_anchor, blocknum, block);
    if (c->m_pos.line == blocknum - 1)
      c->m_block = prev;
  }

  // @TODO: it causes a crash if we call setGarbage() here
  // block.impl()->setGarbage();
  this->lineCount -= 1;

  for (const auto& l : listeners)
  {
    l->blockDestroyed(blocknum, block);
  }

  // @TODO: should we call setGarbage() earlier
  block.impl()->setGarbage();
}

void TextDocumentImpl::apply(const TextDiff& diff, bool inv)
{
  const std::vector<TextDiff::Diff>& diffs = diff.diffs();

  if (diffs.size() == 0)
    return;

  // @TODO: use RAII
  this->cursors_are_ghosts = true;

  TextCursor c{ this->document };
  std::vector<TextCursor> cursors;
  cursors.reserve(diffs.size());

  // Create edit cursors
  for (const auto& d : diff.diffs())
  {
    c.setPosition(d.begin());

    if(d.kind == TextDiff::Removal || (d.kind == TextDiff::Insertion && inv))
      c.setPosition(d.end(), TextCursor::KeepAnchor);

    cursors.push_back(c);
  }

  // Apply diff
  for (int i(0); i < diffs.size(); ++i)
  {
    const auto& d = diff.diffs().at(i);

    TextDiff::Kind kind = d.kind;

    if (inv)
      kind = (kind == TextDiff::Removal ? TextDiff::Insertion : TextDiff::Removal);

    if (kind == TextDiff::Removal)
      cursors[i].removeSelectedText();
    else
      cursors[i].insertText(d.text());
  }

  // @TODO: use RAII
  this->cursors_are_ghosts = false;
}

void TextDocumentImpl::revert(const TextDiff& diff)
{
  apply(diff, true);
}

TextDocumentListener::TextDocumentListener()
{

}

TextDocumentListener::~TextDocumentListener()
{
  if (m_document)
    m_document->removeListener(this);
}

TextDocument* TextDocumentListener::document() const
{
  return m_document;
}

void TextDocumentListener::blockInserted(const Position& pos, const TextBlock& newblock)
{

}

void TextDocumentListener::blockDestroyed(int line, const TextBlock& block)
{

}

void TextDocumentListener::contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{

}


TextDocument::TextDocument()
  : d(new TextDocumentImpl(this))
{

}

TextDocument::TextDocument(const std::string& text)
  : d(new TextDocumentImpl(this))
{
  d->cursors_are_ghosts = true;
  TextCursor{ this }.insertText(text);
  d->cursors_are_ghosts = false;
}

TextDocument::~TextDocument()
{

}

const std::string& TextDocument::text(int line) const
{
  return findBlockByNumber(line).text();
}

std::string TextDocument::toString() const
{
  size_t total_length = 0;

  auto it = firstBlock();
  do
  {
    total_length += it.text().length() + 1;
    it = it.next();
  } while (it.isValid());

  std::string result;
  result.reserve(total_length);

  it = firstBlock();
  do
  {
    result.insert(result.end(), it.text().begin(), it.text().end());
    result.push_back('\n');
    it = it.next();
  } while (it.isValid());


  if(!result.empty())
   result.pop_back();

  return result;
}

TextBlock TextDocument::firstBlock() const
{
  return TextBlock{ this, d->firstBlock.get() };
}

TextBlock TextDocument::lastBlock() const
{
  return TextBlock{ this, d->lastBlock.get() };
}

TextBlock TextDocument::findBlockByNumber(int num) const
{
  int n = 0;
  TextBlockImpl *it = d->firstBlock.get();
  while (it != nullptr && num != 0)
  {
    --num;
    it = it->next.get();
  }

  if (it == nullptr)
    return TextBlock{};
  return TextBlock{ this, it };
}

int TextDocument::availableUndoSteps() const
{
  return 0;
}

int TextDocument::availableRedoSteps() const
{
  return 0;
}

void TextDocument::undo()
{
  if (!isUndoAvailable())
    return;

  throw std::runtime_error{ "Not implemented" };
}

void TextDocument::redo()
{
  if (!isRedoAvailable())
    return;

  throw std::runtime_error{ "Not implemented" };
}

void TextDocument::updatePositionOnInsert(Position & pos, const Position & insertpos, const TextBlock & newblock)
{
  if (pos.line == insertpos.line && pos.column >= insertpos.column)
  {
    pos.line += 1;
    pos.column -= newblock.previous().length();
  }
  else if (pos.line > insertpos.line)
  {
    pos.line += 1;
  }
}

void TextDocument::updatePositionOnBlockDestroyed(Position & pos, int linenum, const TextBlock & block)
{
  if (pos.line == linenum)
  {
    pos.line -= 1;
    pos.column += block.previous().length() - block.length();
  }
  else if (pos.line > linenum)
  {
    pos.line -= 1;
  }
}

void TextDocument::updatePositionOnContentsChange(Position & pos, const TextBlock & block, const Position & editpos, int charsRemoved, int charsAdded)
{
  if (pos.line != editpos.line)
    return;

  if (pos.column >= editpos.column && pos.column <= editpos.column + charsRemoved)
    pos.column = editpos.column;
  else if (pos.column > editpos.column + charsRemoved)
    pos.column -= charsRemoved;

  if (pos.column >= editpos.column)
    pos.column += charsAdded;
}

void TextDocument::addListener(TextDocumentListener* listener)
{
  assert(listener);
  listener->m_document = this;
  d->listeners.push_back(std::unique_ptr<TextDocumentListener>(listener));
}

void TextDocument::removeListener(TextDocumentListener* listener)
{
  auto it = std::find_if(d->listeners.begin(), d->listeners.end(), [listener](const std::unique_ptr<TextDocumentListener>& elem) -> bool {
    return elem.get() == listener;
    });

  if (it == d->listeners.end())
    return;

  listener->m_document = nullptr;
  it->release();
  d->listeners.erase(it);
}

int TextDocument::lineCount() const
{
  return d->lineCount;
}

} // namespace typewriter
