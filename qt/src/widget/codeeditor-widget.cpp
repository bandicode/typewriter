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

#include <QScrollBar>

#include <algorithm>
#include <stdexcept>

namespace typewriter
{

QTypewriterGutter::QTypewriterGutter(QTypewriterView* context, QWidget* parent)
  : QWidget(parent)
  , d(context)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

QTypewriterGutter::~QTypewriterGutter()
{

}

QTypewriterView* QTypewriterGutter::view() const
{
  return d;
}

void QTypewriterGutter::setView(QTypewriterView* v)
{
  if (d != v)
  {
    d = v;
    update();
  }
}

void QTypewriterGutter::addMarker(int line, MarkerType m)
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

void QTypewriterGutter::clearMarkers()
{
  if (!m_markers.empty())
  {
    m_markers.clear();
    update();
  }
}

void QTypewriterGutter::removeMarkers(int line)
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

void QTypewriterGutter::removeMarker(int line, MarkerType m)
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

QSize QTypewriterGutter::sizeHint() const
{
  return QSize{ d->metrics().charwidth * columnCount() + 4, 66 };
}

void QTypewriterGutter::mousePressEvent(QMouseEvent* e)
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

void QTypewriterGutter::paintEvent(QPaintEvent* e)
{
  QPainter painter{ this };

  painter.setBrush(QBrush(d->defaultTextFormat().background_color));
  painter.setPen(Qt::NoPen);
  painter.drawRect(this->rect());

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

void QTypewriterGutter::drawMarkers(QPainter& painter, QPoint pt, int markers)
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

int QTypewriterGutter::columnCount() const
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

void QTypewriterGutter::writeNumber(QString& str, int n)
{
  for (int i(1); i <= str.size(); ++i)
  {
    str[str.size() - i] = (n == 0 ? ' ' : '0' + (n % 10));
    n /= 10;
  }
}

bool QTypewriterGutter::find_marker(int line, std::vector<Marker>::const_iterator& it) const
{
  if (it == m_markers.end())
    return false;

  while (it != m_markers.end() && it->line < line)
    ++it;

  return it != m_markers.end() && it->line == line;
}

QTypewriter::QTypewriter(QWidget* parent)
  : QWidget(parent),
    m_view(nullptr)
{
  m_view = new QTypewriterView(this);
  init();
}

QTypewriter::QTypewriter(QTypewriterView* view, QWidget* parent)
  : QWidget(parent),
    m_view(view)
{
  init();
}

QTypewriter::QTypewriter(QTypewriterDocument* document, QWidget* parent)
  : QWidget(parent),
    m_view(nullptr)
{
  m_view = new QTypewriterView(document, this);
  init();
}


QTypewriter::~QTypewriter()
{

}

void QTypewriter::init()
{
  connect(m_view, &QTypewriterView::invalidated, this, qOverload<>(&QTypewriter::update));
  connect(m_view, &QTypewriterView::lineCountChanged, this, &QTypewriter::updateScrollBarsValues);

  m_horizontal_scrollbar = new QScrollBar(Qt::Horizontal, this);
  connect(m_horizontal_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));

  m_vertical_scrollbar = new QScrollBar(Qt::Vertical, this);
  m_vertical_scrollbar->setRange(0, document()->lineCount() - 1);
  m_vertical_scrollbar->setValue(0);
  connect(m_vertical_scrollbar, &QScrollBar::valueChanged, this, &QTypewriter::setFirstVisibleLine);

  m_gutter = new QTypewriterGutter{ m_view, this };

  //d->syntaxHighlighter = new SyntaxHighlighter(this);

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(14);
    setFont(font);
  }
}

QTypewriterView* QTypewriter::view() const
{
  return m_view;
}

void QTypewriter::setView(QTypewriterView* v)
{
  if (m_view != v && v != nullptr)
  {
    if (m_view)
    {
      connect(m_view, nullptr, this, nullptr);

      if(m_view->parent() == this)
        m_view->deleteLater();
    }

    m_view = v;

    connect(m_view, &QTypewriterView::invalidated, this, qOverload<>(&QTypewriter::update));
    connect(m_view, &QTypewriterView::lineCountChanged, this, &QTypewriter::updateScrollBarsValues);

    if (m_gutter)
      m_gutter->setView(v);

    Q_EMIT viewChanged();

    update();
  }
}

QTypewriterDocument* QTypewriter::document() const
{
  return m_view->document();
}

QTypewriterGutter* QTypewriter::gutter() const
{
  return m_gutter;
}

