// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textview.h"
#include "textedit/private/textview_p.h"

#include "textedit/view/fragment.h"
#include "textedit/view/line.h"
#include "textedit/view/lineelement.h"
#include "textedit/private/gutter_p.h"
#include "textedit/private/syntaxhighlighter_p.h"

#include <QPainter>
#include <QWheelEvent>

#include <QDebug>

namespace textedit
{

namespace view
{

BlockInfo::BlockInfo(const TextBlock & b)
  : block(b)
  , userstate(0)
  , forceHighlighting(true)
  , revision(-1)
{

}

BlockInfo::~BlockInfo()
{

}

Block::Block()
  : mNumber(-1)
  , mView(nullptr)
{

}

Block::Block(const Block & other)
  : mNumber(other.mNumber)
  , mView(other.mView)
{

}

Block::Block(TextViewImpl *view)
  : mNumber(0)
  , mView(view)
{

}

Block::Block(int num, TextViewImpl *view)
  : mNumber(num)
  , mView(view)
{

}

Block::~Block()
{

}


TextBlock Block::block() const
{
  return impl().block;
}

Block Block::next() const
{
  Block ret = *this;
  ret.seekNext();
  return ret;
}

Block Block::previous() const
{
  Block ret = *this;
  ret.seekPrevious();
  return ret;
}

void Block::seekNext()
{
  TextBlock next = block().next();
  Q_ASSERT(!next.isNull());
  mNumber++;
}

void Block::seekPrevious()
{
  mNumber--;
}

void Block::seek(int num)
{
  if (num < mNumber)
  {
    while (num < mNumber)
      seekPrevious();
  }
  else if (num > mNumber)
  {
    while (num > mNumber)
      seekNext();
  }
}

Block Block::nextVisibleLine() const
{
  ActiveFold f = getFold();
  if (f.begin.line != mNumber)
    return next();
  return f.endline;
}

bool Block::isFirst() const
{
  return mNumber == 0;
}

bool Block::isLast() const
{
  return mNumber == block().document()->lineCount() - 1;
}

bool Block::needsRehighlight() const
{
  return impl().forceHighlighting || impl().revision != block().revision()
    || mView->checkNeedsHighlighting(*this);
}

void Block::rehighlight()
{
  mView->highlightLine(*this);
}

void Block::rehighlightLater()
{
  impl().forceHighlighting = true;
  if (mNumber < mView->firstDirtyLine.line)
    mView->firstDirtyLine.line = mNumber;
}

const QVector<FormatRange> & Block::formats() const
{
  return impl().formats;
}

const int Block::userState() const
{
  return impl().userstate;
}

const QVector<FoldPosition> & Block::foldPositions() const
{
  return impl().folds;
}

bool Block::hasActiveFold() const
{
  ActiveFold f = getFold();
  return f.begin.line == mNumber;
}

std::pair<Position, Position> Block::activeFold() const
{
  ActiveFold f = getFold();
  return std::make_pair(f.begin, f.end);
}

int Block::span() const
{
  return 1;
}

QString Block::displayedText() const
{
  /// TODO: take folds into account ?
  QString ret = block().text();
  ret.replace("\t", mView->tabreplace);
  return ret;
}

int Block::columnWidth() const
{
  /// TODO:
  throw std::runtime_error{ "Not implemented" };
}

Fragment Block::begin() const
{
  return Fragment{ &(impl()), 0, formats().begin(), formats().end(), mView };
}

Fragment Block::end() const
{
  return Fragment{ &(impl()), block().length(), formats().end(), formats().end(), mView };
}

BlockInfo & Block::impl()
{
  return mView->blockInfo(mNumber);
}

const BlockInfo & Block::impl() const
{
  return mView->blockInfo(mNumber);
}

bool Block::operator==(const Block & other) const
{
  return mNumber == other.mNumber;
}

bool Block::operator!=(const Block & other) const
{
  return mNumber != other.mNumber;
}

bool Block::operator<(const Block & other) const
{
  return mNumber < other.mNumber;
}

void Block::notifyBlockDestroyed(int linenum)
{
  if (linenum < mNumber)
  {
    mNumber -= 1;
  }
  else if (linenum == mNumber)
  {
    seekPrevious();
    impl().forceHighlighting = true;
  }
}

void Block::notifyBlockInserted(const Position & pos)
{
  if (pos.line < mNumber)
    mNumber += 1;
}

ActiveFold Block::getFold() const
{
  for (const auto & f : mView->activeFolds)
  {
    if (f.begin.line == mNumber)
      return f;
    else if (f.begin.line > mNumber)
      break;
  }

  return ActiveFold{};
}

Blocks::Blocks(TextViewImpl *view)
  : mView(view)
{

}

int Blocks::count() const
{
  return mView->blocks.size();
}

Block Blocks::begin() const
{
  return Block{ 0, mView };
}

Block Blocks::end() const
{
  return Block{ mView->blocks.size(), mView };
}

Block Blocks::at(int index) const
{
  return Block{ std::max(std::min(index, mView->blocks.size()), 0), mView };
}

Fragment::Fragment()
{

}

Fragment::~Fragment()
{

}

Fragment::Fragment(BlockInfo const *line, int col, QVector<FormatRange>::const_iterator iter, QVector<FormatRange>::const_iterator sentinel, TextViewImpl const *view)
  : mLine(line)
  , mColumn(col)
  , mIterator(iter)
  , mSentinel(sentinel)
  , mView(view)
{

}

const TextFormat & Fragment::format() const
{
  return (mIterator == mSentinel || mColumn < mIterator->start) ? mView->defaultFormat : mIterator->format;
}

int Fragment::position() const
{
  return mColumn;
}

int Fragment::length() const
{
  return mIterator == mSentinel ? (mLine->block.length() - mColumn) : (mColumn < mIterator->start ? mIterator->start - mColumn : mIterator->length);
}

QString Fragment::text() const
{
  return mLine->block.text().mid(position(), length()).replace("\t", mView->tabreplace);
}

Fragment Fragment::next() const
{
  if (mIterator == mSentinel)
    return Fragment{ mLine, mLine->block.length(), mSentinel, mSentinel, mView };
  else if (mColumn < mIterator->start)
    return Fragment{ mLine, mIterator->start, mIterator, mSentinel, mView };
  else
    return Fragment{ mLine, mColumn + mIterator->length, std::next(mIterator), mSentinel, mView };
}

bool Fragment::operator==(const Fragment & other) const
{
  return mLine == other.mLine && other.mColumn == mColumn;
}

bool Fragment::operator!=(const Fragment & other) const
{
  return mLine != other.mLine || other.mColumn != mColumn;
}


bool LineElement::isBlockFragment() const
{
  return false;
}

bool LineElement::isFold() const
{
  return false;
}

bool LineElement::isLineFeed() const
{
  return false;
}

const LineElement_Fold & LineElement::asFold() const
{
  return *static_cast<const LineElement_Fold*>(this);
}

const LineElement_BlockFragment & LineElement::asBlockFragment() const
{
  return *static_cast<const LineElement_BlockFragment*>(this);
}

bool LineElement_BlockFragment::isBlockFragment() const
{
  return true;
}

bool LineElement_Fold::isFold() const
{
  return true;
}

bool LineElement_LineFeed::isLineFeed() const
{
  return true;
}


LineElements::LineElements(TextViewImpl*view, int bn, int row)
  : mView(view), mBlockNumber(bn), mRow(row)
{

}

int LineElements::count() const
{
  const auto & dis = mView->blockInfo(mBlockNumber).display;
  int count = 1;
  for (const auto & e : dis)
    count += e->isLineFeed() ? 1 : 0;
  return count;
}

LineElements::Iterator LineElements::begin()
{
  return Iterator{ mView, mBlockNumber, mBlockNumber, 0 };
}

LineElements::Iterator LineElements::end()
{
  const auto & dis = mView->blockInfo(mBlockNumber).display;
  for (int i = int(dis.size()) - 1; i >= 0; --i)
  {
    if (dis.at(i)->isFold())
    {
      const int fold_id = dis.at(i)->asFold().foldid;
      const auto& fold_info = mView->activeFolds.at(fold_id);
      return Iterator{ mView, mBlockNumber, fold_info.end.line, (int)dis.size() };
    }
  }

  return Iterator{ mView, mBlockNumber, mBlockNumber, (int)dis.size() };
}

LineElements::Iterator::Iterator(TextViewImpl *view, int bn, int tb, int ei)
  : mView(view)
  , mBlockNumber(bn)
  , mTargetBlock(tb)
  , mElementIndex(ei)
{
  
}

bool LineElements::Iterator::isFold() const
{
  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->isFold();
}

bool LineElements::Iterator::isBlockFragment() const
{
  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->isBlockFragment();
}

int LineElements::Iterator::block() const
{
  return mTargetBlock;
}

int LineElements::Iterator::blockBegin() const
{
  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->asBlockFragment().begin;
}

int LineElements::Iterator::blockEnd() const
{
  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->asBlockFragment().end;
}

bool LineElements::Iterator::operator==(const Iterator & other) const
{
  return mBlockNumber == other.mBlockNumber && mElementIndex == other.mElementIndex;
}

bool LineElements::Iterator::operator!=(const Iterator & other) const
{
  return mBlockNumber != other.mBlockNumber || mElementIndex != other.mElementIndex;
}


Line::Line(TextViewImpl *view)
  : mNumber(0), mBlockNumber(0), mWidgetNumber(-1), mRow(0), mView(view)
{

}

Line::Line(int num, int blocknum, int widgetnum, int row, TextViewImpl *view)
  : mNumber(num), mBlockNumber(blocknum), mWidgetNumber(widgetnum), mRow(row), mView(view)
{

}

Line Line::next() const
{
  if (isWidget())
  {
    if (row() + 1 < widgetSpan())
      return Line{ number() + 1, mBlockNumber, mWidgetNumber, row() + 1, mView };

    return Line{ number() + 1, mBlockNumber + 1, -1, 0, mView };
  }
  else
  {
    if (row() + 1 < block().span())
      return Line{ number() + 1, mBlockNumber, -1, row() + 1, mView };
    
    /* Check for folds */
    const view::BlockInfo & bi = mView->blockInfo(mBlockNumber);
    for (int i = int(bi.display.size()) - 1; i >= 0; --i)
    {
      if (bi.display.at(i)->isFold())
      {
        const int fold_id = bi.display.at(i)->asFold().foldid;
        const int block_num = mView->activeFolds.at(fold_id).end.line + 1;
        return Line{ number() + 1, block_num , -1, 0, mView };
      }
    }

    /* Check for widgets */
    for (int i(0); i < mView->widgets.size(); ++i)
    {
      if (mView->widgets.at(i).block == mBlockNumber)
        return Line{ number() + 1, mBlockNumber, i, 0, mView };
    }

    return Line{ number() + 1, mBlockNumber, -1, row() + 1, mView };
  }
}

static view::Block iterate_fold_backward(TextViewImpl *view, int fold_id)
{
  const auto & fold_info = view->activeFolds.at(fold_id);

  if (fold_id == 0)
    return view::Block{ fold_info.begin.line, view };

  const auto & prev_fold = view->activeFolds.at(fold_id - 1);
  if (prev_fold.end.line == fold_info.begin.line)
    return iterate_fold_backward(view, fold_id - 1);

  return view::Block{ fold_info.begin.line, view };
}

Line Line::previous() const
{
  if (mRow > 0)
    return Line{ number() - 1, mBlockNumber, mWidgetNumber, row() - 1, mView };

  Q_ASSERT(mRow == 0);

  if (isWidget())
  {
    return Line{ number() - 1, mBlockNumber, -1, block().span() - 1, mView };
  }
  else
  {
    /* Check for folds */
    for (int i = mView->activeFolds.size() - 1; i >= 0; --i)
    {
      const auto & fold_info = mView->activeFolds.at(i);
      if (fold_info.end.line == mBlockNumber - 1)
      {
        const view::Block dest_block = iterate_fold_backward(mView, i);
        return Line{ number() - 1, dest_block.number(), -1, dest_block.span() - 1, mView };
      }
    }

    /* Check for widgets */
    for (int i(0); i < mView->widgets.size(); ++i)
    {
      if (mView->widgets.at(i).block == mBlockNumber - 1)
        return Line{ number() - 1, mBlockNumber - 1, i, mView->widgets.at(i).span - 1, mView };
    }

    return Line{ number() - 1, mBlockNumber - 1, -1, view::Block{mBlockNumber - 1, mView}.span() - 1, mView };
  }
}

void Line::seekNext()
{
  *this = next();
}

void Line::seekPrevious()
{
  *this = previous();
}

void Line::seek(int num)
{
  num = std::max(0, std::min(num, mView->linecount - 1));

  while (num < mNumber)
    seekPrevious();
  while (num > mNumber)
    seekNext();
}

bool Line::isFirst() const
{
  return mNumber == 0;
}

bool Line::isLast() const
{
  return mNumber == mView->linecount - 1;
}

QWidget* Line::widget() const
{
  Q_ASSERT(isWidget());
  return mView->widgets.at(mWidgetNumber).widget;
}

int Line::widgetSpan() const
{
  Q_ASSERT(isWidget());
  return mView->widgets.at(mWidgetNumber).span;
}

bool Line::isBlock() const
{
  if (isWidget() || mRow != 0)
    return false;

  return mView->blockInfo(mBlockNumber).display.empty();
}

int Line::blockNumber() const
{
  return mBlockNumber;
}

view::Block Line::block() const
{
  return view::Block{ mBlockNumber, mView };
}

bool Line::isComplex() const
{
  return !isWidget() && !mView->blockInfo(mBlockNumber).display.empty();
}

void Line::notifyCharsAddedOrRemoved(const Position & pos, int added, int removed, int spanBefore)
{
  if (pos.line > mBlockNumber)
    return;

  if (mBlockNumber > pos.line)
  {
    mNumber += view::Block{ mBlockNumber, mView }.span() - spanBefore;
  }
  else
  {
    Q_ASSERT(mBlockNumber == pos.line);

    const int span = view::Block{ pos.line, mView }.span();

    if (isWidget())
    {
      const int diff = span - spanBefore;
      mNumber += diff;
    }
    else
    {
      if (mRow >= span)
      {
        const int diff = 1 + mRow - span;
        mNumber -= diff;
        mRow -= span;
      }
    }
  }
}

void Line::notifyBlockDestroyed(int linenum, const int span)
{
  if (mBlockNumber > linenum)
    return;

  if (mBlockNumber > linenum)
  {
    mBlockNumber -= 1;
    mNumber -= span;
  }
  else
  {
    Q_ASSERT(mBlockNumber == linenum);

    mBlockNumber -= 1;
    mNumber -= span;

    if (!isWidget())
      mRow = view::Block{ mBlockNumber, mView }.span() - 1;
  }
}

void Line::notifyBlockInserted(const Position & pos, const int spanBefore)
{
  if (mBlockNumber < pos.line)
    return;

  if (mBlockNumber > pos.line)
  {
    mBlockNumber += 1;

    const int span_diff = spanBefore - view::Block{ pos.line, mView }.span();
    Q_ASSERT(span_diff >= 0);
    mNumber += span_diff + view::Block{ pos.line + 1, mView }.span();
  }
  else
  {
    Q_ASSERT(mBlockNumber == pos.line);

    // Things get complicated here...
    const int span = view::Block{ pos.line, mView }.span();

    if (isWidget())
    {
      const int diff = spanBefore - span;
      Q_ASSERT(diff >= 0);
      mNumber -= diff;
    }
    else
    {
      if (mRow >= span)
      {
        const int diff = 1 + mRow - span;
        mNumber -= diff;
        mRow -= span;
      }
    }
  }
}

Lines::Lines(TextViewImpl *view, int block)
  : mView(view), mBlock(block)
{

}

int Lines::count() const
{
  if (mBlock < 0)
    return mView->linecount;
  return view::Block{ mBlock, mView }.span();
}

Line Lines::begin() const
{
  return Line{ 0, std::max(0, mBlock), -1, 0, mView };
}

Line Lines::end() const
{
  if (mBlock < 0)
  {
    const auto last_block = view::Block{ mView->blocks.size() - 1, mView };

    /* Check for widgets */
    if(!mView->widgets.empty())
    {
      if(mView->widgets.back().block == last_block.number())
        return Line{ mView->linecount - 1, last_block.number(), mView->widgets.size() - 1, mView->widgets.last().span - 1, mView };
    }

    /* Check for folds */
    if (!mView->activeFolds.empty())
    {
      if (mView->activeFolds.back().end.line == last_block.number())
      {
        auto block = iterate_fold_backward(mView, mView->activeFolds.size() - 1);
        return Line{ mView->linecount - 1, block.number(), -1, block.span() - 1, mView };
      }
    }

    return Line{ mView->linecount - 1, last_block.number(), -1, last_block.span() - 1, mView };
  }
  else
  {
    auto b = view::Block{ mBlock, mView };
    return Line{ b.span() - 1, b.number(), -1, b.span() - 1, mView };
  }
}

Line Lines::at(int index) const
{
  auto result = begin();
  result.seek(index);
  return result;
}

ActiveFold::ActiveFold()
  : begin{-1, -1}
  , end{-1, -1}
{

}

ActiveFold::ActiveFold(const Position & b, const Position & e, const Block & el)
  : begin(b)
  , end(e)
  , endline(el)
{

}

} // namespace view


TextViewImpl::TextViewImpl(const TextDocument *doc)
  : hpolicy(Qt::ScrollBarAsNeeded)
  , vpolicy(Qt::ScrollBarAlwaysOn)
  , firstDirtyLine{-1, 0}
  , gutter(nullptr)
{
  tabreplace = "    ";

  auto it = doc->firstBlock();
  do
  {
    this->blocks.append(std::make_shared<view::BlockInfo>(it));
    it = it.next();
  } while (it.isValid());

  this->firstBlock = view::Block{ this };
}

void TextViewImpl::calculateMetrics(const QFont & f)
{
  QFontMetrics fm{ f };
  bool ok = f.exactMatch();
  this->metrics.charwidth = fm.averageCharWidth();
  this->metrics.lineheight = fm.height();
  this->metrics.ascent = fm.ascent();
  this->metrics.descent = fm.descent();
  this->metrics.strikeoutpos = fm.strikeOutPos();
  this->metrics.underlinepos = fm.underlinePos();
}

TextBlock TextViewImpl::findLongestLine() const
{
  TextBlock result = this->firstBlock.block();
  TextBlock it = result.next();

  while (!it.isNull())
  {
    if (it.length() > result.length())
      result = it;

    it = it.next();
  }

  return result;
}

void TextViewImpl::setLongestLine(const TextBlock & block)
{
  this->longestLine = block;
  this->hscrollbar->setRange(0, block.length() * this->metrics.charwidth);
}

void TextViewImpl::seekFirstDirtyLine(view::Block previous)
{
  if (previous.isLast())
  {
    this->firstDirtyLine.line = -1;
    return;
  }

  view::Block it = previous;
  do
  {
    it.seekNext();

    if (it.impl().revision != it.block().revision() || it.impl().forceHighlighting)
    {
      this->firstDirtyLine.line = it.number();
      return;
    }

  } while (!it.isLast());

  this->firstDirtyLine.line = -1;
}

bool TextViewImpl::checkNeedsHighlighting(view::Block l)
{
  if (this->firstDirtyLine.line == -1 || !this->syntaxHighlighter->usesBlockState() || this->firstDirtyLine.line > l.number())
    return false;

  view::Block it = l;
  it.seek(this->firstDirtyLine.line);

  while (it != l)
  {
    int state = it.userState();
    invokeSyntaxHighlighter(it);
    if (it.userState() == state)
    {
      this->seekFirstDirtyLine(it);
      if (this->firstDirtyLine.line > l.number() || this->firstDirtyLine.line == -1)
        it = l;
      else
        it.seek(this->firstDirtyLine.line);
    }
    else
    {
      this->firstDirtyLine.line += 1;
      it.seekNext();
    }
  }

  return this->firstDirtyLine.line = l.number();
}

void TextViewImpl::highlightLine(view::Block l)
{
  if (this->firstDirtyLine.line == -1 || !this->syntaxHighlighter->usesBlockState())
  {
    invokeSyntaxHighlighter(l);
  }
  else
  {
    if (this->firstDirtyLine.line != -1 && this->firstDirtyLine.line <= l.number())
    {
      if (this->firstDirtyLine.line == l.number())
      {
        invokeSyntaxHighlighter(l);
        this->seekFirstDirtyLine(l);
      }
      else
      {
        checkNeedsHighlighting(l);
        const int state = l.userState();
        invokeSyntaxHighlighter(l);
        if (state != l.userState() && this->syntaxHighlighter->usesBlockState())
          this->firstDirtyLine.line = l.isLast() ? -1 : l.number() + 1;
      }
    }
    else
    {
      const int state = l.userState();
      invokeSyntaxHighlighter(l);
      if (state != l.userState() && this->syntaxHighlighter->usesBlockState())
        this->firstDirtyLine.line = l.isLast() ? -1 : l.number() + 1;
    }
  }
}

int TextViewImpl::invokeSyntaxHighlighter(view::Block l)
{
  this->syntaxHighlighter->impl()->block = l;
  l.impl().folds.clear();
  l.impl().formats.clear();
  this->syntaxHighlighter->highlightBlock(l.block().text());
  l.impl().forceHighlighting = false;
  l.impl().revision = l.block().revision();
  return l.userState();
}

TextView::TextView(const TextDocument *document)
  : d(new TextViewImpl(document))
{
  connect(document, &TextDocument::blockDestroyed, this, &TextView::onBlockDestroyed);
  connect(document, &TextDocument::blockInserted, this, &TextView::onBlockInserted);
  connect(document, &TextDocument::contentsChange, this, &TextView::onContentsChange);

  d->hscrollbar = new QScrollBar(Qt::Horizontal, this);
  connect(d->hscrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));

