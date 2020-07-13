// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTVIEW_P_H
#define TYPEWRITER_TEXTVIEW_P_H

#include "typewriter/typewriter-defs.h"

#include "typewriter/textfold.h"
#include "typewriter/view/block.h"
#include "typewriter/view/inserts.h"
#include "typewriter/textview.h"

#include <list>
#include <unordered_map>
#include <vector>

namespace typewriter
{

class TextDocument;

class TextViewImpl
{
public:
  TextDocument *document;
  
  std::unordered_map<TextBlockImpl*, std::shared_ptr<view::BlockInfo>> blocks;
  std::list<view::LineInfo> lines;
  int longest_line_length = 0;

  int cpl = -1;
  TextView::WrapMode wrapmode = TextView::WrapMode::NoWrap;
  std::string tabreplace;

  std::vector<SimpleTextFold> folds;
  std::vector<view::Insert> inserts;
  std::vector<view::InlineInsert> inline_inserts;

public:
  TextViewImpl(TextDocument *doc);

  void refreshLongestLineLength();

  //inline view::BlockInfo & blockInfo(int n) const { return *blocks.at(n); }

  inline int ncol(char c) const { return c =='\t' ? tabreplace.size() : 1; }

  static TextBlock getBlock(const view::LineInfo& l);
};

class Composer
{
private:
  TextViewImpl* view;

  enum IteratorKind
  {
    FoldIterator,
    InsertIterator,
    InlineInsertIterator,
    BlockIterator,
    LineFeedIterator,
  };

  struct Iterator
  {
    TextViewImpl* view = nullptr;
    std::vector<SimpleTextFold>::const_iterator folds;
    std::vector<view::Insert>::const_iterator inserts;
    int insert_row = 0;
    std::vector<view::InlineInsert>::const_iterator inline_inserts;
    TextBlockIterator textblock;
    int line = 0;
    IteratorKind current = BlockIterator;

    void init(TextViewImpl* v);
    void update();

    void advance();

    bool atEnd() const;
    int currentWidth() const;

    void seek(const view::LineInfo& l);
    void seek(const TextBlock& b);
  };

private:

  Iterator iterator;

  TextBlock current_block;
  std::list<view::LineInfo>::iterator line_iterator;

  std::vector<view::SimpleLineElement> current_line;
  int current_line_width = 0;

public:
  explicit Composer(TextViewImpl* v);

  void relayout();

  void relayout(TextBlock b);

  void relayout(std::list<view::LineInfo>::iterator it);

  void handleBlockInsertion(const TextBlock& b);
  void handleBlockRemoval(const TextBlock& b);

  void handleFoldInsertion(std::vector<SimpleTextFold>::iterator it);
  void handleFoldRemoval(const TextCursor& sel);

protected:
  void relayoutBlock();

protected:
  std::list<view::LineInfo>::iterator getLine(TextBlock b);
  void writeCurrentLine();
  void updateBlockLineIterator(TextBlock begin, TextBlock end);
  view::SimpleLineElement createLineElement(const Iterator& it);
  view::SimpleLineElement createCarriageReturn();
  view::SimpleLineElement createLineIndent();
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTVIEW_P_H