// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_COMMON_H
#define TYPEWRITER_QTYPEWRITER_COMMON_H

#include "typewriter/contributor.h"
#include "typewriter/textview.h"

#include <QWidget>

#include <QColor>
#include <QPen>

#include <vector>

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
  typewriter::TextDocument* document = nullptr;
  typewriter::TextView view;
  int first_visible_line = 0;
  typewriter::Contributor default_contributor;
  typewriter::Contributor* contributor = nullptr;
  QTypewriterFontMetrics metrics;
  TextFormat default_format;
  std::vector<TextFormat> formats;

public:
  explicit QTypewriterContext(typewriter::TextDocument* doc);

  QTypewriterVisibleLines visibleLines() const;
  
protected:
  virtual int availableHeight() const = 0;
};

} // namespace details

struct Marker
{
  int line = -1;
  int markers = 0;
};


// @TODO: remove me
enum class MarkerType
{
  Breakpoint = 1,
  Breakposition = 2,
};

class QTypewriterRenderer
{
protected:
  typewriter::TextView& m_view;
  QPainter& m_painter;
  TextFormat m_default_format;
  const std::vector<TextFormat>& m_formats;
  QTypewriterFontMetrics m_metrics;
  QRect m_viewport;
  int m_hscroll;

public:

  typedef std::list<typewriter::view::Line> list;
  typedef std::list<typewriter::view::Line>::const_iterator iterator;

  explicit QTypewriterRenderer(details::QTypewriterContext& context, QPainter& p, QRect vp, int hscroll);

  typewriter::TextView& view();
  const QTypewriterFontMetrics& metrics() const;
  QPainter& painter();
  const TextFormat& textFormat(int id) const;

  void paint(iterator begin, iterator end);
  virtual void drawLine(const QPoint& offset, const view::Line& line);
  virtual void drawFoldSymbol(const QPoint& offset, int foldid);
  void drawBlockFragment(QPoint offset, const view::Line& line, const view::LineElement& fragment);
  void drawText(const QPoint& offset, const QString& text, const TextFormat& format);
  void drawStrikeOut(const QPoint& offset, const TextFormat& fmt, int count);
  void drawUnderline(const QPoint& offset, const TextFormat& fmt, int count);
  void drawWaveUnderline(const QPoint& offset, const TextFormat& fmt, int count);
  static void applyFormat(QPainter& painter, const TextFormat& fmt);
};

inline typewriter::TextView& QTypewriterRenderer::view()
{
  return m_view;
}

inline const QTypewriterFontMetrics& QTypewriterRenderer::metrics() const
{
  return m_metrics;
}

inline QPainter& QTypewriterRenderer::painter()
{
  return m_painter;
}

inline const TextFormat& QTypewriterRenderer::textFormat(int id) const
{
  return m_formats.at(static_cast<size_t>(id));
}

} // namespace typewriter

#endif // !TYPEWRITER_QTYPEWRITER_COMMON_H
