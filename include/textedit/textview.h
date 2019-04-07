// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTVIEW_H
#define TEXTEDIT_TEXTVIEW_H

#include "textedit/textdocument.h"

#include <QWidget>

#include <QMap>
#include <QScrollBar>

namespace textedit
{

namespace view
{
class Fragment;
class Block;
class Blocks;
class Line;
class LineElements;
class Lines;
struct Metrics;
} // namespace view

class SyntaxHighlighter;
class TextFoldList;
class TextFormat;
class TextViewImpl;

class TEXTEDIT_API TextView : public QWidget
{
  Q_OBJECT
public:
  TextView(TextDocument *document);
  ~TextView();

  TextDocument * document() const;

  view::Blocks blocks() const;
  view::Lines lines() const;
  view::Lines visibleLines() const;

  void scroll(int delta);

public Q_SLOTS:
  void setFirstVisibleLine(int n);

public:
  void setFont(const QFont & f);
  const QFont & font() const;

  void setTabSize(int n);
  int tabSize() const;

  QScrollBar* horizontalScrollBar() const;
  void setHorizontalScrollBar(QScrollBar *scrollbar);
  Qt::ScrollBarPolicy horizontalScrollBarPolicy() const;
  void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

  QScrollBar* verticalScrollBar() const;
  void setVerticalScrollBar(QScrollBar *scrollbar);
  Qt::ScrollBarPolicy verticalScrollBarPolicy() const;
  void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);

  int hscroll() const;
  const QRect & viewport() const;
  Position hitTest(const QPoint & pos) const;
  QPoint mapToViewport(const Position & pos) const;
  QPoint map(const Position & pos) const;
  bool isVisible(const Position &pos) const;

  void fold(int line);
  void unfold(int line);
  const TextFoldList& folds() const;
  bool hasActiveFolds() const;

  const TextFormat & defaultFormat() const;
  void setDefaultFormat(const TextFormat & format);

  SyntaxHighlighter* syntaxHighlighter() const;
  void setSyntaxHighlighter(SyntaxHighlighter *highlighter);
  
  template<typename T>
  void setSyntaxHighlighter()
  {
    setSyntaxHighlighter(new T);
  }

  /// rename to 'incrustWidget' ?
  void insertWidget(int line, int num, QWidget *w);
  const QMap<LineRange, QWidget*> & insertedWidgets() const;
  void insertFloatingWidget(QWidget *widget, const QPoint & pos);

  inline TextViewImpl* impl() const { return d.get(); }

protected Q_SLOTS:
  void onBlockDestroyed(int line, const TextBlock & block);
  void onBlockInserted(const Position & pos, const TextBlock & block);
  void onContentsChange(const TextBlock & block, const Position & pos, int charsRemoved, int charsAdded);

protected:
  explicit TextView(std::unique_ptr<TextViewImpl> && impl);

protected:
  void paintEvent(QPaintEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;
  void wheelEvent(QWheelEvent *e) override;

  void setupPainter(QPainter *painter);
  void paint(QPainter *painter);
  virtual void drawLine(QPainter *painter, const QPoint & offset, view::Line line);
  virtual void drawBlock(QPainter *painter, const QPoint & offset, view::Block block);
  void drawLineElements(QPainter *painter, const QPoint & offset, view::LineElements elements);
  virtual void drawFoldSymbol(QPainter *painter, const QPoint & offset, int foldid);
  void drawBlockFragment(QPainter *painter, QPoint & offset, int blocknum, int begin, int end);
  void drawFragment(QPainter *painter, QPoint & offset, view::Fragment fragment);
  void drawText(QPainter *painter, QPoint & offset, const QString & text, const TextFormat & format);
  void drawStrikeOut(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void drawUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void drawWaveUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void applyFormat(QPainter *painter, const TextFormat & fmt);
  QString replaceTabs(QString text) const;

  const view::Metrics & metrics() const;

protected:
  void updateLayout();

private: 
  void init();

protected:
  std::unique_ptr<TextViewImpl> d;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTVIEW_H
