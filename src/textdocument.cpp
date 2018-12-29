// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textdocument.h"
#include "textedit/private/textdocument_p.h"

#include "textedit/textblock.h"
#include "textedit/textcursor.h"

#include <QDebug>

namespace textedit
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
  if (!this->cursors.isEmpty())
  {
    qDebug() << "Warning: TextDocument destroyed but some cursors are still active";
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
  this->cursors.append(ret);
  return ret;
}

void TextDocumentImpl::destroyCursor(TextCursorImpl *cursor)
{
  this->cursors.removeOne(cursor);
  delete cursor;
}

void TextDocumentImpl::insertBlock(Position pos, const TextBlock & block)
{
  TextBlockImpl *newblock = new TextBlockImpl{ block.text().mid(pos.column) };
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

  block.impl()->content.chop(block.length() - pos.column);
  block.impl()->revision += 1;

  // Update cursors
  for (int i(0); i < this->cursors.count(); ++i)
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

  Q_EMIT document->blockInserted(pos, TextBlock{ document, newblock });
  Q_EMIT document->blockCountChanged(this->lineCount);
  Q_EMIT document->contentsChanged();
}

void TextDocumentImpl::insertChar(Position pos, const TextBlock & block, const QChar & c)
{
  block.impl()->content.insert(pos.column, c);
  block.impl()->revision += 1;

  for (int i(0); i < this->cursors.count(); ++i)
  {
    if (this->cursors[i]->pos.line == pos.line && this->cursors[i]->pos.column >= pos.column)
      this->cursors[i]->pos.column += 1;

    if (this->cursors[i]->anchor.line == pos.line && this->cursors[i]->anchor.column >= pos.column)
      this->cursors[i]->anchor.column += 1;
  }

  Q_EMIT document->contentsChange(block, pos, 0, 1);
  Q_EMIT document->contentsChanged();
}

void TextDocumentImpl::insertText(Position pos, const TextBlock & block, const QString & str)
{
  block.impl()->content.insert(pos.column, str);

  for (int i(0); i < this->cursors.count(); ++i)
  {
    if (this->cursors[i]->pos.line == pos.line && this->cursors[i]->pos.column >= pos.column)
      this->cursors[i]->pos.column += str.length();

    if (this->cursors[i]->anchor.line == pos.line && this->cursors[i]->anchor.column >= pos.column)
      this->cursors[i]->anchor.column += str.length();
  }

  Q_EMIT document->contentsChange(block, pos, 0, str.length());
  Q_EMIT document->contentsChanged();
}

void TextDocumentImpl::deleteChar(Position pos, const TextBlock & block)
{
  if (pos.column == block.length())
  {
    if (block == document->lastBlock())
      return;

    const int blocklength = block.length();

    TextBlock next = block.next();
    block.impl()->content.append(next.text());
    if (next == document->lastBlock())
    {
      this->lastBlock = block.impl();
      block.impl()->next = nullptr;
    }
    else
    {
      next.next().impl()->previous = block.impl();
      block.impl()->next = next.next().impl();
    }

    block.impl()->setGarbage();
    this->lineCount -= 1;
    Q_EMIT document->blockDestroyed(pos.line + 1, next);
    next.impl()->previous = nullptr;
    next.impl()->next = nullptr;

    // Update cursors
    for (int i(0); i < this->cursors.count(); ++i)
    {
      TextCursorImpl *c = this->cursors[i];

      if (c->pos.line == pos.line + 1)
      {
        c->pos.line -= 1;
        c->pos.column += blocklength;
        c->block = block;
      }
      else if (c->pos.line > pos.line + 1)
      {
        c->pos.line -= 1;
      }

      if (c->anchor.line == pos.line + 1)
      {
        c->anchor.line -= 1;
        c->anchor.column += blocklength;
      }
      else if (c->anchor.line > pos.line + 1)
      {
        c->anchor.line -= 1;
      }
    }

    Q_EMIT document->contentsChange(block, pos, 0, next.text().length());
  }
  else
  {
    block.impl()->content.remove(pos.column, 1);

    // Update cursors
    for (int i(0); i < this->cursors.count(); ++i)
    {
      TextCursorImpl *c = this->cursors[i];

      if (c->pos.line == pos.line && c->pos.column >= pos.column)
        c->pos.column -= 1;

      if (c->anchor.line == pos.line && c->anchor.column >= pos.column)
        c->anchor.column -= 1;
    }

    Q_EMIT document->contentsChange(block, pos, 1, 0);
  }

  Q_EMIT document->contentsChanged();
}