void QTypewriter::scroll(int delta)
{
  if (delta == 0)
    return;

  m_view->setLineScroll(m_view->linescroll() + delta);

  m_vertical_scrollbar->setValue(m_view->linescroll());

  update();
}

void QTypewriter::setFirstVisibleLine(int n)
{
  m_view->setLineScroll(n);
}

void QTypewriter::setTabSize(int n)
{
  if (tabSize() == n)
    return;

  m_view->setTabSize(n);

  update();
}

int QTypewriter::tabSize() const
{
  return m_view->tabSize();
}

QScrollBar* QTypewriter::horizontalScrollBar() const
{
  return m_horizontal_scrollbar;
}

void QTypewriter::setHorizontalScrollBar(QScrollBar* scrollbar)
{
  int hscroll = m_horizontal_scrollbar->value();

  m_horizontal_scrollbar->deleteLater();
  m_horizontal_scrollbar = scrollbar;

  m_horizontal_scrollbar->setRange(0, m_view->effectiveWidth());
  m_horizontal_scrollbar->setValue(hscroll);

  connect(m_horizontal_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));

  updateLayout();
}

Qt::ScrollBarPolicy QTypewriter::horizontalScrollBarPolicy() const
{
  return m_hscrollbar_policy;
}

void QTypewriter::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
  if (m_hscrollbar_policy == policy)
    return;

  m_hscrollbar_policy = policy;
  updateLayout();
}


QScrollBar* QTypewriter::verticalScrollBar() const
{
  return m_vertical_scrollbar;
}

void QTypewriter::setVerticalScrollBar(QScrollBar* scrollbar)
{
  m_vertical_scrollbar->deleteLater();
  m_vertical_scrollbar = scrollbar;
  m_vertical_scrollbar->setRange(0, m_view->lineCount() - 1);
  m_vertical_scrollbar->setValue(m_view->linescroll());
  connect(m_vertical_scrollbar, &QScrollBar::valueChanged, this, &QTypewriter::setFirstVisibleLine);
  updateLayout();
}

Qt::ScrollBarPolicy QTypewriter::verticalScrollBarPolicy() const
{
  return m_vscrollbar_policy;
}

void QTypewriter::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
  if (m_vscrollbar_policy == policy)
    return;

  m_vscrollbar_policy = policy;
  updateLayout();
}

int QTypewriter::hscroll() const
{
  return m_horizontal_scrollbar->value();
}

const QRect& QTypewriter::viewport() const
{
  return m_viewport;
}

Position QTypewriter::hitTest(const QPoint& pos) const
{
  return m_view->hitTest(pos - viewport().topLeft());
}

QPoint QTypewriter::mapToViewport(const Position& pos) const
{
  return m_view->map(pos);
}

QPoint QTypewriter::map(const Position& pos) const
{
  return mapToViewport(pos) + viewport().topLeft();
}

bool QTypewriter::isVisible(const Position& pos) const
{
  return m_view->isVisible(pos);
}

//void QTypewriter::fold(int line)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//void QTypewriter::unfold(int line)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//const TextFoldList& QTypewriter::folds() const
//{
//  return d->folds;
//}
//
//bool QTypewriter::hasActiveFolds() const
//{
//  return d->folds.lastActiveIndex() != -1;
//}

const TextFormat& QTypewriter::defaultFormat() const
{
  return m_view->defaultTextFormat();
}

void QTypewriter::setDefaultFormat(const TextFormat& format)
{
  m_view->setDefaultTextFormat(format);
  update();
}

const TextFormat& QTypewriter::textFormat(int id) const
{
  return m_view->textFormat(id);
}

void QTypewriter::setTextFormat(int id, const TextFormat& fmt)
{
  m_view->setFormat(id, fmt);
  update();
}

//
//SyntaxHighlighter* QTypewriter::syntaxHighlighter() const
//{
//  return d->syntaxHighlighter;
//}
//
//void QTypewriter::setSyntaxHighlighter(SyntaxHighlighter* highlighter)
//{
//  d->syntaxHighlighter->deleteLater();
//  d->syntaxHighlighter = highlighter;
//  d->syntaxHighlighter->impl()->view = this;
//}

//void QTypewriter::insertWidget(int line, int num, QWidget* w)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//const QMap<LineRange, QWidget*>& QTypewriter::insertedWidgets() const
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}

void QTypewriter::insertFloatingWidget(QWidget* widget, const QPoint& pos)
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}

