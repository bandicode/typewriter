// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTDOCUMENT_P_H
#define TYPEWRITER_TEXTDOCUMENT_P_H

#include "typewriter/textdiff.h"

#include "typewriter/private/textblock_p.h"

#include <unicode/unicode.h>

#include <cassert>
#include <vector>

namespace typewriter
{

class Contributor;
class TextDocument;

class Author
{
public:
  Contributor* contributor = nullptr;
  TextCursor* cursor = nullptr;

  Author() = default;

  Author(Contributor* c)
    : contributor(c)
  {

  }

  Author(TextCursor* c)
    : cursor(c)
  {

  }

  bool is_null() const
  {
    return contributor == nullptr && cursor == nullptr;
  }
};

inline bool operator==(const Author& lhs, const Author& rhs)
{
  return lhs.contributor == rhs.contributor && lhs.cursor == rhs.cursor;
}

inline bool operator!=(const Author& lhs, const Author& rhs)
{
  return !(lhs == rhs);
}

struct Contribution
{
  Author author;
  TextDiff delta;
};

inline bool from_same_author(const Contribution& lhs, const Contribution& rhs)
{
  return lhs.author == rhs.author;
}

inline Contribution merge(Contribution&& lhs, Contribution&& rhs)
{
  assert(from_same_author(lhs, rhs));

  Contribution ret;
  ret.author = lhs.author;
  ret.delta = std::move(lhs.delta);
  ret.delta << rhs.delta;
  return ret;
}

struct TextDocumentTransaction
{
  Author author;
  TextDiff delta;
  int depth = 0;

  bool is_active() const
  {
    return depth > 0;
  }
};

class TYPEWRITER_API TextDocumentImpl
{
public:
  TextDocument *document;
  int lineCount;
  TextBlockRef firstBlock;
  TextBlockRef lastBlock;

  std::vector<TextCursor*> cursors;

  std::vector<std::unique_ptr<TextDocumentListener>> listeners;

  int idgen;

  TextDocumentTransaction transaction;

  std::vector<Contribution> m_undo_stack;
  std::vector<Contribution> m_redo_stack;

  bool cursors_are_ghosts = false;

public:
  TextDocumentImpl(TextDocument *doc);
  ~TextDocumentImpl();

  int blockNumber(TextBlockImpl *block) const;
  int blockOffset(TextBlockImpl *block) const;

  void register_cursor(TextCursor* c);
  void swap_cursor(TextCursor* existing_cursor, TextCursor* new_cursor) noexcept;
  void deregister_cursor(TextCursor* c) noexcept;

  void insertBlock(Position pos, const TextBlock & block);
  void insertChar(Position pos, const TextBlock & block, unicode::Character c);
  void insertText(Position pos, const TextBlock & block, const std::string& str);
  void deleteChar(Position pos, const TextBlock & block);
  void deletePreviousChar(Position pos, const TextBlock & block);
  void removeSelection(const Position begin, const TextBlock & beginBlock, const Position end);

  void beginTransaction(Author author);
  void endTransaction(Author author);

  void undo(Author author);
  void redo(Author author);

protected:
  void remove_selection_singleline(const Position begin, const TextBlock & beginBlock, int count);
  void remove_selection_multiline(const Position begin, const TextBlock & beginBlock, const Position end);

  void remove_block(int blocknum, TextBlock block);

  void apply(const TextDiff& diff, bool inv = false);
  void revert(const TextDiff& diff);
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTDOCUMENT_P_H