  d->vscrollbar = new QScrollBar(Qt::Vertical, this);
  d->vscrollbar->setRange(0, document->lineCount() - 1);
  d->vscrollbar->setValue(0);
  connect(d->vscrollbar, &QScrollBar::valueChanged, this, &TextView::setFirstVisibleLine);

  d->gutter = new Gutter{ this };
  d->gutter->impl()->view = this;

  d->setLongestLine(d->findLongestLine());

  d->syntaxHighlighter = new SyntaxHighlighter(this);

  {
    QFont font{ "Courier" };
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPixelSize(40);
    setFont(font);
  }
}

TextView::~TextView()
{

}

const TextDocument * TextView::document() const
{
  return firstVisibleBlock().document();
}

view::Blocks TextView::blocks() const
{
  return view::Blocks{ this->d.get() };
}

int TextView::firstVisibleLine() const
{
  return d->firstBlock.number();
}

TextBlock TextView::firstVisibleBlock() const
{
  return d->firstBlock.block();
}

void TextView::scroll(int delta)
{
  if (delta == 0)
    return;

  while (delta > 0 && d->firstBlock.number() < document()->lineCount() - 1)
  {
    d->firstBlock.seekNext();
    delta -= 1;
  }

  while (delta < 0 && d->firstBlock.number() > 0)
  {
    d->firstBlock.seekPrevious();
    delta += 1;
  }

  d->vscrollbar->setValue(d->firstBlock.number());

  update();
}

