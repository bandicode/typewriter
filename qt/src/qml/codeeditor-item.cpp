// Copyright (C) 2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/qt/codeeditor-item.h"

#include "typewriter/textblock.h"
#include "typewriter/view/fragment.h"

#include <QKeyEvent>
#include <QMouseEvent>

#include <QPainter>


#include <algorithm>
#include <stdexcept>

namespace typewriter
{

QTypewriterGutterItem::QTypewriterGutterItem(QQuickItem* parent)
  : QTypewriterGutterItem(new QTypewriterView(this), parent)
{

}

QTypewriterGutterItem::QTypewriterGutterItem(QTypewriterView* context, QQuickItem* parent)
  : QQuickPaintedItem(parent)
{
  setView(context);
}

QTypewriterGutterItem::~QTypewriterGutterItem()
{

}

QTypewriterView* QTypewriterGutterItem::view() const
{
  return d;
}

void QTypewriterGutterItem::setView(QObject* v)
{
  setView(qobject_cast<QTypewriterView*>(v));
}

void QTypewriterGutterItem::setView(QTypewriterView* v)
{
  if (!v)
    return;

  if (d && d->parent() == this)
    d->deleteLater();

  d = v;
  connect(v, &QTypewriterView::lineCountChanged, this, &QTypewriterGutterItem::minimumWidthChanged);

  Q_EMIT viewChanged();
}

int QTypewriterGutterItem::minimumWidth() const
{
  return d->metrics().charwidth * columnCount() + 4;
}

void QTypewriterGutterItem::addMarker(int line, MarkerType m)
{
  auto it = std::find_if(m_markers.begin(), m_markers.end(), [line](const Marker& m) {
    return m.line >= line;
    });

  if (it == m_markers.end() || it->line > line)
  {
    Marker marker;
    marker.line = line;
    marker.markers = static_cast<int>(m);
    m_markers.insert(it, marker);
    update();
  }
  else
  {
    it->markers |= static_cast<int>(m);
    update();
  }
}

void QTypewriterGutterItem::clearMarkers()
{
  if (!m_markers.empty())
  {
    m_markers.clear();
    update();
  }
}

void QTypewriterGutterItem::removeMarkers(int line)
{
  auto it = std::find_if(m_markers.begin(), m_markers.end(), [line](const Marker& m) {
    return m.line == line;
    });

  if (it != m_markers.end())
  {
    m_markers.erase(it);
    update();
  }
}

void QTypewriterGutterItem::removeMarker(int line, MarkerType m)
{
  auto it = std::find_if(m_markers.begin(), m_markers.end(), [line](const Marker& m) {
    return m.line == line;
    });

  if (it != m_markers.end())
  {
    int markers = it->markers;
    it->markers = it->markers & ~static_cast<int>(m);

    if (it->markers != markers)
      update();
  }
}

void QTypewriterGutterItem::mousePressEvent(QMouseEvent* e)
{
  if (e->pos().x() <= d->metrics().charwidth)
  {
    int line = e->pos().y() / d->metrics().lineheight;

    auto it = std::next(d->view().lines().begin(), d->linescroll());

    const int count = 1 + d->size().height() / d->metrics().lineheight;

    for (int i(0); i < count && it != d->view().lines().end(); ++i, ++it, --line)
    {
      if (it->elements.empty() || !it->elements.front().block.isValid())
        continue;

      if (line == 0)
      {
        int blocknum = it->elements.front().block.blockNumber();
        Q_EMIT clicked(blocknum);
      }
    }
  }
}

void QTypewriterGutterItem::paint(QPainter* painter_ptr)
{
  QPainter& painter = *painter_ptr;
  painter.setBrush(QBrush(d->defaultTextFormat().background_color));
  painter.setPen(Qt::NoPen);
  painter.drawRect(QRect(QPoint(0, 0), size().toSize()));

  painter.setFont(d->font());
  painter.setPen(QPen(d->defaultTextFormat().text_color));

  painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());

  auto it = std::next(d->view().lines().begin(), d->linescroll());
  std::vector<Marker>::const_iterator marker_it = m_markers.cbegin();

  const int count = 1 + d->size().height() / d->metrics().lineheight;

  QString label = QString(" ").repeated(columnCount());

