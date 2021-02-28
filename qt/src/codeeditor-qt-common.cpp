// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/qt/codeeditor-widget.h"

#include "typewriter/textblock.h"
#include "typewriter/view/fragment.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <QPainter>
#include <QPainterPath>

#include <QScrollBar>

#include <algorithm>
#include <stdexcept>

namespace typewriter
{

QTypewriterFontMetrics::QTypewriterFontMetrics(const QFont& f)
{
  QFontMetrics fm{ f };
  this->charwidth = fm.averageCharWidth();
  this->lineheight = fm.height();
  this->ascent = fm.ascent();
  this->descent = fm.descent();
  this->strikeoutpos = fm.strikeOutPos();
  this->underlinepos = fm.underlinePos();
}

namespace details
{

QTypewriterContext::QTypewriterContext(typewriter::TextDocument* doc)
  : document(doc),
    view(doc)
{
  this->formats.resize(16);
}

QTypewriterVisibleLines QTypewriterContext::visibleLines() const
{
  size_t count = availableHeight() / this->metrics.lineheight;
  return QTypewriterVisibleLines(this->view.lines(), this->first_visible_line, count);
}

} // namespace details

QTypewriterRenderer::QTypewriterRenderer(details::QTypewriterContext& context, QPainter& p, QRect vp, int hscroll)
  : m_view(context.view),
    m_painter(p),
    m_default_format(context.default_format),
    m_formats(context.formats),
    m_metrics(context.metrics),
    m_viewport(vp),
    m_hscroll(hscroll)
{

}

void QTypewriterRenderer::paint(iterator begin, iterator end)
{
  painter().setBrush(QBrush(m_default_format.background_color));
  painter().setPen(Qt::NoPen);
  painter().drawRect(m_viewport);

  int i = 0;
  for (auto it = begin; it != end; ++it, ++i)
  {
    const int baseline = i * m_metrics.lineheight + m_metrics.ascent;
    drawLine(QPoint{ m_viewport.left() - m_hscroll, baseline }, *it);
  }
}

void QTypewriterRenderer::drawLine(const QPoint& offset, const view::Line& line)
{
  if (line.elements.empty() || line.elements.front().kind == view::LineElement::LE_Insert)
    return;

  QPoint pt = offset;

  for (const auto& e : line.elements)
  {
    if (e.kind == view::LineElement::LE_LineIndent || e.kind == view::LineElement::LE_CarriageReturn)
      continue;

    if (e.kind == view::LineElement::LE_Fold)
    {
      drawFoldSymbol(pt, e.id);
      pt.rx() += e.width * m_metrics.charwidth;
    }
    else if (e.kind == view::LineElement::LE_BlockFragment)
    {
      drawBlockFragment(pt, line, e);
      pt.rx() += e.width * m_metrics.charwidth;
    }
  }

}

void QTypewriterRenderer::drawFoldSymbol(const QPoint& offset, int foldid)
{
  /// TODO: 
  throw std::runtime_error{ "Not implemented" };
}

void QTypewriterRenderer::drawBlockFragment(QPoint offset, const view::Line& line, const view::LineElement& fragment)
{
  view::StyledFragments fragments = view().fragments(line, fragment);

  for (auto it = fragments.begin(); it != fragments.end(); it = it.next())
  {
    //QString text = QString::fromStdString(fragment.block.text().substr(fragment.begin, fragment.width));
    //drawText(painter, offset, text, m_context->default_format);
    QString text = QString::fromStdString(it.text());
    drawText(offset, text, textFormat(it.format()));
    offset.rx() += it.length() * metrics().charwidth;
  }
}

void QTypewriterRenderer::drawStrikeOut(const QPoint& offset, const TextFormat& fmt, int count)
{
  QPen pen{ fmt.strikeout_color };
  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  painter().setPen(pen);

  painter().drawLine(offset.x(), offset.y() - metrics().strikeoutpos, offset.x() + count * metrics().charwidth, offset.y() - metrics().strikeoutpos);
}

void QTypewriterRenderer::drawUnderline(const QPoint& offset, const TextFormat& fmt, int count)
{
  if (fmt.underline == TextFormat::NoUnderline)
  {
    return;
  }
  else if (fmt.underline == TextFormat::WaveUnderline)
  {
    drawWaveUnderline(offset, fmt, count);
  }
  else
  {
    QPen pen{ fmt.underline_color };

    const int penwidth = std::max(1, metrics().ascent / 10);
    pen.setWidth(penwidth);

    switch (fmt.underline)
    {
    case TextFormat::SingleUnderline:
      pen.setStyle(Qt::SolidLine);
      break;
    case TextFormat::DashUnderline:
      pen.setStyle(Qt::DashLine);
      break;
    case TextFormat::DotLine:
      pen.setStyle(Qt::DotLine);
      break;
    case TextFormat::DashDotLine:
      pen.setStyle(Qt::DashDotLine);
      break;
    case TextFormat::DashDotDotLine:
      pen.setStyle(Qt::DashDotDotLine);
      break;
    default:
      break;
    }

    painter().setPen(pen);
    //painter->drawLine(offset.x(), offset.y() + metrics().underlinepos, offset.x() + count * metrics().charwidth, offset.y() + metrics().underlinepos);
    painter().drawLine(offset.x(), offset.y() + metrics().descent - penwidth - 1, offset.x() + count * metrics().charwidth, offset.y() + metrics().descent - penwidth - 1);
  }
}

static QPixmap generate_wave_pattern(const QPen& pen, const QTypewriterFontMetrics& metrics)
{
  /// TODO: add cache

  const int pattern_height = metrics.descent * 2 / 3;
  const int pattern_width = metrics.charwidth * 13 / 16;

  QPixmap img{ pattern_width, pattern_height };
  img.fill(QColor{ 255, 255, 255, 0 });

  QPainterPath path;
  path.moveTo(0, img.height() - 1);
  path.lineTo(pattern_width / 2.f, 0.5);
  path.lineTo(pattern_width, img.height() - 1);

  QPainter painter{ &img };
  painter.setPen(pen);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.drawPath(path);

  return img;
}

static void fill_with_pattern(QPainter& painter, const QRect& rect, const QPixmap& pattern)
{
  const int y = rect.top();

  int x = rect.left();

  const int full = rect.width() / pattern.width();
  for (int i(0); i < full; ++i)
  {
    painter.drawPixmap(x, y, pattern);
    x += pattern.width();
  }

  const int partial_width = rect.width() - pattern.width() * full;

  if (partial_width > 0)
    painter.drawPixmap(QPoint(x, y), pattern, QRect(0, 0, partial_width, pattern.height()));
}

void QTypewriterRenderer::drawWaveUnderline(const QPoint& offset, const TextFormat& fmt, int count)
{
  /// TODO:
  QPen pen{ fmt.underline_color };

  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  pen.setJoinStyle(Qt::RoundJoin);

  QPixmap pattern = generate_wave_pattern(pen, metrics());
  QRect rect{ offset.x(), offset.y() + metrics().descent - pattern.height(), count * metrics().charwidth, pattern.height() };

  fill_with_pattern(painter(), rect, pattern);
}

void QTypewriterRenderer::drawText(const QPoint& offset, const QString& text, const TextFormat& format)
{
  applyFormat(painter(), format);

  painter().drawText(offset, text);

  if (format.strikeout)
    drawStrikeOut(offset, format, text.length());

  drawUnderline(offset, format, text.length());
}

void QTypewriterRenderer::applyFormat(QPainter& painter, const TextFormat& fmt)
{
  QFont f = painter.font();
  f.setBold(fmt.bold);
  f.setItalic(fmt.italic);
  painter.setFont(f);
  painter.setPen(QPen(fmt.text_color));
  painter.setBrush(QBrush(fmt.background_color));
}

} // namespace typewriter
