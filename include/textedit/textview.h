// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTVIEW_H
#define TEXTEDIT_TEXTVIEW_H

#include "textedit/textdocument.h"
#include "textedit/view/extraselection.h"

#include <QWidget>

#include <QMap>
#include <QScrollBar>

namespace textedit
{

namespace view
{
class Fragment;
class Line;
struct Metrics;
} // namespace view

class SyntaxHighlighter;
class TextViewImpl;

class TEXTEDIT_API TextView : public QWidget
{
  Q_OBJECT
public:
  TextView(const TextDocument *document);
  ~TextView();

  const TextDocument * document() const;

  int firstVisibleLine() const;
  TextBlock firstVisibleBlock() const;
  void scroll(int delta);
  int visibleLineCount() const;
  int lastVisibleLine() const;
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

  const TextFormat & defaultFormat() const;
  void setDefaultFormat(const TextFormat & format);

  SyntaxHighlighter* syntaxHighlighter() const;
  void setSyntaxHighlighter(SyntaxHighlighter *highlighter);
  
  template<typename T>
  void setSyntaxHighlighter()
  {
    setSyntaxHighlighter(new T);
  }

  typedef view::ExtraSelection ExtraSelection;
  const QList<ExtraSelection> & extraSelections() const;
  void setExtraSelections(const QList<ExtraSelection> & selections);

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
  void paintEvent(QPaintEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;
  void wheelEvent(QWheelEvent *e) override;

  void setupPainter(QPainter *painter);
  void paint(QPainter *painter);
  virtual void drawLine(QPainter *painter, const QPoint & offset, view::Line line);
  void drawFragment(QPainter *painter, QPoint & offset, view::Fragment fragment);
  void drawStrikeOut(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void drawUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void drawWaveUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count);
  void applyFormat(QPainter *painter, const TextFormat & fmt);

  const view::Metrics & metrics() const;

protected:
  void updateLayout();

private:
  std::unique_ptr<TextViewImpl> d;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTVIEW_H
