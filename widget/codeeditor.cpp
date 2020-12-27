// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "codeeditor.h"

#include "typewriter/textblock.h"
#include "typewriter/private/texteditor_p.h"
#include "typewriter/view/fragment.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <QPainter>
#include <QPainterPath>

#include <QScrollBar>

#include <stdexcept>

namespace typewriter
{

//TextViewImpl::TextViewImpl(TextDocument* doc)
//  : document(doc)
//  , hpolicy(Qt::ScrollBarAsNeeded)
//  , vpolicy(Qt::ScrollBarAlwaysOn)
//  , firstDirtyBlock{ -1, 0 }
//  , gutter(nullptr)
//  , firstLine(0, 0, -1, 0, this)
//  , longestLine(0, 0, -1, 0, this)
//  , linecount(1)
//{
//  tabreplace = "    ";
//
//  auto it = doc->firstBlock();
//  do
//  {
//    this->blocks.append(std::make_shared<view::BlockInfo>(it));
//    it = it.next();
//  } while (it.isValid());
//}
//
//void TextViewImpl::calculateMetrics(const QFont& f)
//{
//  QFontMetrics fm{ f };
//  bool ok = f.exactMatch();
//  this->metrics.charwidth = fm.averageCharWidth();
//  this->metrics.lineheight = fm.height();
//  this->metrics.ascent = fm.ascent();
//  this->metrics.descent = fm.descent();
//  this->metrics.strikeoutpos = fm.strikeOutPos();
//  this->metrics.underlinepos = fm.underlinePos();
//}
//
//view::Line TextViewImpl::findLongestLine()
//{
//  auto lines = view::Lines{ this };
//  view::Line result = lines.begin();
//  int colcount = result.colcount();
//
//  const auto end = lines.end();
//  for (auto it = this->firstLine; it != end; ++it)
//  {
//    const int cc = it.colcount();
//    if (cc > colcount)
//    {
//      colcount = cc;
//      result = it;
//    }
//  }
//
//  return result;
//}
//
//void TextViewImpl::setLongestLine(const view::Line& line)
//{
//  this->longestLine = line;
//  this->hscrollbar->setRange(0, line.colcount() * this->metrics.charwidth);
//}
//
//int TextViewImpl::getFold(int blocknum, int from) const
//{
//  for (int i(from); i < folds.size(); ++i)
//  {
//    const auto& fi = folds.at(i);
//    if (fi.start().line > blocknum)
//      return -1;
//    else if (fi.start().line == blocknum)
//      return i;
//  }
//  return -1;
//}
//
//void TextViewImpl::addFold(const TextFold& f)
//{
//  this->folds.insert(f);
//
//  for (const auto& fold : this->folds)
//  {
//    if (fold.isActive())
//      relayout(fold.start().line);
//  }
//}
//
//void TextViewImpl::removeFold(int index)
//{
//  TextFold removed = this->folds.at(index);
//  this->folds.remove(this->folds.begin() + index);
//
//  if (removed.isActive())
//    relayout(removed.start().line);
//
//  for (const auto& fold : this->folds)
//  {
//    if (fold.isActive())
//      relayout(fold.start().line);
//  }
//}
//
//void TextViewImpl::activateFold(int index, bool active)
//{
//  if (this->folds.at(index).isActive() == active)
//    return;
//
//  this->folds.at(index).setActive(active);
//
//  for (const auto& fold : this->folds)
//  {
//    if (fold.isActive())
//      relayout(fold.start().line);
//  }
//}
//
//void TextViewImpl::relayout(int blocknum)
//{
//  /// TODO: handle word-wrap
//
//  int fold = getFold(blocknum);
//  if (fold == -1)
//  {
//    blockInfo(blocknum).display.clear();
//    return;
//  }
//
//  std::vector<std::unique_ptr<view::LineElement>> elems;
//  TextFold foldinfo = folds.at(fold);
//  elems.push_back(std::unique_ptr<view::LineElement>(new view::LineElement_BlockFragment{ 0, foldinfo.start().column }));
//  do
//  {
//    elems.push_back(std::unique_ptr<view::LineElement>(new view::LineElement_Fold{ fold }));
//
//    fold = getFold(foldinfo.end().line, fold);
//
//    if (fold == -1)
//    {
//      const view::Block b{ foldinfo.end().line, this };
//      elems.push_back(std::unique_ptr<view::LineElement>(new view::LineElement_BlockFragment{ foldinfo.end().column, b.block().length() }));
//    }
//    else
//    {
//      const int blockbegin = foldinfo.end().column;
//      foldinfo = folds.at(fold);
//      elems.push_back(std::unique_ptr<view::LineElement>(new view::LineElement_BlockFragment{ blockbegin, foldinfo.start().column }));
//    }
//  } while (fold != -1);
//}
//
//void TextViewImpl::seekFirstDirtyLine(view::Block previous)
//{
//  if (previous.isLast())
//  {
//    this->firstDirtyBlock.line = -1;
//    return;
//  }
//
//  view::Block it = previous;
//  do
//  {
//    it.seekNext();
//
//    if (it.impl().revision != it.block().revision() || it.impl().forceHighlighting)
//    {
//      this->firstDirtyBlock.line = it.number();
//      return;
//    }
//
//  } while (!it.isLast());
//
//  this->firstDirtyBlock.line = -1;
//}
//
//bool TextViewImpl::checkNeedsHighlighting(view::Block l)
//{
//  if (this->firstDirtyBlock.line == -1 || !this->syntaxHighlighter->usesBlockState() || this->firstDirtyBlock.line > l.number())
//    return false;
//
//  view::Block it = l;
//  it.seek(this->firstDirtyBlock.line);
//
//  while (it != l)
//  {
//    int state = it.userState();
//    invokeSyntaxHighlighter(it);
//    if (it.userState() == state)
//    {
//      this->seekFirstDirtyLine(it);
//      if (this->firstDirtyBlock.line > l.number() || this->firstDirtyBlock.line == -1)
//        it = l;
//      else
//        it.seek(this->firstDirtyBlock.line);
//    }
//    else
//    {
//      this->firstDirtyBlock.line += 1;
//      it.seekNext();
//    }
//  }
//
//  return this->firstDirtyBlock.line = l.number();
//}
//
//void TextViewImpl::highlightLine(view::Block l)
//{
//  if (this->firstDirtyBlock.line == -1 || !this->syntaxHighlighter->usesBlockState())
//  {
//    invokeSyntaxHighlighter(l);
//  }
//  else
//  {
//    if (this->firstDirtyBlock.line != -1 && this->firstDirtyBlock.line <= l.number())
//    {
//      if (this->firstDirtyBlock.line == l.number())
//      {
//        invokeSyntaxHighlighter(l);
//        this->seekFirstDirtyLine(l);
//      }
//      else
//      {
//        checkNeedsHighlighting(l);
//        const int state = l.userState();
//        invokeSyntaxHighlighter(l);
//        if (state != l.userState() && this->syntaxHighlighter->usesBlockState())
//          this->firstDirtyBlock.line = l.isLast() ? -1 : l.number() + 1;
//      }
//    }
//    else
//    {
//      const int state = l.userState();
//      invokeSyntaxHighlighter(l);
//      if (state != l.userState() && this->syntaxHighlighter->usesBlockState())
//        this->firstDirtyBlock.line = l.isLast() ? -1 : l.number() + 1;
//    }
//  }
//}
//
//int TextViewImpl::invokeSyntaxHighlighter(view::Block l)
//{
//  this->syntaxHighlighter->impl()->block = l;
//  l.impl().formats.clear();
//  this->syntaxHighlighter->highlightBlock(l.block().text());
//  l.impl().forceHighlighting = false;
//  l.impl().revision = l.block().revision();
//  return l.userState();
//}

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

QTypewriterContext::QTypewriterContext(QTypewriter* w, typewriter::TextDocument* doc)
  : widget(w),
    document(doc),
    view(doc)
{
  this->formats.resize(16);
}

QTypewriterVisibleLines QTypewriterContext::visibleLines() const
{
  size_t count = this->widget->viewport().height() / this->metrics.lineheight;
  return QTypewriterVisibleLines(this->view.lines(), this->first_visible_line, count);
}

void QTypewriterContext::blockDestroyed(int line, const TextBlock& block)
{
  widget->onBlockDestroyed(line, block);
}

void QTypewriterContext::blockInserted(const Position& pos, const TextBlock& block)
{
  widget->onBlockInserted(pos, block);
}

void QTypewriterContext::contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{
  widget->onContentsChange(block, pos, charsRemoved, charsAdded);
}

} // namespace details