void TextView::setFirstVisibleLine(int n)
{
  if (d->firstBlock.number() == n)
    return;

  scroll(n - firstVisibleLine());
}

int TextView::visibleLineCount() const
{
  int numline = d->viewport.height() / d->metrics.lineheight;
  if (d->metrics.lineheight * d->viewport.height() < numline)
    numline++;
  return std::min(document()->lineCount() - firstVisibleLine(), numline);
}

int TextView::lastVisibleLine() const
{
  const int numline = visibleLineCount() - 1;
  return std::min(document()->lineCount() - 1, firstVisibleLine() + numline);
}

void TextView::setFont(const QFont & f)
{
  /// TODO: check typewriter 
  d->font = f;
  d->calculateMetrics(d->font);

  update();
}

const QFont & TextView::font() const
{
  return d->font;
}


void TextView::setTabSize(int n)
{
  if (tabSize() == n)
    return;

  d->tabreplace = QString(" ").repeated(std::max(n, 0));
  update();
}

int TextView::tabSize() const
{
  return d->tabreplace.size();
}

QScrollBar* TextView::horizontalScrollBar() const
{
  return d->hscrollbar;
}

void TextView::setHorizontalScrollBar(QScrollBar *scrollbar)
{
  int hscroll = d->hscrollbar->value();

  d->hscrollbar->deleteLater();
  d->hscrollbar = scrollbar;

  d->hscrollbar->setRange(0, d->longestLine.length() * d->metrics.charwidth);
  d->hscrollbar->setValue(hscroll);

  connect(d->hscrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));

  updateLayout();
}

