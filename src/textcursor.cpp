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

bool TextCursor::movePosition(MoveOperation operation, MoveMode mode, int n)
{
  switch (operation)
  {
  case NoMove:
    return true;
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
  /// TODO
  throw std::runtime_error{ "Not Implemented" };
}

void TextCursor::removeSelectedText()
{
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
  detach();
  
  if (hasSelection())
    removeSelectedText();

  mDocument->impl()->insertBlock(position(), block());
}

void TextCursor::insertText(const QString & text)
{
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