void QTypewriter::paintEvent(QPaintEvent* e)
{
  QPainter painter{ this };
  setupPainter(&painter);

  QTransform tr;
  tr.translate(m_viewport.topLeft().x(), m_viewport.topLeft().y());
  painter.setTransform(tr);

  QTypewriterPainterRenderer renderer{ *m_view, painter };
  typewriter::render(*m_view, renderer);
}

void QTypewriter::resizeEvent(QResizeEvent* e)
{
  updateLayout();
}

void QTypewriter::wheelEvent(QWheelEvent* e)
{
  scroll(-3 * e->delta() / (15 * 8));
}

void QTypewriter::onFontChange()
{
  update();
}

void QTypewriter::setupPainter(QPainter* painter)
{
  painter->setFont(font());
  painter->setClipRect(m_viewport);
}

const QTypewriterFontMetrics& QTypewriter::metrics() const
{
  return m_view->metrics();
}

void QTypewriter::updateScrollBarsValues()
{
  if (m_vertical_scrollbar)
  {
    m_vertical_scrollbar->setMaximum(m_view->maxLinescroll());
    m_vertical_scrollbar->setEnabled(true);
    m_vertical_scrollbar->setSingleStep(3);
  }
}

void QTypewriter::updateLayout()
{
  int available_width = this->width();
  int available_height = this->height();

  QRect rect_vscrollbar{ this->width(), 0, 0, 0 };
  bool vscrollbar_visible = true;
  if (m_vscrollbar_policy != Qt::ScrollBarAlwaysOff)
  {
    rect_vscrollbar = QRect(this->width() - m_vertical_scrollbar->sizeHint().width(), 0, m_vertical_scrollbar->sizeHint().width(), this->height());
    available_width -= rect_vscrollbar.width();
  }
  else
  {
    vscrollbar_visible = false;
  }

  QRect rect_gutter{ 0, 0, 0, 0 };
  if (m_gutter->isVisible())
  {
    rect_gutter = QRect(0, 0, m_gutter->sizeHint().width(), available_height);
    available_width -= m_gutter->sizeHint().width();
  }

  QRect rect_hscrollbar;
  bool hscrollbar_visible = true;
  if (m_hscrollbar_policy == Qt::ScrollBarAlwaysOn)
  {
    rect_hscrollbar = QRect(0, this->height() - m_horizontal_scrollbar->sizeHint().height(), available_width, m_horizontal_scrollbar->sizeHint().height());
    rect_vscrollbar.adjust(0, 0, 0, -rect_hscrollbar.height());
    rect_gutter.adjust(0, 0, 0, -rect_hscrollbar.height());
    available_height -= rect_hscrollbar.height();
  }
  else if (m_hscrollbar_policy == Qt::ScrollBarAsNeeded)
  {
    if (available_width > m_view->effectiveWidth())
    {
      hscrollbar_visible = false;
    }
    else
    {
      rect_hscrollbar = QRect(0, this->height() - m_horizontal_scrollbar->sizeHint().height(), available_width, m_horizontal_scrollbar->sizeHint().height());
      rect_vscrollbar.adjust(0, 0, 0, -rect_hscrollbar.height());
      rect_gutter.adjust(0, 0, 0, -rect_hscrollbar.height());
    }
  }
  else
  {
    hscrollbar_visible = false;
  }

  if (vscrollbar_visible)
  {
    m_vertical_scrollbar->setVisible(true);
    m_vertical_scrollbar->setGeometry(rect_vscrollbar);
  }
  else
  {
    m_vertical_scrollbar->setVisible(false);
  }

  if (m_gutter->isVisible())
  {
    m_gutter->setGeometry(rect_gutter);
  }

  if (hscrollbar_visible)
  {
    m_horizontal_scrollbar->setVisible(true);
    m_horizontal_scrollbar->setGeometry(rect_hscrollbar);
  }
  else
  {
    m_horizontal_scrollbar->setVisible(false);
  }

  QRect vp = m_viewport;
  m_viewport = QRect(rect_gutter.right() + 1, 0, rect_vscrollbar.left() - rect_gutter.right() - 1, available_height);

  if (vp != m_viewport)
  {
    m_view->resize(m_viewport.size());
  }
}

//const TextCursor & QTypewriter::cursor() const
//{
//  return d_func()->cursor;
//}

void QTypewriter::mousePressEvent(QMouseEvent *e)
{
  //d_func()->cursor.setPosition(hitTest(e->pos()));
  m_cursor_blink = true;
  /// TODO: scroll if cursor not fully visible
  update();
}