  for (int i(0); i < count && it != d->view().lines().end(); ++i, ++it)
  {
    if (it->elements.empty() || !it->elements.front().block.isValid())
      continue;

    // @TODO: improve performance, avoid recomputing the block number every time
    int blocknum = it->elements.front().block.blockNumber();
    writeNumber(label, blocknum + 1);

    if (find_marker(blocknum, marker_it))
      drawMarkers(painter, QPoint(0, i * d->metrics().lineheight), marker_it->markers);

    painter.drawText(QPoint(0, i * d->metrics().lineheight + d->metrics().ascent), label);
  }

  // @TODO: draw cursors
}

void QTypewriterGutterItem::drawMarkers(QPainter& painter, QPoint pt, int markers)
{
  if (markers & static_cast<int>(MarkerType::Breakpoint))
  {
    painter.setBrush(QBrush(Qt::red));
    painter.drawEllipse(QRect(pt, QSize(d->metrics().charwidth, d->metrics().lineheight)));
  }

  if (markers & static_cast<int>(MarkerType::Breakposition))
  {
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawEllipse(QRect(pt, QSize(d->metrics().charwidth, d->metrics().lineheight)));
  }
}

int QTypewriterGutterItem::columnCount() const
{
  int n = d->document()->lineCount() + 1;
  int result = 1;
  while (n >= 10)
  {
    n /= 10;
    result += 1;
  }
  return result + 1;
}

void QTypewriterGutterItem::writeNumber(QString& str, int n)
{
  for (int i(1); i <= str.size(); ++i)
  {
    str[str.size() - i] = (n == 0 ? ' ' : '0' + (n % 10));
    n /= 10;
  }
}

bool QTypewriterGutterItem::find_marker(int line, std::vector<Marker>::const_iterator& it) const
{
  if (it == m_markers.end())
    return false;

  while (it != m_markers.end() && it->line < line)
    ++it;

  return it != m_markers.end() && it->line == line;
}

QTypewriterItem::QTypewriterItem(QQuickItem* parent)
  : QQuickPaintedItem(parent),
    m_view(new QTypewriterView(this))
{
  init();
}

QTypewriterItem::~QTypewriterItem()
{

}

QTypewriterView* QTypewriterItem::view() const
{
  return m_view;
}

void QTypewriterItem::setView(QObject* v)
{
  setView(qobject_cast<QTypewriterView*>(v));
}

void QTypewriterItem::setView(QTypewriterView* v)
{
  if (!v)
    return;

  if (m_view && m_view->parent() == this)
    m_view->deleteLater();

  m_view = v;

  connect(m_view, &QTypewriterView::invalidated, this, &QTypewriterItem::requestUpdate);

  requestUpdate();

  Q_EMIT viewChanged();
}

void QTypewriterItem::init()
{

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(14);
    m_view->setFont(font);
  }
}

void QTypewriterItem::requestUpdate()
{
  update();
}

TextDocument* QTypewriterItem::document() const
{
  return m_view->document()->document();
}

void QTypewriterItem::scroll(int delta)
{
  if (delta == 0)
    return;

  m_view->setLineScroll(m_view->linescroll() + delta);

  update();
}

void QTypewriterItem::setFirstVisibleLine(int n)
{
  m_view->setLineScroll(n);
}

Position QTypewriterItem::hitTest(const QPoint& pos) const
{
  return m_view->hitTest(pos);
}

QPoint QTypewriterItem::map(const Position& pos) const
{
  return m_view->map(pos);
}

bool QTypewriterItem::isVisible(const Position& pos) const
{
  return m_view->isVisible(pos);
}

const TextFormat& QTypewriterItem::defaultFormat() const
{
  return m_view->defaultTextFormat();
}

void QTypewriterItem::setDefaultFormat(const TextFormat& format)
{
  m_view->setDefaultTextFormat(format);
  update();
}

const TextFormat& QTypewriterItem::textFormat(int id) const
{
  return m_view->textFormat(id);
}

void QTypewriterItem::setTextFormat(int id, const TextFormat& fmt)
{
  m_view->setFormat(id, fmt);
  update();
}

void QTypewriterItem::paint(QPainter* painter)
{
  setupPainter(painter);
  m_view->paint(painter);
}

void QTypewriterItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
  m_view->resize(size().toSize());
  QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QTypewriterItem::wheelEvent(QWheelEvent* e)
{
  scroll(-3 * e->delta() / (15 * 8));
}

void QTypewriterItem::setupPainter(QPainter* painter)
{
  painter->setFont(m_view->font());
}

const QTypewriterFontMetrics& QTypewriterItem::metrics() const
{
  return m_view->metrics();
}

} // namespace typewriter