Qt::ScrollBarPolicy TextView::horizontalScrollBarPolicy() const
{
  return d->hpolicy;
}

void TextView::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
  if (d->hpolicy == policy)
    return;

  d->hpolicy = policy;
  updateLayout();
}


QScrollBar* TextView::verticalScrollBar() const
{
  return d->vscrollbar;
}

void TextView::setVerticalScrollBar(QScrollBar *scrollbar)
{
  d->vscrollbar->deleteLater();
  d->vscrollbar = scrollbar;
  d->vscrollbar->setRange(0, document()->lineCount() - 1);
  d->vscrollbar->setValue(d->firstBlock.number());
  connect(d->vscrollbar, &QScrollBar::valueChanged, this, &TextView::setFirstVisibleLine);
  updateLayout();
}

Qt::ScrollBarPolicy TextView::verticalScrollBarPolicy() const
{
  return d->vpolicy;
}

void TextView::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
  if (d->vpolicy == policy)
    return;

  d->vpolicy = policy;
  updateLayout();
}

int TextView::hscroll() const
{
  return d->hscrollbar->value();
}

const QRect & TextView::viewport() const
{
  return d->viewport;
}

Position TextView::hitTest(const QPoint & pos) const
{
  const int line_offset = (pos.y() - viewport().top()) / d->metrics.lineheight;
  const int column_offset = std::round((pos.x() + hscroll() - viewport().left()) / float(d->metrics.charwidth));

  /* Seek visible line */
  auto it = d->firstBlock;
  for (int i(0); i < line_offset; ++i)
  {
    if (it.isLast())
      break;

    it = it.nextVisibleLine();
  }

  /* Take into account tabulations */
  const QString & text = it.text();
  int col = 0;
  for (int i(0), counter(column_offset); i < text.length() && counter > 0; ++i)
  {
    if (text.at(i) == QChar('\t'))
    {
      counter -= tabSize();
      col += counter <= -tabSize() / 2 ? 0 : 1;
    }
    else
    {
      counter -= 1;
      col += 1;
    }
  }

  return Position{ it.number(), col };
}

