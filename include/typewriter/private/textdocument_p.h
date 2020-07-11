// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTDOCUMENT_P_H
#define TYPEWRITER_TEXTDOCUMENT_P_H

#include "typewriter/textdiff.h"

#include "typewriter/private/textblock_p.h"
#include "typewriter/private/textcursor_p.h"

#include <unicode/unicode.h>

#include <vector>

namespace typewriter
{

class TextDocument;

class TYPEWRITER_API TextDocumentImpl
{
public:
  TextDocument *document;
  int lineCount;
  TextBlockRef firstBlock;
  TextBlockRef lastBlock;

  std::vector<TextCursorImpl*> cursors;

  std::vector<std::unique_ptr<TextDocumentListener>> listeners;

  int idgen;


public:
  TextDocumentImpl(TextDocument *doc);
  ~TextDocumentImpl();

  int blockNumber(TextBlockImpl *block) const;
  int blockOffset(TextBlockImpl *block) const;

  TextCursorImpl* createCursor(const Position & pos, const TextBlock & b);
  void destroyCursor(TextCursorImpl *cursor);

  void insertBlock(Position pos, const TextBlock & block);
  void insertChar(Position pos, const TextBlock & block, unicode::Character c);
  void insertText(Position pos, const TextBlock & block, const std::string& str);
  void deleteChar(Position pos, const TextBlock & block);
  void deletePreviousChar(Position pos, const TextBlock & block);
  void removeSelection(const Position begin, const TextBlock & beginBlock, const Position end);

protected:
  void remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count);
  void remove_selection_multiline(const Position begin, const TextBlock & beginBlock, const Position end);

  void remove_block(int blocknum, TextBlock block);
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTDOCUMENT_P_H
