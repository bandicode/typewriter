// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTVIEW_P_H
#define TEXTEDIT_TEXTVIEW_P_H

#include "textedit/textedit.h"

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
  Line endline;

  ActiveFold();
  ActiveFold(const ActiveFold &) = default;
  ActiveFold(const Position & b, const Position & e, const Line & el);
};

} // namespace view

class TextDocument;

class TextViewImpl
{
public:
  
  QLinkedList<view::TextLine> lines;
  view::Line firstLine;
  TextBlock longestLine;

  QFont font;
  view::Metrics metrics;

  TextFormat defaultFormat;
  QString tabreplace;

  QList<view::ActiveFold> activeFolds;

  QList<TextView::ExtraSelection> extraSelections;

  QRect viewport;
  QScrollBar *hscrollbar;
  Qt::ScrollBarPolicy hpolicy;
  QScrollBar *vscrollbar;
  Qt::ScrollBarPolicy vpolicy;

  SyntaxHighlighter *syntaxHighlighter;
  Position firstDirtyLine; // first line that needs highlighting

public:
  TextViewImpl(const TextDocument *doc);

  void calculateMetrics(const QFont & f);

  /// TODO: take into account tabreplace
  TextBlock findLongestLine() const;
  void setLongestLine(const TextBlock & block);

  /// TODO: highlightUpToLine(view::Line l);
  // similar to checkNeedsHighlighting
  void seekFirstDirtyLine(view::Line previous);
  bool checkNeedsHighlighting(view::Line l);
  void highlightLine(view::Line l);
  int invokeSyntaxHighlighter(view::Line l);
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTVIEW_P_H
