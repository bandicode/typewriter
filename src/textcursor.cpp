// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/textcursor.h"

#include "typewriter/textblock.h"
#include "typewriter/textdocument.h"
#include "typewriter/private/textdocument_p.h"

#include <algorithm>
#include <stdexcept>

namespace typewriter
{

struct CursorTransaction
{
  TextCursor* cursor;
  bool has_opened_transaction = false;

  CursorTransaction(TextCursor* c)
    : cursor(c)
  {
    auto* d = c->document()->impl();

    if (!d->transaction.is_active() && !d->cursors_are_ghosts)
    {
      has_opened_transaction = true;
      c->beginEdit();
    }
  }

  ~CursorTransaction()
  {
    if (has_opened_transaction)
      cursor->endEdit();
  }
};


TextCursor::TextCursor()
  : m_document(nullptr)
{

}

TextCursor::TextCursor(const TextCursor& other)
  : m_document(other.m_document),
    m_block(other.m_block),
    m_pos(other.m_pos),
    m_anchor(other.m_anchor)
{
  if (m_document)
  {
    m_document->impl()->register_cursor(this);
  }
}

TextCursor::TextCursor(TextCursor&& other)
  : m_document(other.m_document),
    m_block(other.m_block),
    m_pos(other.m_pos),
    m_anchor(other.m_anchor)
{
  if (m_document)
  {
    m_document->impl()->swap_cursor(&other, this);
    other.m_document = nullptr;
  }
}

TextCursor::~TextCursor()
{
  if (m_document)
  {
    m_document->impl()->deregister_cursor(this);
  }
}

TextCursor::TextCursor(TextDocument *document)
  : m_document(document)
{
  if (m_document)
  {
    m_block = document->firstBlock();
    m_pos = Position{ 0, 0 };
    m_anchor = m_pos;
    
    document->impl()->register_cursor(this);
  }
}

TextCursor::TextCursor(const TextBlock& block)
  : m_document(nullptr)
{
  if (!block.isValid())
    return;

  m_document = block.document()->impl()->document;
  m_block = block;
  m_pos = Position{ block.blockNumber(), 0 };
  m_anchor = m_pos;

  m_document->impl()->register_cursor(this);
}

bool TextCursor::isNull() const
{
  return m_document == nullptr;
}

TextDocument* TextCursor::document() const
{
  return m_document;
}

const Position & TextCursor::position() const
{
  return m_pos;
}

int TextCursor::offset() const
{
  return position().column + document()->impl()->blockOffset(block().impl());
}

const Position & TextCursor::anchor() const
{
  return m_anchor;
}

void TextCursor::setPosition(const Position & pos, MoveMode mode)
{
  if (pos.line >= document()->lineCount() || (pos.line == document()->lineCount() - 1 && pos.column >= document()->lastBlock().length()))
  {
    m_pos.line = document()->lineCount() - 1;
    m_pos.column = document()->lastBlock().length();
    m_block = document()->lastBlock();
  }
  else if (pos.line < 0 || (pos.line == 0 && pos.column < 0))
  {
    m_pos.line = 0;
    m_pos.column = 0;
    m_block = document()->firstBlock();
  }
  else 
  {
    m_block = next(m_block, pos.line - m_pos.line);
    m_pos.line = pos.line;
    m_pos.column = std::min({ std::max({ 0, pos.column }), m_block.length() });
  }

  if (mode == MoveAnchor)
    m_anchor = m_pos;
}

class MoveGuard
{
public:
  Position* anchor;
  Position* pos;
  TextCursor::MoveMode mode;

