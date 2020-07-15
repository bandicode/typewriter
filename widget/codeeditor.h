// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_H
#define TYPEWRITER_QTYPEWRITER_H

#include "typewriter/contributor.h"
#include "typewriter/textview.h"

#include <QWidget>

#include <QColor>
#include <QPen>

class QScrollBar;

namespace typewriter
{

struct QTypewriterFontMetrics
{
  int charwidth;
  int lineheight;
  int ascent;
  int descent;
  int underlinepos;
  int strikeoutpos;

  QTypewriterFontMetrics() = default;
  QTypewriterFontMetrics(const QTypewriterFontMetrics&) = default;

  explicit QTypewriterFontMetrics(const QFont& f);

  QTypewriterFontMetrics& operator=(const QTypewriterFontMetrics&) = default;
};

struct TextFormat
{

  enum UnderlineStyle {
    NoUnderline = 0,
    SingleUnderline = 1,
    DashUnderline = 2,
    DotLine = 3,
    DashDotLine = 4,
    DashDotDotLine = 5,
    WaveUnderline = 6,
  };

  bool bold = false;
  bool italic = false;
  bool strikeout = false;
  QColor strikeout_color = QColor();
  QColor text_color = QColor(0, 0, 0);
  QColor background_color = QColor(255, 255, 255);
  QColor foreground_color = QColor(0, 0, 0, 0);
  QPen border_pen;
  UnderlineStyle underline = NoUnderline;
  QColor underline_color = QColor();
};

class QTypewriter;

namespace details
{

class QTypewriterVisibleLines
{
public:

  typedef std::list<typewriter::view::Line> list;
  typedef std::list<typewriter::view::Line>::const_iterator iterator;

private:
  iterator m_begin;
  iterator m_end;
  size_t m_size;

public:

  QTypewriterVisibleLines(const list& lines, size_t b, size_t s)
  {
    m_begin = std::next(lines.begin(), b);
    m_size = std::min({ lines.size() - b, s });
    m_end = std::next(m_begin, m_size);
  }

  iterator begin() const { return m_begin; }
  iterator end() const { return m_end; }

  size_t size() const { return m_size; }
};

class QTypewriterContext : public typewriter::TextDocumentListener
{
public:
  QTypewriter* widget = nullptr;
  typewriter::TextDocument* document = nullptr;
  typewriter::TextView view;
  int first_visible_line = 0;
  typewriter::Contributor default_contributor;
  typewriter::Contributor* contributor = nullptr;
  QTypewriterFontMetrics metrics;
  TextFormat default_format;

public:
  explicit QTypewriterContext(QTypewriter* w, typewriter::TextDocument* doc);

  QTypewriterVisibleLines visibleLines() const;

  void blockDestroyed(int line, const TextBlock& block) override;
  void blockInserted(const Position& pos, const TextBlock& block) override;
  void contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded) override;
};

} // namespace details

class QTypewriterGutter : public QWidget
{
public:
  QTypewriterGutter(std::shared_ptr<details::QTypewriterContext> context, QWidget* parent);
  ~QTypewriterGutter();

  virtual QSize sizeHint() const override;

protected:
  int columnCount() const;

protected:
  void paintEvent(QPaintEvent* e);

  void writeNumber(QString& str, int n);

private:
  std::shared_ptr<details::QTypewriterContext> d;
};

class TYPEWRITER_API QTypewriter : public QWidget
{
  Q_OBJECT
public:
  explicit QTypewriter(QWidget* parent = nullptr);
  explicit QTypewriter(TextDocument *doc, QWidget* parent = nullptr);
  ~QTypewriter();

  TextDocument* document() const;

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

  //SyntaxHighlighter* syntaxHighlighter() const;
  //void setSyntaxHighlighter(SyntaxHighlighter* highlighter);

  //template<typename T>
  //void setSyntaxHighlighter()
  //{
  //  setSyntaxHighlighter(new T);
  //}

  /// rename to 'incrustWidget' ?
  void insertWidget(int line, int num, QWidget* w);
  const QMap<LineRange, QWidget*>& insertedWidgets() const;
  void insertFloatingWidget(QWidget* widget, const QPoint& pos);

protected:
  friend class details::QTypewriterContext;
  void onBlockDestroyed(int line, const TextBlock& block);
  void onBlockInserted(const Position& pos, const TextBlock& block);
  void onContentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded);

protected:
  bool event(QEvent* ev) override;
  void paintEvent(QPaintEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void wheelEvent(QWheelEvent* e) override;

  virtual void onFontChange();

  void setupPainter(QPainter* painter);
  void paint(QPainter* painter);
  virtual void drawLine(QPainter* painter, const QPoint& offset, const view::Line& line);
  virtual void drawFoldSymbol(QPainter* painter, const QPoint& offset, int foldid);
  void drawBlockFragment(QPainter* painter, const QPoint& offset, const view::LineElement& fragment);
  //void drawFragment(QPainter* painter, QPoint& offset, view::Fragment fragment);
  void drawText(QPainter* painter, const QPoint& offset, const QString& text, const TextFormat& format);
  void drawStrikeOut(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count);
  void drawUnderline(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count);
  void drawWaveUnderline(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count);
  void applyFormat(QPainter* painter, const TextFormat& fmt);

  const QTypewriterFontMetrics& metrics() const;

protected:
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
  std::shared_ptr<details::QTypewriterContext> m_context;
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