QTypewriterGutter::QTypewriterGutter(std::shared_ptr<details::QTypewriterContext> context, QWidget* parent)
  : QWidget(parent)
  , d(context)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

QTypewriterGutter::~QTypewriterGutter()
{

}

QSize QTypewriterGutter::sizeHint() const
{
  return QSize{ d->metrics.charwidth * columnCount() + 4, 66 };
}

void QTypewriterGutter::paintEvent(QPaintEvent* e)
{
  QPainter painter{ this };

  painter.setBrush(QBrush(d->default_format.background_color));
  painter.setPen(Qt::NoPen);
  painter.drawRect(this->rect());

  painter.setFont(d->widget->font());
  painter.setPen(QPen(d->default_format.text_color));

  painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());

  auto it = std::next(d->view.lines().begin(), d->first_visible_line);

  const int count = 1 + d->widget->viewport().height() / d->metrics.lineheight;

  QString label = QString(" ").repeated(columnCount());

  for (int i(0); i < count && it != d->view.lines().end(); ++i, ++it)
  {
    if (it->elements.empty() || !it->elements.front().block.isValid())
      continue;

    writeNumber(label, it->elements.front().block.blockNumber() + 1);

    painter.drawText(QPoint(0, i * d->metrics.lineheight + d->metrics.ascent), label);
  }

  // @TODO: draw cursors
}