QPoint TextView::mapToViewport(const Position & pos) const
{
  /// TODO: take into account invisible lines
  int dy = (pos.line - firstVisibleLine()) * d->metrics.lineheight + d->metrics.ascent;
  int dx = pos.column * metrics().charwidth;

  if (viewport().contains(QPoint(dx - hscroll(), dy)))
  {
    /* We need to take into account tabulations */
    auto it = d->firstBlock;
    it.seek(pos.line);
    int charincrement = it.text().leftRef(pos.column).count(QChar('\t')) * (tabSize() - 1);
    dx += charincrement * metrics().charwidth;
  }

  return QPoint{ dx - hscroll(), dy };
}

QPoint TextView::map(const Position & pos) const
{
  return mapToViewport(pos) + viewport().topLeft();
}

bool TextView::isVisible(const Position &pos) const
{
  return viewport().contains(map(pos));
}

void TextView::fold(int line)
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}

void TextView::unfold(int line)
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}


const TextFormat & TextView::defaultFormat() const
{
  return d->defaultFormat;
}

void TextView::setDefaultFormat(const TextFormat & format)
{
  d->defaultFormat = format;
  update();
}


SyntaxHighlighter* TextView::syntaxHighlighter() const
{
  return d->syntaxHighlighter;
}

void TextView::setSyntaxHighlighter(SyntaxHighlighter *highlighter)
{
  d->syntaxHighlighter->deleteLater();
  d->syntaxHighlighter = highlighter;
}

