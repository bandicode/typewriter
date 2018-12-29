// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTCURSOR_H
#define TEXTEDIT_TEXTCURSOR_H

#include "textedit/textedit.h"

#include <QString>

namespace textedit
{

class TextBlock;
class TextDocument;

class TextCursorImpl;

class TEXTEDIT_API TextCursor
{
public:
  TextCursor();
  TextCursor(const TextCursor & other);
  TextCursor(TextCursor && other);
  ~TextCursor();

  TextCursor(TextDocument *document);
  TextCursor(const TextBlock & block);

  bool isNull() const;
  bool isCopyOf(const TextCursor & other) const;

  TextDocument * document() const;

  enum MoveMode {
    MoveAnchor = 0,
    KeepAnchor = 1,
  };

  enum MoveOperation {
    NoMove = 0,
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
  QString selectedText() const;
  void removeSelectedText();

  void beginEdit();
  void endEdit();

  void undo();
  void redo();

  void deleteChar();
  void deletePreviousChar();
  void insertBlock();
  void insertText(const QString & text);
  void insertChar(const QChar & c);

  void swap(TextCursor & other);
  void detach();

  TextCursor & operator=(const TextCursor & other);
  TextCursor & operator=(TextCursor && other);
  bool operator<(const TextCursor & other) const;

private:
  TextCursorImpl *mImpl;
  TextDocument *mDocument;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTCURSOR_H