int QTypewriterGutter::columnCount() const
{
  int n = d->document->lineCount() + 1;
  int result = 1;
  while (n >= 10)
  {
    n /= 10;
    result += 1;
  }
  return result;
}

void QTypewriterGutter::writeNumber(QString& str, int n)
{
  for (int i(1); i <= str.size(); ++i)
  {
    str[str.size() - i] = (n == 0 ? ' ' : '0' + (n % 10));
    n /= 10;
  }
}

QTypewriter::QTypewriter(QWidget* parent)
  : QTypewriter(nullptr, parent)
{
}

QTypewriter::QTypewriter(TextDocument* document, QWidget* parent)
  : QWidget(parent),
    m_context(new details::QTypewriterContext(this, document))
{
  init();
}

QTypewriter::~QTypewriter()
{

}

void QTypewriter::init()
{
  //connect(document(), &TextDocument::blockDestroyed, this, &QTypewriter::onBlockDestroyed);
  //connect(document(), &TextDocument::blockInserted, this, &QTypewriter::onBlockInserted);
  //connect(document(), &TextDocument::contentsChange, this, &QTypewriter::onContentsChange);

  m_horizontal_scrollbar = new QScrollBar(Qt::Horizontal, this);
  connect(m_horizontal_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));

  m_vertical_scrollbar = new QScrollBar(Qt::Vertical, this);
  m_vertical_scrollbar->setRange(0, document()->lineCount() - 1);
  m_vertical_scrollbar->setValue(0);
  connect(m_vertical_scrollbar, &QScrollBar::valueChanged, this, &QTypewriter::setFirstVisibleLine);

  m_gutter = new QTypewriterGutter{ m_context, this };

  //d->syntaxHighlighter = new SyntaxHighlighter(this);

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(40);
    setFont(font);
  }
}

TextDocument* QTypewriter::document() const
{
  return m_context->document;
}

TextView& QTypewriter::view() const
{
  return m_context->view;
}

QTypewriterGutter* QTypewriter::gutter() const
{
  return m_gutter;
}

void QTypewriter::scroll(int delta)
{
  if (delta == 0)
    return;

  while (delta > 0 && m_context->first_visible_line < document()->lineCount() - 1)
  {
    m_context->first_visible_line += 1;
    delta -= 1;
  }

  while (delta < 0 && m_context->first_visible_line > 0)
  {
    m_context->first_visible_line -= 1;
    delta += 1;
  }

  m_vertical_scrollbar->setValue(m_context->first_visible_line);

  update();
}

