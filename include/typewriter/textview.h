// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTVIEW_H
#define TYPEWRITER_TEXTVIEW_H

#include "typewriter/textdocument.h"
#include "typewriter/view/inserts.h"
#include "typewriter/view/line.h"
#include "typewriter/utils/range.h"

#include <list>
#include <map>

namespace typewriter
{

namespace view
{
class Fragment;
class Block;
class Line;
} // namespace view

class TextViewImpl;

class TYPEWRITER_API TextView : public TextDocumentListener
{
public:
  TextView(TextDocument *document);
  ~TextView();

  TextDocument* document() const;

  int tabSize() const;
  void setTabSize(int n);

  int charactersPerLine() const;
  void setCharactersPerLine(int n);

  int height() const;
  int width() const;

  const std::list<view::LineInfo>& lines() const;

  enum class WrapMode
  {
    NoWrap,
    Word,
    Anywhere,
    WordBoundaryOrAnywhere,
  };

  WrapMode wrapMode() const;
  void setWrapMode(WrapMode wm);

  void addFold(int id, TextCursor sel, int w = 3);
  void removeFold(int id);
  void clearFolds();

  void addInsert(view::Insert ins);
  void addInlineInsert(view::InlineInsert ins);
  void clearInserts();

  const std::vector<view::Insert>& inserts() const;
  const std::vector<view::InlineInsert>& inlineInserts() const;

  inline TextViewImpl* impl() const { return d.get(); }

protected:
  void blockDestroyed(int line, const TextBlock & block) override;
  void blockInserted(const Position & pos, const TextBlock & block) override;
  void contentsChange(const TextBlock & block, const Position & pos, int charsRemoved, int charsAdded) override;

protected:
  explicit TextView(std::unique_ptr<TextViewImpl> && impl);

private: 
  void init();

protected:
  std::unique_ptr<TextViewImpl> d;
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTVIEW_H