void TextDocumentImpl::deletePreviousChar(Position pos, const TextBlock & block)
{
  if (pos.column == 0)
  {
    if (block == document->firstBlock())
      return;

    TextBlock prev = block.previous();
    const int prevlength = prev.length();

    prev.impl()->content.append(block.text());
    const int charsAdded = block.text().length();

    if (block == document->lastBlock())
    {
      this->lastBlock = prev.impl();
      prev.impl()->next = nullptr;
    }
    else
    {
      block.next().impl()->previous = prev.impl();
      prev.impl()->next = block.next().impl();
    }

    block.impl()->setGarbage();
    this->lineCount -= 1;
    Q_EMIT document->blockDestroyed(pos.line, block);
    block.impl()->previous = nullptr;
    block.impl()->next = nullptr;

    // Update cursors
    for (int i(0); i < this->cursors.count(); ++i)
    {
      TextCursorImpl *c = this->cursors[i];

      if (c->pos.line == pos.line)
      {
        c->pos.line -= 1;
        c->pos.column += prevlength;
        c->block = prev;
      }
      else if (c->pos.line > pos.line)
      {
        c->pos.line -= 1;
      }

      if (c->anchor.line == pos.line)
      {
        c->anchor.line -= 1;
        c->anchor.column += prevlength;
      }
      else if (c->anchor.line > pos.line)
      {
        c->anchor.line -= 1;
      }
    }

    Q_EMIT document->contentsChange(prev, Position{pos.line - 1, prevlength }, 0, charsAdded);
  }
  else
  {
    block.impl()->content.remove(pos.column - 1, 1);

    // Update cursors
    for (int i(0); i < this->cursors.count(); ++i)
    {
      TextCursorImpl *c = this->cursors[i];

      if (c->pos.line == pos.line && c->pos.column >= pos.column)
        c->pos.column -= 1;

      if (c->anchor.line == pos.line && c->anchor.column >= pos.column)
        c->anchor.column -= 1;
    }

    Q_EMIT document->contentsChange(block, Position{ pos.line, pos.column - 1 }, 1, 0);
  }

  Q_EMIT document->contentsChanged();
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

void TextDocumentImpl::remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count)
{
  beginBlock.impl()->content.remove(begin.column, count);

  // update cursors
  auto update_cursor = [&begin, count](Position & pos) -> void
  {
    if (pos.line != begin.line)
      return;

    if (pos.column >= begin.column && pos.column <= begin.column + count)
      pos.column = begin.column;
    else if (pos.column > begin.column + count)
      pos.column -= count;
  };

  for (int i(0); i < this->cursors.count(); ++i)
  {
    TextCursorImpl *c = this->cursors[i];
    update_cursor(c->pos);
    update_cursor(c->anchor);
  }

  Q_EMIT document->contentsChange(beginBlock, begin, count, 0);
  Q_EMIT document->contentsChanged();
}

void TextDocumentImpl::remove_selection_multiline(const Position begin, const TextBlock & beginBlock, const Position end)
{
  Q_ASSERT(begin.line < end.line);

  const int count = end.line - begin.line;

  // erase content on the first line
  const int charsRemoved = beginBlock.length() - begin.column;
  beginBlock.impl()->content.chop(charsRemoved);

  // erase all middle lines
  TextBlock it = beginBlock;
  for (int i(1); i <= count; ++i)
  {
    it = it.next();
    it.impl()->setGarbage();
    Q_EMIT document->blockDestroyed(begin.line + i, it);
  }

  // append text of the last line
  TextBlock endBlock = it;
  QString appendText = endBlock.text().mid(end.column);
  beginBlock.impl()->content.append(appendText);
  const int charsAdded = appendText.length();

  // update block previous & next
  if (endBlock == document->lastBlock())
  {
    beginBlock.impl()->next = nullptr;
    this->lastBlock = beginBlock.impl();
  }
  else
  {
    beginBlock.impl()->next = endBlock.next().impl();
    endBlock.next().impl()->previous = beginBlock.impl();
  }

  // update cursors
  auto update_cursor = [&begin, &end](Position & pos) -> void
  {
    if (pos.line == begin.line && pos.column > begin.column)
    {
      pos.column = begin.column;
    }
    else if (pos.line > begin.line && pos.line < end.line)
    {
      pos.line = begin.line;
      pos.column = begin.column;
    }
    else if (pos.line == end.line)
    {
      pos.line = begin.line;
      if (pos.column <= end.column)
      {
        pos.column = begin.column;
      }
      else
      {
        pos.column = pos.column - end.column + begin.column;
      }
    }
    else if (pos.line > end.line)
    {
      pos.line -= (end.line - begin.line);
    }
  };

  for (int i(0); i < this->cursors.count(); ++i)
  {
    TextCursorImpl *c = this->cursors[i];

    update_cursor(c->pos);
    if (c->pos.line == begin.line)
      c->block = beginBlock;

    update_cursor(c->anchor);
  }

  Q_EMIT document->contentsChange(beginBlock, begin, charsRemoved, charsAdded);
  Q_EMIT document->contentsChanged();
}


TextDocument::TextDocument()
  : d(new TextDocumentImpl(this))
{

}

TextDocument::TextDocument(const QString & text)
  : d(new TextDocumentImpl(this))
{
  TextCursor{ this }.insertText(text);
}

TextDocument::~TextDocument()
{

}

const QString & TextDocument::text(int line) const
{
  return findBlockByNumber(line).text();
}

QString TextDocument::toString() const
{
  QStringList lines;
  lines.reserve(lineCount());

  auto it = firstBlock();
  do
  {
    lines.append(it.text());
    it = it.next();
  } while (it.isValid());

  return lines.join(QChar('\n'));
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
    pos.column = block.previous().length();
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

int TextDocument::lineCount() const
{
  return d->lineCount;
}

} // namespace textedit