void QTypewriter::setFirstVisibleLine(int n)
{
  if (m_context->first_visible_line == n)
    return;

  scroll(n - m_context->first_visible_line);
}

void QTypewriter::setTabSize(int n)
{
  if (tabSize() == n)
    return;

  m_context->view.setTabSize(n);

  update();
}

int QTypewriter::tabSize() const
{
  return m_context->view.tabSize();
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

  m_horizontal_scrollbar->setRange(0, m_context->view.width() * m_context->metrics.charwidth);
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
  m_vertical_scrollbar->setRange(0, document()->lineCount() - 1);
  m_vertical_scrollbar->setValue(m_context->first_visible_line);
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
  const int line_offset = (pos.y() - viewport().top()) / m_context->metrics.lineheight;
  const int column_offset = std::round((pos.x() + hscroll() - viewport().left()) / float(m_context->metrics.charwidth));

  auto visible_lines = m_context->visibleLines();

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

QPoint QTypewriter::mapToViewport(const Position& pos) const
{
  auto visible_lines = m_context->visibleLines();

  if (pos.line < visible_lines.begin()->block().blockNumber())
    return QPoint{ 0, -m_context->metrics.descent };

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

  int dy = line_offset * m_context->metrics.lineheight + m_context->metrics.ascent;
  int dx = column_offset * metrics().charwidth;

  return QPoint{ dx - hscroll(), dy };
}

QPoint QTypewriter::map(const Position& pos) const
{
  return mapToViewport(pos) + viewport().topLeft();
}

bool QTypewriter::isVisible(const Position& pos) const
{
  return viewport().contains(map(pos));
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
  return m_context->default_format;
}

void QTypewriter::setDefaultFormat(const TextFormat& format)
{
  m_context->default_format = format;
  m_context->formats[0] = format;
  update();
}

const TextFormat& QTypewriter::textFormat(int id) const
{
  return m_context->formats.at(id);
}

void QTypewriter::setTextFormat(int id, const TextFormat& fmt)
{
  m_context->formats[id] = fmt;
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

void QTypewriter::onBlockDestroyed(int line, const TextBlock& block)
{
  m_vertical_scrollbar->setMaximum(m_context->view.height());
  updateLayout();

  // @TODO: check if update is really needed
  update();
}

void QTypewriter::onBlockInserted(const Position& pos, const TextBlock& block)
{
  m_vertical_scrollbar->setMaximum(m_context->view.height());
  updateLayout();

  // @TODO: check if update is really needed
  update();
}

void QTypewriter::onContentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded)
{
  // @TODO: check if update is really needed
  update();
}

void QTypewriter::paintEvent(QPaintEvent* e)
{
  QPainter painter{ this };
  setupPainter(&painter);
  paint(&painter);
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
  m_context->metrics = QTypewriterFontMetrics(this->font());
}

void QTypewriter::setupPainter(QPainter* painter)
{
  painter->setFont(font());
  painter->setClipRect(m_viewport);
}

void QTypewriter::paint(QPainter* painter)
{
  painter->setBrush(QBrush(m_context->default_format.background_color));
  painter->setPen(Qt::NoPen);
  painter->drawRect(m_viewport);

  auto it = std::next(m_context->view.lines().begin(), m_context->first_visible_line);

  const int numline = 1 + m_viewport.height() / m_context->metrics.lineheight;

  for (int i(0); i < numline && it != m_context->view.lines().end(); ++i, ++it)
  {
    const int baseline = i * m_context->metrics.lineheight + m_context->metrics.ascent;
    drawLine(painter, QPoint{ m_viewport.left() - m_horizontal_scrollbar->value(), baseline }, *it);
  }
}

void QTypewriter::drawLine(QPainter* painter, const QPoint& offset, const view::Line& line)
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
      drawFoldSymbol(painter, pt, e.id);
      pt.rx() += e.width * m_context->metrics.charwidth;
    }
    else if (e.kind == view::LineElement::LE_BlockFragment)
    {
      drawBlockFragment(painter, pt, line, e);
      pt.rx() += e.width * m_context->metrics.charwidth;
    }
  }

}

void QTypewriter::drawFoldSymbol(QPainter* painter, const QPoint& offset, int foldid)
{
  /// TODO: 
  throw std::runtime_error{ "Not implemented" };
}