void TextView::insertWidget(int line, int num, QWidget *w)
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}

const QMap<LineRange, QWidget*> & TextView::insertedWidgets() const
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}

void TextView::insertFloatingWidget(QWidget *widget, const QPoint & pos)
{
  /// TODO
  throw std::runtime_error{ "Not implemented" };
}

void TextView::onBlockDestroyed(int line, const TextBlock & block)
{
  d->blocks.removeAt(line);
  d->firstBlock.notifyBlockDestroyed(line);

  for (auto & fold : d->activeFolds)
  {
    /// TODO: check if fold is destroyed

    TextDocument::updatePositionOnBlockDestroyed(fold.begin, line, block);
    TextDocument::updatePositionOnBlockDestroyed(fold.end, line, block);
    fold.endline.notifyBlockDestroyed(line);
  }

  TextDocument::updatePositionOnBlockDestroyed(d->firstDirtyLine, line, block);
  d->firstDirtyLine.column = 0;

  d->vscrollbar->setMaximum(d->vscrollbar->maximum() - 1);
  updateLayout();

  if (line <= lastVisibleLine())
  {
    update();
  }
}

void TextView::onBlockInserted(const Position & pos, const TextBlock & block)
{
  d->blocks.insert(pos.line + 1, std::make_shared<view::BlockInfo>(block));
  d->firstBlock.notifyBlockInserted(pos);
  
  for (auto & fold : d->activeFolds)
  {
    /// TODO: check if fold is destroyed

    TextDocument::updatePositionOnInsert(fold.begin, pos, block);
    TextDocument::updatePositionOnInsert(fold.end, pos, block);
  }

  TextDocument::updatePositionOnInsert(d->firstDirtyLine, pos, block);
  d->firstDirtyLine.column = 0;

  d->vscrollbar->setMaximum(d->vscrollbar->maximum() + 1);
  updateLayout();

  if (pos.line <= lastVisibleLine())
  {
    update();
  }
}