void QTypewriter::mouseMoveEvent(QMouseEvent *e)
{
  //auto pos = d_func()->cursor.position();
  //d_func()->cursor.setPosition(hitTest(e->pos()), TextCursor::KeepAnchor);
  //if (d_func()->cursor.position() != pos)
  //  update();
}

void QTypewriter::mouseReleaseEvent(QMouseEvent *e)
{
  /// TODO
}

void QTypewriter::mouseDoubleClickEvent(QMouseEvent *e)
{
  /// TODO
}

bool QTypewriter::event(QEvent* ev)
{
  if (ev->type() == QEvent::FontChange)
    onFontChange();

  return QWidget::event(ev);
}

void QTypewriter::keyPressEvent(QKeyEvent *e)
{
  const bool shift_modifier = e->modifiers() & Qt::ShiftModifier;
  const bool alt_modifier = e->modifiers() & Qt::AltModifier;

  //switch (e->key())
  //{
  //case Qt::Key_Down:
  //  d_func()->cursor.movePosition(TextCursor::Down, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
  //  break;
  //case Qt::Key_Left:
  //  d_func()->cursor.movePosition(TextCursor::Left, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
  //  break;
  //case Qt::Key_Right:
  //  d_func()->cursor.movePosition(TextCursor::Right, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
  //  break;
  //case Qt::Key_Up:
  //  d_func()->cursor.movePosition(TextCursor::Up, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
  //  break;
  //case Qt::Key_Enter:
  //case Qt::Key_Return:
  //  d_func()->cursor.insertBlock();
  //  break;
  //case Qt::Key_Delete:
  //  d_func()->cursor.deleteChar();
  //  break;
  //case Qt::Key_Backspace:
  //  d_func()->cursor.deletePreviousChar();
  //  break;
  //default:
  //{
  //  QString text = e->text();
  //  if (!text.isEmpty())
  //    d_func()->cursor.insertText(text);
  //}
  //break;
  //}

  m_cursor_blink = true;
  update();
}

void QTypewriter::keyReleaseEvent(QKeyEvent *e)
{
  /// TODO
}

void QTypewriter::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == m_timer_id)
  {
    m_cursor_blink = !m_cursor_blink;
    update();
  }

  QWidget::timerEvent(e);
}

void QTypewriter::showEvent(QShowEvent *e)
{
  if (m_timer_id == -1)
  {
    m_timer_id = startTimer(750);
  }

  updateLayout();

  QWidget::showEvent(e);
}

static TextBlock selection_start_block(const TextCursor & c)
{
  if (c.position() == c.selectionStart())
    return c.block();
  return prev(c.block(), c.selectionEnd().line - c.selectionStart().line);
}

void QTypewriter::drawCursor(QPainter *painter, const TextCursor & c)
{
  if (c.hasSelection())
    drawSelection(painter, selection_start_block(c), c.selectionStart(), c.selectionEnd());

  if (!m_cursor_blink)
    return;

  painter->setPen(QPen(Qt::black));

  QPoint pt = map(c.position());
  pt.ry() -= metrics().ascent;

  painter->drawLine(pt, pt + QPoint(0, metrics().lineheight));
}

void QTypewriter::drawSelection(QPainter *painter, TextBlock block, const Position & begin, const Position & end)
{
  // @TODO: to simplify selection drawing (if we want to change text color
  // and not just draw unicolor overlay):
  // redraw everything into a pixmap by modifying the text format,
  // the blit the selected sections over the 'un-selected' rendering.
  // really not performance friendly but much simpler & powerfull

  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(QColor(100, 100, 255, 100)));

  QPoint pt = map(begin);
  pt.ry() -= metrics().ascent;

  if (begin.line == end.line)
  {
    QPoint endpt = map(end);
    endpt.ry() -= metrics().ascent;

    const int colcount = end.column - begin.column;
    painter->drawRect(QRect(pt, QSize(endpt.x() - pt.x(), metrics().lineheight)));
  }
  else
  {
    /// TODO: take tabulations into account

    // Drawing first line selection
    int colcount = 1 + block.length() - begin.column;
    painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));

    // Drawing middle lines
    pt = map(Position{ begin.line, 0 });
    pt.ry() -= metrics().ascent;
    const int linecount = end.line - begin.line - 1;
    for (int i(0); i < linecount; ++i)
    {
      block = block.next();
      pt.ry() += metrics().lineheight;
      colcount = 1 + block.length();
      painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));
    }

    // Drawing last line
    block = block.next();
    pt.ry() += metrics().lineheight;
    colcount = end.column;
    painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));
  }
}

} // namespace typewriter