void QTypewriter::drawBlockFragment(QPainter* painter, QPoint offset, const view::Line& line, const view::LineElement& fragment)
{
  view::StyledFragments fragments = m_context->view.fragments(line, fragment);

  for (auto it = fragments.begin(); it != fragments.end(); it = it.next())
  {
    //QString text = QString::fromStdString(fragment.block.text().substr(fragment.begin, fragment.width));
    //drawText(painter, offset, text, m_context->default_format);
    QString text = QString::fromStdString(it.text());
    drawText(painter, offset, text, textFormat(it.format()));
    offset.rx() += it.length() * m_context->metrics.charwidth;
  }
}

void QTypewriter::drawStrikeOut(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count)
{
  QPen pen{ fmt.strikeout_color };
  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  painter->setPen(pen);

  painter->drawLine(offset.x(), offset.y() - metrics().strikeoutpos, offset.x() + count * metrics().charwidth, offset.y() - metrics().strikeoutpos);
}

void QTypewriter::drawUnderline(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count)
{
  if (fmt.underline == TextFormat::NoUnderline)
  {
    return;
  }
  else if (fmt.underline == TextFormat::WaveUnderline)
  {
    drawWaveUnderline(painter, offset, fmt, count);
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

    painter->setPen(pen);
    //painter->drawLine(offset.x(), offset.y() + metrics().underlinepos, offset.x() + count * metrics().charwidth, offset.y() + metrics().underlinepos);
    painter->drawLine(offset.x(), offset.y() + metrics().descent - penwidth - 1, offset.x() + count * metrics().charwidth, offset.y() + metrics().descent - penwidth - 1);
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

static void fill_with_pattern(QPainter* painter, const QRect& rect, const QPixmap& pattern)
{
  const int y = rect.top();

  int x = rect.left();

  const int full = rect.width() / pattern.width();
  for (int i(0); i < full; ++i)
  {
    painter->drawPixmap(x, y, pattern);
    x += pattern.width();
  }

  const int partial_width = rect.width() - pattern.width() * full;

  if (partial_width > 0)
    painter->drawPixmap(QPoint(x, y), pattern, QRect(0, 0, partial_width, pattern.height()));
}

void QTypewriter::drawWaveUnderline(QPainter* painter, const QPoint& offset, const TextFormat& fmt, int count)
{
  /// TODO:
  QPen pen{ fmt.underline_color };

  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  pen.setJoinStyle(Qt::RoundJoin);

  QPixmap pattern = generate_wave_pattern(pen, metrics());
  QRect rect{ offset.x(), offset.y() + metrics().descent - pattern.height(), count * metrics().charwidth, pattern.height() };

  fill_with_pattern(painter, rect, pattern);
}

//void QTypewriter::drawFragment(QPainter* painter, QPoint& offset, view::Fragment fragment)
//{
//  const TextFormat& fmt = fragment.format();
//  applyFormat(painter, fmt);
//
//  QString text = fragment.text();
//  painter->drawText(offset, text);
//
//  if (fmt.strikeout)
//    drawStrikeOut(painter, offset, fmt, text.length());
//
//  drawUnderline(painter, offset, fragment.format(), text.length());
//
//  offset.rx() += text.length() * m_context->metrics.charwidth;
//}

void QTypewriter::drawText(QPainter* painter, const QPoint& offset, const QString& text, const TextFormat& format)
{
  applyFormat(painter, format);

  painter->drawText(offset, text);

  if (format.strikeout)
    drawStrikeOut(painter, offset, format, text.length());

  drawUnderline(painter, offset, format, text.length());
}

void QTypewriter::applyFormat(QPainter* painter, const TextFormat& fmt)
{
  QFont f = painter->font();
  f.setBold(fmt.bold);
  f.setItalic(fmt.italic);
  painter->setFont(f);
  painter->setPen(QPen(fmt.text_color));
  painter->setBrush(QBrush(fmt.background_color));
}

const QTypewriterFontMetrics& QTypewriter::metrics() const
{
  return m_context->metrics;
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
    if (available_width > m_context->view.width() * m_context->metrics.charwidth)
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
    update();
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