void TextView::onContentsChange(const TextBlock & block, const Position & pos, int charsRemoved, int charsAdded)
{
  for (auto & fold : d->activeFolds)
  {
    /// TODO: check if fold is destroyed
    TextDocument::updatePositionOnContentsChange(fold.begin, block, pos, charsRemoved, charsAdded);
    TextDocument::updatePositionOnContentsChange(fold.end, block, pos, charsRemoved, charsAdded);
  }

  if (d->firstDirtyLine.line == -1 || d->firstDirtyLine.line > pos.line)
    d->firstDirtyLine = Position{ pos.line, 0 };

  if (d->longestLine == block && charsRemoved > charsAdded)
  {
    d->setLongestLine(d->findLongestLine());
    updateLayout();
  }
  else if (block.length() > d->longestLine.length())
  {
    d->setLongestLine(block);
    updateLayout();
  }

  if (pos.line >= firstVisibleLine() && pos.line <= lastVisibleLine())
  {
    update();
  }
}

void TextView::paintEvent(QPaintEvent *e)
{
  QPainter painter{ this };
  setupPainter(&painter);
  paint(&painter);
}

void TextView::resizeEvent(QResizeEvent *e)
{
  updateLayout();
}

void TextView::wheelEvent(QWheelEvent *e)
{
  scroll(-e->delta());
}

void TextView::setupPainter(QPainter *painter)
{
  painter->setFont(d->font);
  painter->setClipRect(d->viewport);
}

void TextView::paint(QPainter *painter)
{
  painter->setBrush(QBrush(d->defaultFormat.backgroundColor()));
  painter->setPen(Qt::NoPen);
  painter->drawRect(d->viewport);

  auto it = d->firstBlock;

  const int firstline = it.number();
  const int numline = visibleLineCount();

  for (int i(0); i < numline; ++i)
  {
    it.seek(firstline + i);

    if (it.needsRehighlight())
      it.rehighlight();

    const int baseline = i * d->metrics.lineheight + d->metrics.ascent;
    drawLine(painter, QPoint{ d->viewport.left() - d->hscrollbar->value(), baseline }, it);
  }
}

void TextView::drawLine(QPainter *painter, const QPoint & offset, view::Block line)
{
  QPoint pt = offset;

  const auto end = line.end();
  for (auto it = line.begin(); it != end; it = it.next())
  {
    drawFragment(painter, pt, it);
  }
}

