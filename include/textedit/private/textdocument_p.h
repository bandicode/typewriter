// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTDOCUMENT_P_H
#define TEXTEDIT_TEXTDOCUMENT_P_H

#include "textedit/textedit.h"

#include "textedit/textdiff.h"

#include "textedit/private/textblock_p.h"
#include "textedit/private/textcursor_p.h"

#include <QList>

namespace textedit
{

class TextDocument;

class TEXTEDIT_API TextDocumentImpl
{
public:
  TextDocument *document;
  int lineCount;
  TextBlockRef firstBlock;
  TextBlockRef lastBlock;

  QList<TextCursorImpl*> cursors;
  bool readOnly;

  int idgen;


public:
  TextDocumentImpl(TextDocument *doc);
  ~TextDocumentImpl();

  int blockNumber(TextBlockImpl *block) const;
  int blockOffset(TextBlockImpl *block) const;

  TextCursorImpl* createCursor(const Position & pos, const TextBlock & b);
  void destroyCursor(TextCursorImpl *cursor);

  void insertBlock(Position pos, const TextBlock & block);
  void insertChar(Position pos, const TextBlock & block, const QChar & c);
  void insertText(Position pos, const TextBlock & block, const QString & str);
  void deleteChar(Position pos, const TextBlock & block);
  void deletePreviousChar(Position pos, const TextBlock & block);
  void removeSelection(const Position begin, const TextBlock & beginBlock, const Position end);

protected:
  void remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count);
  void remove_selection_multiline(const Position begin, const TextBlock & beginBlock, const Position end);

  void remove_block(int blocknum, TextBlock block);
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTDOCUMENT_P_H
