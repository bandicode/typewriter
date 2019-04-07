// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textcursor.h"
#include "textedit/private/textcursor_p.h"

#include "textedit/textblock.h"
#include "textedit/textdocument.h"
#include "textedit/private/textdocument_p.h"

namespace textedit
{

TextCursorImpl::TextCursorImpl(const TextBlock & firstBlock)
  : ref(0)
  , block(firstBlock)
{
  pos.line = 0;
  pos.column = 0;

  anchor = pos;
}

TextCursorImpl::TextCursorImpl(const Position & p, const TextBlock & b)
  : ref(0)
  , block(b)
  , pos(p)
{
  anchor = pos;
}

TextCursor::TextCursor()
  : mDocument(nullptr)
  , mImpl(nullptr)
{

}

TextCursor::TextCursor(const TextCursor & other)
  : mDocument(other.mDocument)
  , mImpl(other.mImpl)
{
  if (mImpl)
  {
    mImpl->ref++;
  }
}

TextCursor::TextCursor(TextCursor && other)
  : mDocument(other.mDocument)
  , mImpl(other.mImpl)
{
  other.mDocument = nullptr;
  other.mImpl = nullptr;
}

TextCursor::~TextCursor()
{
  if (mImpl)
  {
    if (--mImpl->ref == 0)
      mDocument->impl()->destroyCursor(mImpl);
  }
}

TextCursor::TextCursor(TextDocument *document)
  : mDocument(document)
  , mImpl(nullptr)
{
  if (mDocument)
    mImpl = mDocument->impl()->createCursor(Position{ 0, 0 }, document->firstBlock());
}

TextCursor::TextCursor(const TextBlock & block)
  : mDocument(nullptr)
  , mImpl(nullptr)
{
  if (!block.isValid())
    return;

  mDocument = block.document()->impl()->document;
  mImpl = mDocument->impl()->createCursor(Position{ block.blockNumber(), 0 }, block);
}

bool TextCursor::isNull() const
{
  return mImpl == nullptr;
}

bool TextCursor::isCopyOf(const TextCursor & other) const
{
  return mImpl == other.mImpl;
}

TextDocument* TextCursor::document() const
{
  return mDocument;
}

const Position & TextCursor::position() const
{
  return mImpl->pos;
}

int TextCursor::offset() const
{
  return position().column + document()->impl()->blockOffset(block().impl());
}

const Position & TextCursor::anchor() const
{
  return mImpl->anchor;
}

void TextCursor::setPosition(const Position & pos, MoveMode mode)
{
  detach();

  if (pos.line >= document()->lineCount() || (pos.line == document()->lineCount() - 1 && pos.column >= document()->lastBlock().length()))
  {
    mImpl->pos.line = document()->lineCount() - 1;
    mImpl->pos.column = document()->lastBlock().length();
    mImpl->block = document()->lastBlock();
  }
  else if (pos.line < 0 || (pos.line == 0 && pos.column < 0))
  {
    mImpl->pos.line = 0;
    mImpl->pos.column = 0;
    mImpl->block = document()->firstBlock();
  }
  else 
  {
    mImpl->block = next(mImpl->block, pos.line - mImpl->pos.line);
    mImpl->pos.line = pos.line;
    mImpl->pos.column = std::min(std::max(0, pos.column), mImpl->block.length());
  }

  if (mode == MoveAnchor)
    mImpl->anchor = mImpl->pos;
}

class MoveGuard
{
public:
  TextCursorImpl * cursor;
  TextCursor::MoveMode mode;

