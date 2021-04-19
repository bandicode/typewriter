// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_H
#define TYPEWRITER_QTYPEWRITER_H

#include "typewriter/qt/codeeditor-qt-common.h"

#include "typewriter/contributor.h"
#include "typewriter/textview.h"

#include <QWidget>

#include <QColor>

class QScrollBar;

namespace typewriter
{
class QTypewriter;

class TYPEWRITER_QAPI QTypewriterGutter : public QWidget
{
  Q_OBJECT
public:
  QTypewriterGutter(QTypewriterView* context, QWidget* parent);
  ~QTypewriterGutter();

  QTypewriterView* view() const;
  void setView(QTypewriterView* v);

  void addMarker(int line, MarkerType m);
  void clearMarkers();
  void removeMarkers(int line);
  void removeMarker(int line, MarkerType m);

  virtual QSize sizeHint() const override;

Q_SIGNALS:
  void clicked(int line);

protected:
  int columnCount() const;

protected:
  void mousePressEvent(QMouseEvent* e) override;
  void paintEvent(QPaintEvent* e) override;

protected:
  void drawMarkers(QPainter& painter, QPoint pt, int markers);
  
  void writeNumber(QString& str, int n);
  bool find_marker(int line, std::vector<Marker>::const_iterator& it) const;

private:
  QTypewriterView* d;
  std::vector<Marker> m_markers;
};

class TYPEWRITER_QAPI QTypewriter : public QWidget
{
  Q_OBJECT
public:
  explicit QTypewriter(QWidget* parent = nullptr);
  explicit QTypewriter(QTypewriterView* view, QWidget* parent = nullptr);
  explicit QTypewriter(QTypewriterDocument* doc, QWidget* parent = nullptr);
  ~QTypewriter();

  QTypewriterView* view() const;
  void setView(QTypewriterView* v);

  QTypewriterDocument* document() const;

  QTypewriterGutter* gutter() const;

  void scroll(int delta);

public Q_SLOTS:
  void setFirstVisibleLine(int n);

public:

  void setTabSize(int n);
  int tabSize() const;

  QScrollBar* horizontalScrollBar() const;
  void setHorizontalScrollBar(QScrollBar* scrollbar);
  Qt::ScrollBarPolicy horizontalScrollBarPolicy() const;
  void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

  QScrollBar* verticalScrollBar() const;
  void setVerticalScrollBar(QScrollBar* scrollbar);
  Qt::ScrollBarPolicy verticalScrollBarPolicy() const;
  void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);

  int hscroll() const;
  const QRect& viewport() const;
  Position hitTest(const QPoint& pos) const;
  QPoint mapToViewport(const Position& pos) const;
  QPoint map(const Position& pos) const;
  bool isVisible(const Position& pos) const;

  //void fold(int line);
  //void unfold(int line);
  //const TextFoldList& folds() const;
  //bool hasActiveFolds() const;

  const TextFormat& defaultFormat() const;
  void setDefaultFormat(const TextFormat& format);

  const TextFormat& textFormat(int id) const;
  void setTextFormat(int id, const TextFormat& fmt);

  //SyntaxHighlighter* syntaxHighlighter() const;
  //void setSyntaxHighlighter(SyntaxHighlighter* highlighter);

  //template<typename T>
  //void setSyntaxHighlighter()
  //{
  //  setSyntaxHighlighter(new T);
  //}

  /// rename to 'incrustWidget' ?
  void insertWidget(int line, int num, QWidget* w);
  //const QMap<LineRange, QWidget*>& insertedWidgets() const;
  void insertFloatingWidget(QWidget* widget, const QPoint& pos);

Q_SIGNALS:
  void viewChanged();

protected:
  bool event(QEvent* ev) override;
  void paintEvent(QPaintEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void wheelEvent(QWheelEvent* e) override;

  virtual void onFontChange();

  void setupPainter(QPainter* painter);

  const QTypewriterFontMetrics& metrics() const;

protected:
  void updateScrollBarsValues();
  void updateLayout();

private:
  void init();

public:

  //const TextCursor & cursor() const;

protected:
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseDoubleClickEvent(QMouseEvent *e) override;

  void keyPressEvent(QKeyEvent *e) override;
  void keyReleaseEvent(QKeyEvent *e) override;

  void timerEvent(QTimerEvent *e) override;

  void showEvent(QShowEvent *e) override;

protected:
  void drawCursor(QPainter *painter, const TextCursor & c);
  void drawSelection(QPainter *painter, TextBlock block, const Position & begin, const Position & end);

protected:
  QTypewriterView* m_view;
  QTypewriterGutter* m_gutter;
  Qt::ScrollBarPolicy m_hscrollbar_policy;
  QScrollBar* m_horizontal_scrollbar;
  Qt::ScrollBarPolicy m_vscrollbar_policy;
  QScrollBar* m_vertical_scrollbar;
  QRect m_viewport;
  int m_timer_id = -1;
  bool m_cursor_blink = true;
};

} // namespace typewriter

#endif // !TYPEWRITER_QTYPEWRITER_H
