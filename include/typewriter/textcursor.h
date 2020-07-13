// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTCURSOR_H
#define TYPEWRITER_TEXTCURSOR_H

#include "typewriter/textblock.h"

#include <unicode/unicode.h>

#include <string>

namespace typewriter
{

class TextBlock;
class TextDocument;

class TYPEWRITER_API TextCursor
{
public:
  TextCursor();
  TextCursor(const TextCursor & other);
  TextCursor(TextCursor && other);
  ~TextCursor();

  TextCursor(TextDocument* document);
  TextCursor(const TextBlock& block);

  bool isNull() const;

  TextDocument * document() const;

  enum MoveMode {
    MoveAnchor = 0,
    KeepAnchor = 1,
  };

  enum MoveOperation {
    NoMove = 0,
    Down = 12,
    Left = 9,
    Right = 19,
    Up = 2,
  };

  const Position & position() const;
  int offset() const;
  const Position & anchor() const;
  void setPosition(const Position & pos, MoveMode mode = MoveAnchor);
  bool movePosition(MoveOperation operation, MoveMode mode = MoveAnchor, int n = 1);

  bool atEnd() const;
  bool atStart() const;

  TextBlock block() const;
  bool atBlockEnd() const;
  bool atBlockStart() const;

  Position selectionStart() const;
  Position selectionEnd() const;
  bool hasSelection() const;
  std::string selectedText() const;
  void clearSelection();
  void removeSelectedText();

  void beginEdit();
  void endEdit();

  void undo();
  void redo();

  void deleteChar();
  void deletePreviousChar();
  void insertBlock();
  void insertText(const std::string & text);
  void insertChar(const unicode::Character c);

  void swap(TextCursor & other);
  void detach();

  TextCursor & operator=(const TextCursor& other);
  TextCursor & operator=(TextCursor&& other);
  bool operator<(const TextCursor & other) const;

protected:
  bool move_up(int n);
  bool move_down(int n);
  bool move_left(int n);
  bool move_right(int n);

private:
  friend class TextDocument;
  friend class TextDocumentImpl;
  TextDocument *m_document;
  TextBlock m_block;
  Position m_pos;
  Position m_anchor;
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTCURSOR_H