  ~MoveGuard()
  {
    if (mode == TextCursor::MoveAnchor)
    {
      *anchor = *pos;
    }
  }
};

bool TextCursor::move_down(int n)
{
  const int count = std::min(n, m_block.document()->lineCount() - 1 - m_pos.line);

  for (int i(0); i < count; ++i)
  {
    m_pos.line += 1;
    m_block = m_block.next();
  }

  if (m_pos.column > m_block.length())
    m_pos.column = m_block.length();

  return count == n;
}

bool TextCursor::move_left(int n)
{
  m_pos.column -= n;

  while (m_pos.column < 0 && m_pos.line > 0)
  {
    m_block = m_block.previous();
    m_pos.column += m_block.length() + 1;
    m_pos.line -= 1;
  }

  if (m_pos.column < 0)
  {
    m_pos.column = 0;
    return false;
  }

  return true;
}

bool TextCursor::move_right(int n)
{
  const int maxline = m_block.document()->lineCount() - 1;

  m_pos.column += n;

  while (m_pos.column > m_block.length() && m_pos.line < maxline)
  {
    m_pos.column -= m_block.length() + 1;
    m_block = m_block.next();
    m_pos.line += 1;
  }

  if (m_pos.column > m_block.length())
  {
    m_pos.column = m_block.length();
    return false;
  }

  return true;
}

bool TextCursor::move_up(int n)
{
  const int count = std::min(n, m_pos.line);

  for (int i(0); i < count; ++i)
  {
    m_pos.line -= 1;
    m_block = m_block.previous();
  }

  if (m_pos.column > m_block.length())
    m_pos.column = m_block.length();

  return count == n;
}

bool TextCursor::movePosition(MoveOperation operation, MoveMode mode, int n)
{
  MoveGuard anchor_move{ &m_anchor, &m_pos, mode };

  switch (operation)
  {
  case NoMove:
    return true;
  case Down:
    return move_down(n);
  case Left:
    return move_left(n);
  case Right:
    return move_right(n);
  case Up:
    return move_up(n);
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
  return isNull() ? TextBlock{} : m_block;
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

std::string TextCursor::selectedText() const
{
  if (!hasSelection())
    return std::string();

  auto start = selectionStart();
  auto end = selectionEnd();

  if (start.line == end.line)
    return block().text().substr(start.column, end.column - start.column);

  TextBlock b = start == position() ? block() : prev(block(), end.line - start.line);

  std::string result = b.text().substr(start.column);

  for (int i(start.line + 1); i < end.line; ++i)
  {
    b = b.next();
    result.push_back('\n');
    result.insert(result.end(), b.text().begin(), b.text().end());
  }

  b = b.next();
  result.push_back('\n');
  result.insert(result.end(), b.text().end() - end.column, b.text().end());

  return result;
}

void TextCursor::clearSelection()
{
  m_anchor = m_pos;
}

void TextCursor::removeSelectedText()
{
  CursorTransaction tr{ this };
  document()->impl()->removeSelection(position(), block(), anchor());
}

void TextCursor::beginEdit()
{
  document()->impl()->beginTransaction(this);
}

void TextCursor::endEdit()
{
  document()->impl()->endTransaction(this);
}

void TextCursor::undo()
{
  document()->impl()->undo(this);
}

void TextCursor::redo()
{
  document()->impl()->redo(this);
}


void TextCursor::deleteChar()
{
  if (hasSelection())
  {
    removeSelectedText();
    return;
  }

  CursorTransaction tr{ this };
  m_document->impl()->deleteChar(position(), block());
}

void TextCursor::deletePreviousChar()
{
  if (hasSelection())
  {
    removeSelectedText();
    return;
  }

  CursorTransaction tr{ this };
  m_document->impl()->deletePreviousChar(position(), block());
}

void TextCursor::insertBlock()
{
  if (hasSelection())
    removeSelectedText();

  CursorTransaction tr{ this };
  m_document->impl()->insertBlock(position(), block());
}

static std::vector<std::string> split_str(const std::string& text, char c)
{
  std::vector<std::string> result;

  size_t start = 0;
  size_t end = text.find(c);

  while (end != std::string::npos)
  {
    result.push_back(std::string(text.begin() + start, text.begin() + end));
    start = end + 1;
    end = text.find(c, start);
  }

  result.push_back(std::string(text.begin() + start, text.end()));

  return result;
}

void TextCursor::insertText(const std::string& text)
{
  CursorTransaction tr{ this };

  if (hasSelection())
    removeSelectedText();

  std::vector<std::string> lines = split_str(text, '\n');
  for (auto & l : lines)
  {
    if (!l.empty() && l.back() == '\r')
      l.pop_back();
  }

  for (size_t i(0); i < lines.size(); ++i)
  {
    m_document->impl()->insertText(position(), block(), lines.at(i));

    if (i != lines.size() - 1)
      insertBlock();
  }
}

void TextCursor::insertChar(unicode::Character c)
{
  if (hasSelection())
    removeSelectedText();

  m_document->impl()->insertChar(position(), block(), c);
}


void TextCursor::swap(TextCursor& other)
{
  if (other.m_document != m_document)
  {
    TextCursor tmp{ *this };
    *this = other;
    other = tmp;
  }
  else
  {
    std::swap(m_block, other.m_block);
    std::swap(m_pos, other.m_pos);
    std::swap(m_anchor, other.m_anchor);
  }
}

TextCursor & TextCursor::operator=(const TextCursor& other)
{
  if (this == &other)
    return *this;

  if (m_document && m_document != other.m_document)
  {
    m_document->impl()->deregister_cursor(this);
    m_document = nullptr;
  }


  if (!m_document && other.m_document)
  {
    m_document = other.m_document;
    m_document->impl()->register_cursor(this);
  }

  m_block = other.m_block;
  m_pos = other.m_pos;
  m_anchor = other.m_anchor;

  return *this;
}

TextCursor& TextCursor::operator=(TextCursor&& other)
{
  if (m_document)
  {
    if (m_document != other.document())
    {
      m_document->impl()->deregister_cursor(this);

      m_document = other.m_document;
      m_block = other.m_block;
      m_pos = other.m_pos;
      m_anchor = other.m_anchor;

      if (m_document)        
        m_document->impl()->swap_cursor(&other, this);
    }
    else
    {
      assert(m_document == other.m_document);
      m_block = other.m_block;
      m_pos = other.m_pos;
      m_anchor = other.m_anchor;

      m_document->impl()->deregister_cursor(&other);
    }
  }
  else
  {
    m_document = other.m_document;
    m_block = other.m_block;
    m_pos = other.m_pos;
    m_anchor = other.m_anchor;

    if (m_document)
      m_document->impl()->swap_cursor(&other, this);
  }

  other.m_document = nullptr;

  return *this;
}

bool TextCursor::operator<(const TextCursor & other) const
{
  return position() < other.position();
}

} // namespace typewriter