  ~MoveGuard()
  {
    if (mode == TextCursor::MoveAnchor)
    {
      cursor->anchor = cursor->pos;
    }
  }
};

bool TextCursorImpl::move_down(int n)
{
  const int count = std::min(n, this->block.document()->lineCount() - 1 - this->pos.line);

  for (int i(0); i < count; ++i)
  {
    this->pos.line += 1;
    this->block = this->block.next();
  }

  if (this->pos.column > this->block.length())
    this->pos.column = this->block.length();

  return count == n;
}

bool TextCursorImpl::move_left(int n)
{
  this->pos.column -= n;

  while (this->pos.column < 0 && this->pos.line > 0)
  {
    this->block = this->block.previous();
    this->pos.column += this->block.length() + 1;
    this->pos.line -= 1;
  }

  if (this->pos.column < 0)
  {
    this->pos.column = 0;
    return false;
  }

  return true;
}

bool TextCursorImpl::move_right(int n)
{
  const int maxline = this->block.document()->lineCount() - 1;

  this->pos.column += n;

  while (this->pos.column > this->block.length() && this->pos.line < maxline)
  {
    this->pos.column -= this->block.length() + 1;
    this->block = this->block.next();
    this->pos.line += 1;
  }

  if (this->pos.column > this->block.length())
  {
    this->pos.column = this->block.length();
    return false;
  }

  return true;
}

bool TextCursorImpl::move_up(int n)
{
  const int count = std::min(n, this->pos.line);

  for (int i(0); i < count; ++i)
  {
    this->pos.line -= 1;
    this->block = this->block.previous();
  }

  if (this->pos.column > this->block.length())
    this->pos.column = this->block.length();

  return count == n;
}

bool TextCursor::movePosition(MoveOperation operation, MoveMode mode, int n)
{
  MoveGuard anchor_move{ mImpl, mode };

  switch (operation)
  {
  case NoMove:
    return true;
  case Down:
    return mImpl->move_down(n);
  case Left:
    return mImpl->move_left(n);
  case Right:
    return mImpl->move_right(n);
  case Up:
    return mImpl->move_up(n);
  default:
    return false;
  }
}

bool TextCursor::atEnd() const
{
  return block() == document()->lastBlock() && position().column == block().length();
}

bool TextCursor::atStart() const
{
  return position() == Position{ 0, 0 };
}

TextBlock TextCursor::block() const
{
  return isNull() ? TextBlock{} : mImpl->block;
}

bool TextCursor::atBlockEnd() const
{
  return position().column == block().length();
}

bool TextCursor::atBlockStart() const
{
  return position().column == 0;
}

Position TextCursor::selectionStart() const
{
  return anchor() < position() ? anchor() : position();
}

Position TextCursor::selectionEnd() const
{
  return anchor() < position() ? position() : anchor();
}

bool TextCursor::hasSelection() const
{
  return position() != anchor();
}

QString TextCursor::selectedText() const
{
  if (!hasSelection())
    return QString();

  auto start = selectionStart();
  auto end = selectionEnd();

  if (start.line == end.line)
    return block().text().mid(start.column, end.column - start.column);

  TextBlock b = start == position() ? block() : prev(block(), end.line - start.line);

  QString result = b.text().mid(start.column);

  for (int i(start.line + 1); i < end.line; ++i)
  {
    b = b.next();
    result.append("\n").append(b.text());
  }

  b = b.next();
  result.append("\n").append(b.text().left(end.column));

  return result;
}

void TextCursor::clearSelection()
{
  Q_ASSERT(!document()->isReadOnly());

  detach();
  mImpl->anchor = mImpl->pos;
}

void TextCursor::removeSelectedText()
{
  Q_ASSERT(!document()->isReadOnly());

  detach();
  document()->impl()->removeSelection(position(), block(), anchor());
}


void TextCursor::beginEdit()
{
  /// TODO
  throw std::runtime_error{ "Not Implemented" };
}

void TextCursor::endEdit()
{
  /// TODO
  throw std::runtime_error{ "Not Implemented" };
}


void TextCursor::undo()
{
  /// TODO
  throw std::runtime_error{ "Not Implemented" };
}

void TextCursor::redo()
{
  /// TODO
  throw std::runtime_error{ "Not Implemented" };
}


void TextCursor::deleteChar()
{
  Q_ASSERT(!document()->isReadOnly());

  if (hasSelection())
  {
    removeSelectedText();
    return;
  }

  detach();

  mDocument->impl()->deleteChar(position(), block());
}

void TextCursor::deletePreviousChar()
{
  Q_ASSERT(!document()->isReadOnly());

  if (hasSelection())
  {
    removeSelectedText();
    return;
  }

  detach();

  mDocument->impl()->deletePreviousChar(position(), block());
}

void TextCursor::insertBlock()
{
  Q_ASSERT(!document()->isReadOnly());

  detach();
  
  if (hasSelection())
    removeSelectedText();

  mDocument->impl()->insertBlock(position(), block());
}

void TextCursor::insertText(const QString & text)
{
  Q_ASSERT(!document()->isReadOnly());

  detach();

  if (hasSelection())
    removeSelectedText();

  QStringList lines = text.split(QChar('\n'));
  for (auto & l : lines)
  {
    if (l.endsWith(QChar('\r')))
      l.chop(1);
  }

  for (int i(0); i < lines.count(); ++i)
  {
    mDocument->impl()->insertText(position(), block(), lines.at(i));

    if (i != lines.count() - 1)
      insertBlock();
  }
}

void TextCursor::insertChar(const QChar & c)
{
  Q_ASSERT(!document()->isReadOnly());

  detach();

  if (hasSelection())
    removeSelectedText();

  mDocument->impl()->insertChar(position(), block(), c);
}


void TextCursor::swap(TextCursor & other)
{
  TextCursorImpl *d = mImpl;
  TextDocument *doc = mDocument;

  mImpl = other.mImpl;
  mDocument = other.mDocument;

  other.mImpl = d;
  other.mDocument = doc;
}

void TextCursor::detach()
{
  if (mImpl->ref == 1)
    return;

  TextCursorImpl *self = mImpl;
  self->ref -= 1;

  mImpl = mDocument->impl()->createCursor(position(), block());
  mImpl->anchor = self->anchor;
}

TextCursor & TextCursor::operator=(const TextCursor & other)
{
  if (this == &other)
    return *this;

  if (mImpl)
  {
    if(--mImpl->ref == 0)
      mDocument->impl()->destroyCursor(mImpl);
  }

  mDocument = other.mDocument;
  mImpl = other.mImpl;
  if (mImpl)
    mImpl->ref++;

  return *this;
}

TextCursor & TextCursor::operator=(TextCursor && other)
{
  if (this == &other)
    return *this;

  if (mImpl)
  {
    if (--mImpl->ref == 0)
      mDocument->impl()->destroyCursor(mImpl);
  }

  mDocument = other.mDocument;
  mImpl = other.mImpl;

  other.mDocument = nullptr;
  other.mImpl = nullptr;

  return *this;
}

bool TextCursor::operator<(const TextCursor & other) const
{
  return position() < other.position();
}

} // namespace textedit
