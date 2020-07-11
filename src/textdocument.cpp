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

TextCursorImpl* TextDocumentImpl::createCursor(const Position & pos, const TextBlock & b)
{
  TextCursorImpl *ret = new TextCursorImpl(pos, b);
  ret->ref = 1;
  this->cursors.push_back(ret);
  return ret;
}

void TextDocumentImpl::destroyCursor(TextCursorImpl *cursor)
{
  auto it = std::find(this->cursors.begin(), this->cursors.end(), cursor);

  if (it != this->cursors.end())
    this->cursors.erase(it);

  delete cursor;
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

  block.impl()->content.erase(block.impl()->content.begin() + block.length() - pos.column, block.impl()->content.end());
  block.impl()->revision += 1;

  // Update cursors
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    TextCursorImpl *c = this->cursors[i];

    if (c->pos.line == pos.line && c->pos.column >= pos.column)
    {
      c->pos.line += 1;
      c->pos.column -= block.length();
      c->block = c->block.next();
    }
    else if (c->pos.line > pos.line)
    {
      c->pos.line += 1;
    }

    if (c->anchor.line == pos.line && c->anchor.column >= pos.column)
    {
      c->anchor.line += 1;
      c->anchor.column -= block.length();
    }
    else if (c->anchor.line > pos.line)
    {
      c->anchor.line += 1;
    }
  }

  for (const auto& l : listeners)
  {
    l->blockInserted(pos, TextBlock{ document, newblock });
    l->blockCountChanged(this->lineCount);
    l->contentsChanged();
  }
}

void TextDocumentImpl::insertChar(Position pos, const TextBlock & block, unicode::Character c)
{
  // TODO: not correct, we need to take into account the 1 character != 1 char
  unicode::Utf8Char u8c{ c };
  block.impl()->content.insert(pos.column, u8c.data());
  block.impl()->revision += 1;

  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    auto *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->pos, block, pos, 0, 1);
    TextDocument::updatePositionOnContentsChange(c->anchor, block, pos, 0, 1);
  }

  for (const auto& l : listeners)
  {
    l->contentsChange(block, pos, 0, 1);
    l->contentsChanged();
  }
}

void TextDocumentImpl::insertText(Position pos, const TextBlock & block, const std::string& str)
{
  // TODO: not correct, we need to take into account the 1 character != 1 char
  block.impl()->content.insert(pos.column, str);
  block.impl()->revision += 1;

  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    auto *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->pos, block, pos, 0, str.length());
    TextDocument::updatePositionOnContentsChange(c->anchor, block, pos, 0, str.length());
  }

  for (const auto& l : listeners)
  {
    l->contentsChange(block, pos, 0, str.length());
    l->contentsChanged();
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

  for (const auto& l : listeners)
  {
    l->contentsChanged();
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

  for (const auto& l : listeners)
  {
    l->contentsChanged();
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

  for (const auto& l : listeners)
  {
    l->contentsChanged();
  }
}

void TextDocumentImpl::remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count)
{
  beginBlock.impl()->content.erase(begin.column, count);

  // update cursors
  for (size_t i(0); i < this->cursors.size(); ++i)
  {
    TextCursorImpl *c = this->cursors[i];
    TextDocument::updatePositionOnContentsChange(c->pos, beginBlock, begin, count, 0);
    TextDocument::updatePositionOnContentsChange(c->anchor, beginBlock, begin, count, 0);
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
    TextCursorImpl *c = this->cursors[i];
    TextDocument::updatePositionOnBlockDestroyed(c->pos, blocknum, block);
    TextDocument::updatePositionOnBlockDestroyed(c->anchor, blocknum, block);
    if (c->pos.line == blocknum - 1)
      c->block = prev;
  }

  block.impl()->setGarbage();
  this->lineCount -= 1;

  for (const auto& l : listeners)
  {
    l->blockDestroyed(blocknum, block);
  }
}


TextDocumentListener::TextDocumentListener()
{

}

TextDocumentListener::~TextDocumentListener()
{

}

TextDocument* TextDocumentListener::document() const
{
  return m_document;
}

void TextDocumentListener::blockInserted(const Position& pos, const TextBlock& newblock)
{

}

void TextDocumentListener::blockCountChanged(int newBlockCount)
{

}

void TextDocumentListener::blockDestroyed(int line, const TextBlock& block)
{

}

void TextDocumentListener::contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{

}

void TextDocumentListener::contentsChanged()
{

}


TextDocument::TextDocument()
  : d(new TextDocumentImpl(this))
{

}

TextDocument::TextDocument(const std::string& text)
  : d(new TextDocumentImpl(this))
{
  TextCursor{ this }.insertText(text);
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

void TextDocument::apply(const TextDiff & diff)
{
  const std::vector<TextDiff::Diff> & diffs = diff.diffs();

  if (diffs.size() == 0)
    return;

  TextCursor c{ this };
  std::vector<TextCursor> cursors;
  cursors.reserve(diffs.size());

  /// TODO: add to undo stack

  // Create edit cursors
  for (const auto & d : diff.diffs())
  {
    c.setPosition(d.begin());
    c.setPosition(d.end(), TextCursor::KeepAnchor);
    cursors.push_back(c);
  }

  // Apply diff
  for (int i(0); i < diffs.size(); ++i)
  {
    const auto & d = diff.diffs().at(i);
    if (d.kind == TextDiff::Removal)
      cursors[i].removeSelectedText();
    else
      cursors[i].insertText(d.text());
  }
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
