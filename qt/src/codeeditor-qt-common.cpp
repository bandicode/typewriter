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

#include <QFile>

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

class DocumentDocListener : public typewriter::TextDocumentListener
{
public:
  QTypewriterDocument& backref;

public:
  DocumentDocListener(QTypewriterDocument& doc)
    : backref(doc)
  {

  }

  void blockDestroyed(int line, const TextBlock& block)
  {
    backref.notifyBlockDestroyed(line);
  }

  void blockInserted(const Position& pos, const TextBlock& block)
  {
    backref.notifyBlockInserted(pos, block);
  }

  void contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
  {
    backref.notifyContentsChange(block, pos, charsRemoved, charsAdded);
  }
};

QTypewriterDocument::QTypewriterDocument(QObject* parent)
  : QObject(parent),
    m_listener(new DocumentDocListener(*this))
{

}

QTypewriterDocument::QTypewriterDocument(typewriter::TextDocument* document, QObject* parent)
  : QObject(parent),
    m_document(document),
    m_listener(new DocumentDocListener(*this))
{
  if (m_document)
    m_document->addListener(m_listener.get());
}

QTypewriterDocument::QTypewriterDocument(std::unique_ptr<typewriter::TextDocument> document, QObject* parent)
  : QObject(parent),
    m_document_ptr(std::move(document)),
    m_document(m_document_ptr.get()),
    m_listener(new DocumentDocListener(*this))
{
  if (m_document)
    m_document->addListener(m_listener.get());
}

QTypewriterDocument::~QTypewriterDocument()
{
  if (m_document)
    m_document->removeListener(m_listener.get());
}

typewriter::TextDocument* QTypewriterDocument::document()
{
  return m_document;
}

void QTypewriterDocument::setDocument(typewriter::TextDocument* doc)
{
  if (doc == document())
    return;

  m_document->removeListener(m_listener.get());

  m_document = doc;
  m_document_ptr.reset();

  m_document->addListener(m_listener.get());
}

void QTypewriterDocument::setDocument(std::unique_ptr<typewriter::TextDocument> doc)
{
  if (doc.get() == document())
    return;

  m_document->removeListener(m_listener.get());

  m_document = doc.get();
  m_document_ptr.reset(doc.release());

  m_document->addListener(m_listener.get());
}

QString QTypewriterDocument::filepath() const
{
  return m_filepath;
}

void QTypewriterDocument::setFilePath(const QString& filepath)
{
  if (m_filepath != filepath)
  {
    m_filepath = filepath;

    QFile file{ filepath };

    if (file.open(QIODevice::ReadOnly))
    {
      QByteArray data = file.readAll();

      typewriter::TextCursor cursor{ document() };
      cursor.setPosition({ lineCount() + 1, 0 }, typewriter::TextCursor::KeepAnchor);
      cursor.removeSelectedText();
      cursor.insertText(data.toStdString());
    }

    Q_EMIT filepathChanged();
  }
}

int QTypewriterDocument::lineCount() const
{
  return m_document->lineCount();
}

void QTypewriterDocument::notifyBlockDestroyed(int line)
{
  Q_EMIT blockDestroyed();
  Q_EMIT lineCountChanged();
}

void QTypewriterDocument::notifyBlockInserted(const Position& pos, const TextBlock& block)
{
  Q_EMIT blockInserted(pos.line);
  Q_EMIT lineCountChanged();
}

void QTypewriterDocument::notifyContentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{

}

QTypewriterView::QTypewriterView(QObject* parent)
  : QObject(parent),
    m_document(new QTypewriterDocument(this)),
    m_view(m_document->document()),
    m_size(400, 600)
{
  m_formats.resize(16);

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(14);
    setFont(font);
  }
}

QTypewriterView::QTypewriterView(QTypewriterDocument* document, QObject* parent)
  : QObject(parent),
    m_document(document),
    m_view(m_document->document()),
    m_size(400, 600)
{
  m_formats.resize(16);

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(14);
    setFont(font);
  }
}

QTypewriterView::~QTypewriterView()
{

}

QTypewriterDocument* QTypewriterView::document()
{
  return m_document;
}

void QTypewriterView::setDocument(QObject* doc)
{
  setDocument(qobject_cast<QTypewriterDocument*>(doc));
}

void QTypewriterView::setDocument(QTypewriterDocument* doc)
{
  if (doc == document() || doc == nullptr)
    return;

  if (m_document && m_document->parent() == this)
  {
    m_document->document()->removeListener(this);

    if(m_document->parent() == this)
      m_document->deleteLater();
  }

  m_document = doc;
  m_view.reset(m_document->document());

  m_document->document()->addListener(this);

  Q_EMIT documentChanged();
}

typewriter::TextView& QTypewriterView::view()
{
  return m_view;
}

const typewriter::TextView& QTypewriterView::view() const
{
  return m_view;
}