void TextView::drawStrikeOut(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
{
  QPen pen{ fmt.strikeOutColor() };
  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  painter->setPen(pen);

  painter->drawLine(offset.x(), offset.y() - metrics().strikeoutpos, offset.x() + count * metrics().charwidth, offset.y() - metrics().strikeoutpos);
}

void TextView::drawUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
{
  if (fmt.underlineStyle() == TextFormat::NoUnderline)
  {
    return;
  }
  else if (fmt.underlineStyle() == TextFormat::WaveUnderline)
  {
    drawWaveUnderline(painter, offset, fmt, count);
  }
  else
  {
    QPen pen{ fmt.underlineColor() };

    const int penwidth = std::max(1, metrics().ascent / 10);
    pen.setWidth(penwidth);

    switch (fmt.underlineStyle())
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

static QPixmap generate_wave_pattern(const QPen & pen, const view::Metrics & metrics)
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

static void fill_with_pattern(QPainter *painter, const QRect & rect, const QPixmap & pattern)
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

void TextView::drawWaveUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
{
  /// TODO:
  QPen pen{ fmt.underlineColor() };

  const int penwidth = std::max(1, metrics().ascent / 10);
  pen.setWidth(penwidth);

  pen.setJoinStyle(Qt::RoundJoin);

  QPixmap pattern = generate_wave_pattern(pen, metrics());  
  QRect rect{ offset.x(), offset.y() + metrics().descent - pattern.height(), count * metrics().charwidth, pattern.height() };

  fill_with_pattern(painter, rect, pattern);
}

void TextView::drawFragment(QPainter *painter, QPoint & offset, view::Fragment fragment)
{
  const TextFormat & fmt = fragment.format();
  applyFormat(painter, fmt);

  QString text = fragment.text();
  painter->drawText(offset, text);

  if (fmt.strikeOut())
    drawStrikeOut(painter, offset, fmt, text.length());

  drawUnderline(painter, offset, fragment.format(), text.length());

  offset.rx() += text.length() * d->metrics.charwidth;
}

void TextView::applyFormat(QPainter *painter, const TextFormat & fmt)
{
  QFont f = painter->font();
  f.setBold(fmt.bold());
  f.setItalic(fmt.italic());
  painter->setFont(f);
  painter->setPen(QPen(fmt.textColor()));
  painter->setBrush(QBrush(fmt.backgroundColor()));
}

const view::Metrics & TextView::metrics() const
{
  return d->metrics;
}

void TextView::updateLayout()
{
  int available_width = this->width();
  int available_height = this->height();

  QRect rect_vscrollbar{ this->width(), 0, 0, 0 };
  bool vscrollbar_visible = true;
  if (d->vpolicy != Qt::ScrollBarAlwaysOff)
  {
    rect_vscrollbar = QRect(this->width() - d->vscrollbar->sizeHint().width(), 0, d->vscrollbar->sizeHint().width(), this->height());
    available_width -= rect_vscrollbar.width();
  }
  else
  {
    vscrollbar_visible = false;
  }

  QRect rect_gutter{ 0, 0, 0, 0 };
  if (d->gutter->isVisible())
  {
    rect_gutter = QRect(0, 0, d->gutter->sizeHint().width(), available_height);
    available_width -= d->gutter->sizeHint().width();
  }

  QRect rect_hscrollbar;
  bool hscrollbar_visible = true;
  if (d->hpolicy == Qt::ScrollBarAlwaysOn)
  {
    rect_hscrollbar = QRect(0, this->height() - d->hscrollbar->sizeHint().height(), available_width, d->hscrollbar->sizeHint().height());
    rect_vscrollbar.adjust(0, 0, 0, -rect_hscrollbar.height());
    rect_gutter.adjust(0, 0, 0, -rect_hscrollbar.height());
    available_height -= rect_hscrollbar.height();
  }
  else if (d->hpolicy == Qt::ScrollBarAsNeeded)
  {
    if (available_width > d->longestLine.length() * d->metrics.charwidth)
    {
      hscrollbar_visible = false;
    }
    else
    {
      rect_hscrollbar = QRect(0, this->height() - d->hscrollbar->sizeHint().height(), available_width, d->hscrollbar->sizeHint().height());
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
    d->vscrollbar->setVisible(true);
    d->vscrollbar->setGeometry(rect_vscrollbar);
  }
  else
  {
    d->vscrollbar->setVisible(false);
  }

  if (d->gutter->isVisible())
  {
    d->gutter->setGeometry(rect_gutter);
  }

  if (hscrollbar_visible)
  {
    d->hscrollbar->setVisible(true);
    d->hscrollbar->setGeometry(rect_hscrollbar);
  }
  else
  {
    d->hscrollbar->setVisible(false);
  }

  QRect vp = d->viewport;
  d->viewport = QRect(rect_gutter.right() + 1, 0, rect_vscrollbar.left() - rect_gutter.right() - 1, available_height);

  if (vp != d->viewport)
    update();
}

} // namespace textedit
