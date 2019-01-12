// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTVIEW_P_H
#define TEXTEDIT_TEXTVIEW_P_H

#include "textedit/textedit.h"

#include "textedit/gutter.h"
#include "textedit/view/block.h"
#include "textedit/view/incrusted-widget.h"
#include "textedit/view/line.h"
#include "textedit/view/metrics.h"
#include "textedit/syntaxhighlighter.h"
#include "textedit/textview.h"

#include <QFont>
#include <QLinkedList>
#include <QVector>

namespace textedit
{

namespace view
{

struct ActiveFold 
{
  Position begin;
  Position end;

  ActiveFold();
  ActiveFold(const ActiveFold &) = default;
  ActiveFold(const Position & b, const Position & e);
};

} // namespace view

class TextDocument;

class TextViewImpl
{
public:
  
  view::BlockInfoList blocks;
  view::Line firstLine;
  view::Line longestLine;
  int linecount;

  QFont font;
  view::Metrics metrics;

  TextFormat defaultFormat;
  QString tabreplace;

  QList<view::ActiveFold> activeFolds;

  QRect viewport;
  QScrollBar *hscrollbar;
  Qt::ScrollBarPolicy hpolicy;
  QScrollBar *vscrollbar;
  Qt::ScrollBarPolicy vpolicy;

  SyntaxHighlighter *syntaxHighlighter;
  Position firstDirtyBlock; // first line that needs highlighting

  Gutter *gutter;

  QList<view::IncrustedWidget> widgets;

public:
  TextViewImpl(const TextDocument *doc);

  void calculateMetrics(const QFont & f);

  /// TODO: take into account tabreplace
  view::Line findLongestLine();
  void setLongestLine(const view::Line & line);

  int getFold(int blocknum, int from = 0) const;
  void relayout(int blocknum);

  /// TODO: highlightUpToLine(view::Block l);
  // similar to checkNeedsHighlighting
  void seekFirstDirtyLine(view::Block previous);
  bool checkNeedsHighlighting(view::Block l);
  void highlightLine(view::Block l);
  int invokeSyntaxHighlighter(view::Block l);

  inline view::BlockInfo & blockInfo(int n) const { return *blocks.at(n); }
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTVIEW_P_H