int QTypewriterView::lineCount() const
{
  return static_cast<int>(m_view.lines().size());
}

int QTypewriterView::columnCount() const
{
  return m_view.width();
}

int QTypewriterView::effectiveWidth()
{
  return view().width() * metrics().charwidth;
}

int QTypewriterView::tabSize() const
{
  return m_view.tabSize();
}

void QTypewriterView::setTabSize(int colcount)
{
  if (colcount != view().tabSize())
  {
    view().setTabSize(colcount);
    Q_EMIT tabSizeChanged();
    Q_EMIT invalidated();
  }
}

QSize QTypewriterView::size() const
{
  return m_size;
}

void QTypewriterView::resize(const QSize& s)
{
  if (m_size != s)
  {
    m_size = s;
    Q_EMIT sizeChanged();
    Q_EMIT invalidated();
  }
}

int QTypewriterView::hscroll() const
{
  return m_hscroll;
}

void QTypewriterView::setHScroll(int hscroll)
{
  if (m_hscroll != hscroll)
  {
    m_hscroll = hscroll;
    Q_EMIT hscrollChanged();
    Q_EMIT invalidated();
  }
}

int QTypewriterView::linescroll() const
{
  return m_linescroll;
}

void QTypewriterView::setLineScroll(int linescroll)
{
  linescroll = std::max(0, std::min(linescroll, lineCount()));

  if (m_linescroll != linescroll)
  {
    m_linescroll = linescroll;
    Q_EMIT linescrollChanged();
    Q_EMIT invalidated();
  }
}

const QFont& QTypewriterView::font() const
{
  return m_font;
}

void QTypewriterView::setFont(const QFont& font)
{
  if (m_font != font)
  {
    m_font = font;
    m_metrics = QTypewriterFontMetrics(m_font);
    Q_EMIT fontChanged();
    Q_EMIT invalidated();
  }
}

const QTypewriterFontMetrics& QTypewriterView::metrics() const
{
  return m_metrics;
}

const TextFormat& QTypewriterView::defaultTextFormat() const
{
  return m_default_format;
}

void QTypewriterView::setDefaultTextFormat(TextFormat fmt)
{
  m_default_format = fmt;
  m_formats[0] = fmt;
}

const TextFormat& QTypewriterView::textFormat(int id) const
{
  return m_formats.at(static_cast<size_t>(id));
}

void QTypewriterView::setFormat(int id, TextFormat fmt)
{
  m_formats[id] = fmt;
}

void QTypewriterView::paint(QPainter* painter)
{
  m_painter = painter;

  painter->setFont(font());

  auto begin = std::next(view().lines().begin(), linescroll());

  auto it = begin;
  int numline = 1 + size().height() / metrics().lineheight;

  while (numline > 0 && it != view().lines().end())
  {
    ++it;
    --numline;
  }

  paint(begin, it);

  m_painter = nullptr;
}

Position QTypewriterView::hitTest(const QPoint& pos) const
{
  const int line_offset = pos.y() / m_metrics.lineheight;
  const int column_offset = std::round((pos.x() + hscroll()) / float(m_metrics.charwidth));

  auto visible_lines = visibleLines();

  /* Seek visible line */
  auto it = std::next(visible_lines.begin(), line_offset);

  /* Take into account tabulations & folds */
  if (it->elements.size() > 1)
  {
    int target_block = 0;
    int target_col = 0;
    int counter = column_offset;

    for (auto elem = it->elements.begin(); elem != it->elements.end(); ++elem)
    {
      if (elem->kind == view::LineElement::LE_Fold)
      {
        counter -= elem->width;
        if (counter <= 0)
          return Position{ target_block, target_col };
      }
      else
      {
        target_block = elem->block.blockNumber();
        target_col = elem->begin;
        const int count = elem->width;
        for (int i(0); i < count; ++i)
        {
          // @TODO: handle tab
          //if (elem.text().at(i) == QChar('\t'))
          //{
          //  counter -= tabSize();
          //  target_col += counter <= -tabSize() / 2 ? 0 : 1;
          //}
          //else
          //{
          counter -= 1;
          target_col += 1;
          //}

          if (counter <= 0)
            return Position{ target_block, target_col };
        }
      }
    }

    return Position{ target_block, target_col };
  }
  else
  {
    const std::string& text = it->block().text();
    int col = 0;
    for (int i(0), counter(column_offset); i < text.length() && counter > 0; ++i)
    {
      // @TODO: handle tabs

      //if (text.at(i) == QChar('\t'))
      //{
      //  counter -= tabSize();
      //  col += counter <= -tabSize() / 2 ? 0 : 1;
      //}
      //else
      //{
      counter -= 1;
      col += 1;
      //}
    }

    return Position{ it->block().blockNumber(), col };
  }
}

static bool map_pos_complex(const Position& pos, const view::Line& line, int& column_offset)
{
  for (auto elem = line.elements.begin(); elem != line.elements.end(); ++elem)
  {
    if (elem->kind == view::LineElement::LE_BlockFragment)
    {
      if (elem->block.blockNumber() != pos.line)
      {
        if (elem->begin <= pos.column && pos.column <= elem->begin + elem->width)
        {
          int count = pos.column - elem->begin;
          for (int i(0); i < count; ++i)
            column_offset += /* @TODO: fixtme : view->ncol(elem.text().at(i)); */ 1;
          return true;
        }
      }
      else
      {
        column_offset += elem->width;
      }
    }
    else
    {
      /* @TODO: fixme */
      //const auto& fold_info = view->folds.at(elem.foldid());
      //if (fold_info.start() < pos && pos < fold_info.end())
      //  return true;
      //column_offset += elem.colcount();
    }
  }

  return false;
}

QPoint QTypewriterView::map(const Position& pos) const
{
  auto visible_lines = visibleLines();

  if (pos.line < visible_lines.begin()->block().blockNumber())
    return QPoint{ 0, -metrics().descent };

  int line_offset = 0;
  int column_offset = 0;

  for (auto line = visible_lines.begin(); line_offset < visible_lines.size(); ++line, ++line_offset)
  {
    if (line->isInsert())
      continue;

    if (line->elements.size() > 1)
    {
      if (map_pos_complex(pos, *line, column_offset))
        break;
    }
    else
    {
      if (line->block().blockNumber() == pos.line)
      {
        column_offset = pos.column;
        break;
      }
    }
  }

  int dy = line_offset * metrics().lineheight + metrics().ascent;
  int dx = column_offset * metrics().charwidth;

  return QPoint{ dx - hscroll(), dy };
}

bool QTypewriterView::isVisible(const Position& pos) const
{
  return QRect(QPoint(0, 0), size()).contains(map(pos));
}

void QTypewriterView::blockDestroyed(int line, const TextBlock& block)
{
  Q_EMIT blockDestroyed();
  Q_EMIT lineCountChanged();

  // @TODO: check if update is really needed
  Q_EMIT invalidated();
}

void QTypewriterView::blockInserted(const Position& pos, const TextBlock& block)
{
  Q_EMIT blockInserted(pos.line);
  Q_EMIT lineCountChanged();

  // @TODO: check if update is really needed
  Q_EMIT invalidated();
}

void QTypewriterView::contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{
  // @TODO: check if update is really needed
  Q_EMIT invalidated();
}

QPainter& QTypewriterView::painter()
{
  return *m_painter;
}

void QTypewriterView::paint(std::list<typewriter::view::Line>::const_iterator begin, std::list<typewriter::view::Line>::const_iterator end)
{
  painter().setBrush(QBrush(m_default_format.background_color));
  painter().setPen(Qt::NoPen);
  painter().drawRect(QRect(QPoint(0, 0), size()));

  int i = 0;
  for (auto it = begin; it != end; ++it, ++i)
  {
    const int baseline = i * m_metrics.lineheight + m_metrics.ascent;
    drawLine(QPoint{-m_hscroll, baseline }, *it);
  }
}

void QTypewriterView::drawLine(const QPoint& offset, const view::Line& line)
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

void QTypewriterView::drawFoldSymbol(const QPoint& offset, int foldid)
{
  /// TODO: 
  throw std::runtime_error{ "Not implemented" };
}

void QTypewriterView::drawBlockFragment(QPoint offset, const view::Line& line, const view::LineElement& fragment)
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

void QTypewriterView::drawStrikeOut(const QPoint& offset, const TextFormat& fmt, int count)
{
  QPen pen{ fmt.strikeout_color };
  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  painter().setPen(pen);

  painter().drawLine(offset.x(), offset.y() - metrics().strikeoutpos, offset.x() + count * metrics().charwidth, offset.y() - metrics().strikeoutpos);
}

void QTypewriterView::drawUnderline(const QPoint& offset, const TextFormat& fmt, int count)
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


void QTypewriterView::drawWaveUnderline(const QPoint& offset, const TextFormat& fmt, int count)
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

void QTypewriterView::drawText(const QPoint& offset, const QString& text, const TextFormat& format)
{
  applyFormat(painter(), format);

  painter().drawText(offset, text);

  if (format.strikeout)
    drawStrikeOut(offset, format, text.length());

  drawUnderline(offset, format, text.length());
}

void QTypewriterView::applyFormat(QPainter& painter, const TextFormat& fmt)
{
  QFont f = painter.font();
  f.setBold(fmt.bold);
  f.setItalic(fmt.italic);
  painter.setFont(f);
  painter.setPen(QPen(fmt.text_color));
  painter.setBrush(QBrush(fmt.background_color));
}

details::QTypewriterVisibleLines QTypewriterView::visibleLines() const
{
  size_t count = size().height() / metrics().lineheight;
  return details::QTypewriterVisibleLines(view().lines(), linescroll(), count);
}

} // namespace typewriter
