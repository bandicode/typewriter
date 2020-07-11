// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

//#include "textedit/textview.h"
//#include "textedit/private/textview_p.h"
//
//#include "textedit/view/fragment.h"
//#include "textedit/view/line.h"
//#include "textedit/view/lineelement.h"
//#include "textedit/private/gutter_p.h"
//#include "textedit/private/syntaxhighlighter_p.h"
//
//#include <QPainter>
//#include <QWheelEvent>
//
//#include <QDebug>
//
//namespace textedit
//{
//
//namespace view
//{
//
//BlockInfo::BlockInfo(const TextBlock & b)
//  : block(b)
//  , userstate(0)
//  , forceHighlighting(true)
//  , revision(-1)
//{
//
//}
//
//BlockInfo::~BlockInfo()
//{
//
//}
//
//Block::Block()
//  : mNumber(-1)
//  , mView(nullptr)
//{
//
//}
//
//Block::Block(const Block & other)
//  : mNumber(other.mNumber)
//  , mView(other.mView)
//{
//
//}
//
//Block::Block(TextViewImpl *view)
//  : mNumber(0)
//  , mView(view)
//{
//
//}
//
//Block::Block(int num, TextViewImpl *view)
//  : mNumber(num)
//  , mView(view)
//{
//
//}
//
//Block::~Block()
//{
//
//}
//
//
//TextBlock Block::block() const
//{
//  return impl().block;
//}
//
//Block Block::next() const
//{
//  Block ret = *this;
//  ret.seekNext();
//  return ret;
//}
//
//Block Block::previous() const
//{
//  Block ret = *this;
//  ret.seekPrevious();
//  return ret;
//}
//
//void Block::seekNext()
//{
//  TextBlock next = block().next();
//  Q_ASSERT(!next.isNull());
//  mNumber++;
//}
//
//void Block::seekPrevious()
//{
//  mNumber--;
//}
//
//void Block::seek(int num)
//{
//  if (num < mNumber)
//  {
//    while (num < mNumber)
//      seekPrevious();
//  }
//  else if (num > mNumber)
//  {
//    while (num > mNumber)
//      seekNext();
//  }
//}
//
//bool Block::isFirst() const
//{
//  return mNumber == 0;
//}
//
//bool Block::isLast() const
//{
//  return mNumber == block().document()->lineCount() - 1;
//}
//
//bool Block::needsRehighlight() const
//{
//  return impl().forceHighlighting || impl().revision != block().revision()
//    || mView->checkNeedsHighlighting(*this);
//}
//
//void Block::rehighlight()
//{
//  mView->highlightLine(*this);
//}
//
//void Block::rehighlightLater()
//{
//  impl().forceHighlighting = true;
//  if (mNumber < mView->firstDirtyBlock.line)
//    mView->firstDirtyBlock.line = mNumber;
//}
//
//const QVector<FormatRange> & Block::formats() const
//{
//  return impl().formats;
//}
//
//const int Block::userState() const
//{
//  return impl().userstate;
//}
//
//static int count_line_feed(const std::vector<std::unique_ptr<LineElement>> & elems)
//{
//  int n = 0;
//  for (size_t i(0); i < elems.size(); ++i)
//    n += elems.at(i)->isLineFeed() ? 1 : 0;
//  return n;
//}
//
//int Block::span() const
//{
//  return 1 + count_line_feed(impl().display);
//}
//
//QString Block::displayedText() const
//{
//  /// TODO: take folds into account ?
//  QString ret = block().text();
//  ret.replace("\t", mView->tabreplace);
//  return ret;
//}
//
//Fragment Block::begin() const
//{
//  return Fragment{ &(impl()), 0, formats().begin(), formats().end(), mView };
//}
//
//Fragment Block::end() const
//{
//  return Fragment{ &(impl()), block().length(), formats().end(), formats().end(), mView };
//}
//
//BlockInfo & Block::impl()
//{
//  return mView->blockInfo(mNumber);
//}
//
//const BlockInfo & Block::impl() const
//{
//  return mView->blockInfo(mNumber);
//}
//
//bool Block::operator==(const Block & other) const
//{
//  return mNumber == other.mNumber;
//}
//
//bool Block::operator!=(const Block & other) const
//{
//  return mNumber != other.mNumber;
//}
//
//bool Block::operator<(const Block & other) const
//{
//  return mNumber < other.mNumber;
//}
//
//Blocks::Blocks(TextViewImpl *view)
//  : mView(view)
//{
//
//}
//
//int Blocks::count() const
//{
//  return mView->blocks.size();
//}
//
//Block Blocks::begin() const
//{
//  return Block{ 0, mView };
//}
//
//Block Blocks::end() const
//{
//  return Block{ mView->blocks.size(), mView };
//}
//
//Block Blocks::at(int index) const
//{
//  return Block{ std::max(std::min(index, mView->blocks.size()), 0), mView };
//}
//
//Fragment::Fragment()
//{
//
//}
//
//Fragment::~Fragment()
//{
//
//}
//
//Fragment::Fragment(BlockInfo const *line, int col, QVector<FormatRange>::const_iterator iter, QVector<FormatRange>::const_iterator sentinel, TextViewImpl const *view)
//  : mLine(line)
//  , mColumn(col)
//  , mIterator(iter)
//  , mSentinel(sentinel)
//  , mView(view)
//{
//
//}
//
//const TextFormat & Fragment::format() const
//{
//  return (mIterator == mSentinel || mColumn < mIterator->start) ? mView->defaultFormat : mIterator->format;
//}
//
//int Fragment::position() const
//{
//  return mColumn;
//}
//
//int Fragment::length() const
//{
//  return mIterator == mSentinel ? (mLine->block.length() - mColumn) : (mColumn < mIterator->start ? mIterator->start - mColumn : mIterator->length);
//}
//
//TextBlock Fragment::block() const
//{
//  return mLine->block;
//}
//
//QString Fragment::text() const
//{
//  return mLine->block.text().mid(position(), length()).replace("\t", mView->tabreplace);
//}
//
//Fragment Fragment::next() const
//{
//  if (mIterator == mSentinel)
//    return Fragment{ mLine, mLine->block.length(), mSentinel, mSentinel, mView };
//  else if (mColumn < mIterator->start)
//    return Fragment{ mLine, mIterator->start, mIterator, mSentinel, mView };
//  else
//    return Fragment{ mLine, mColumn + mIterator->length, std::next(mIterator), mSentinel, mView };
//}
//
//bool Fragment::operator==(const Fragment & other) const
//{
//  return mLine == other.mLine && other.mColumn == mColumn;
//}
//
//bool Fragment::operator!=(const Fragment & other) const
//{
//  return mLine != other.mLine || other.mColumn != mColumn;
//}
//
//
//bool LineElement::isBlockFragment() const
//{
//  return false;
//}
//
//bool LineElement::isFold() const
//{
//  return false;
//}
//
//bool LineElement::isLineFeed() const
//{
//  return false;
//}
//
//const LineElement_Fold & LineElement::asFold() const
//{
//  return *static_cast<const LineElement_Fold*>(this);
//}
//
//const LineElement_BlockFragment & LineElement::asBlockFragment() const
//{
//  return *static_cast<const LineElement_BlockFragment*>(this);
//}
//
//LineElement_BlockFragment::LineElement_BlockFragment(int b, int e)
//  : begin(b), end(e)
//{
//
//}
//
//bool LineElement_BlockFragment::isBlockFragment() const
//{
//  return true;
//}
//
//LineElement_Fold::LineElement_Fold(int id)
//  : foldid(id)
//{
//
//}
//
//bool LineElement_Fold::isFold() const
//{
//  return true;
//}
//
//bool LineElement_LineFeed::isLineFeed() const
//{
//  return true;
//}
//
//
//LineElements::LineElements(TextViewImpl*view, int bn, int row)
//  : mView(view), mBlockNumber(bn), mRow(row)
//{
//
//}
//
//int LineElements::count() const
//{
//  const auto & dis = mView->blockInfo(mBlockNumber).display;
//  int count = 1;
//  for (const auto & e : dis)
//    count += e->isLineFeed() ? 1 : 0;
//  return count;
//}
//
//LineElements::Iterator LineElements::begin() const
//{
//  return Iterator{ mView, mBlockNumber, mBlockNumber, 0 };
//}
//
//LineElements::Iterator LineElements::end() const
//{
//  const auto & dis = mView->blockInfo(mBlockNumber).display;
//  for (int i = int(dis.size()) - 1; i >= 0; --i)
//  {
//    if (dis.at(i)->isFold())
//    {
//      const int fold_id = dis.at(i)->asFold().foldid;
//      const auto& fold_info = mView->folds.at(fold_id);
//      return Iterator{ mView, mBlockNumber, fold_info.end().line, (int)dis.size() };
//    }
//  }
//
//  return Iterator{ mView, mBlockNumber, mBlockNumber, (int)dis.size() };
//}
//
//LineElements::Iterator::Iterator(TextViewImpl *view, int bn, int tb, int ei)
//  : mView(view)
//  , mBlockNumber(bn)
//  , mTargetBlock(tb)
//  , mElementIndex(ei)
//{
//  
//}
//
//bool LineElements::Iterator::isFold() const
//{
//  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->isFold();
//}
//
//int LineElements::Iterator::foldid() const
//{
//  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->asFold().foldid;
//}
//
//bool LineElements::Iterator::isBlockFragment() const
//{
//  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->isBlockFragment();
//}
//
//int LineElements::Iterator::block() const
//{
//  return mTargetBlock;
//}
//
//int LineElements::Iterator::blockBegin() const
//{
//  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->asBlockFragment().begin;
//}
//
//int LineElements::Iterator::blockEnd() const
//{
//  return mView->blockInfo(mBlockNumber).display.at(mElementIndex)->asBlockFragment().end;
//}
//
//QStringRef LineElements::Iterator::text() const
//{
//  const QString & str = mView->blockInfo(mBlockNumber).block.text();
//  return str.midRef(blockBegin(), blockEnd() - blockBegin());
//}
//
//int LineElements::Iterator::colcount() const
//{
//  if (isFold())
//    return 3;
//  return text().length() + text().count('\t') * (mView->tabreplace.size() - 1);
//}
//
//LineElements::Iterator & LineElements::Iterator::operator++()
//{
//  if (isFold())
//    mTargetBlock = mView->folds.at(foldid()).end().line;
//
//  ++mElementIndex;
//
//  return *this;
//}
//
//bool LineElements::Iterator::operator==(const Iterator & other) const
//{
//  return mBlockNumber == other.mBlockNumber && mElementIndex == other.mElementIndex;
//}
//
//bool LineElements::Iterator::operator!=(const Iterator & other) const
//{
//  return mBlockNumber != other.mBlockNumber || mElementIndex != other.mElementIndex;
//}
//
//
//Line::Line(TextViewImpl *view)
//  : mNumber(0), mBlockNumber(0), mWidgetNumber(-1), mRow(0), mView(view)
//{
//
//}
//
//Line::Line(int num, int blocknum, int widgetnum, int row, TextViewImpl *view)
//  : mNumber(num), mBlockNumber(blocknum), mWidgetNumber(widgetnum), mRow(row), mView(view)
//{
//
//}
//
//Line Line::next() const
//{
//  if (isWidget())
//  {
//    if (row() + 1 < widgetSpan())
//      return Line{ number() + 1, mBlockNumber, mWidgetNumber, row() + 1, mView };
//
//    return Line{ number() + 1, mBlockNumber + 1, -1, 0, mView };
//  }
//  else
//  {
//    if (row() + 1 < block().span())
//      return Line{ number() + 1, mBlockNumber, -1, row() + 1, mView };
//    
//    /* Check for folds */
//    const view::BlockInfo & bi = mView->blockInfo(mBlockNumber);
//    for (int i = int(bi.display.size()) - 1; i >= 0; --i)
//    {
//      if (bi.display.at(i)->isFold())
//      {
//        const int fold_id = bi.display.at(i)->asFold().foldid;
//        const int block_num = mView->folds.at(fold_id).end().line + 1;
//        return Line{ number() + 1, block_num , -1, 0, mView };
//      }
//    }
//
//    /* Check for widgets */
//    for (int i(0); i < mView->widgets.size(); ++i)
//    {
//      if (mView->widgets.at(i).block == mBlockNumber)
//        return Line{ number() + 1, mBlockNumber, i, 0, mView };
//    }
//
//    return Line{ number() + 1, mBlockNumber + 1, -1, 0, mView };
//  }
//}
//
//static int prev_active_fold_index(TextViewImpl *view, int fold_id)
//{
//  for (int i(fold_id - 1); i >= 0; --i)
//  {
//    if (view->folds.at(i).isActive())
//      return i;
//  }
//
//  return -1;
//}
//
//static view::Block iterate_fold_backward(TextViewImpl *view, int fold_id)
//{
//  const auto & fold_info = view->folds.at(fold_id);
//
//  const int prev_active_fold = prev_active_fold_index(view, fold_id);
//
//  if (prev_active_fold == -1)
//    return view::Block{ fold_info.start().line, view };
//
//  const auto & prev_fold = view->folds.at(prev_active_fold);
//  if (prev_fold.end().line == fold_info.start().line)
//    return iterate_fold_backward(view, prev_active_fold);
//
//  return view::Block{ fold_info.start().line, view };
//}
//
//Line Line::previous() const
//{
//  if (mRow > 0)
//    return Line{ number() - 1, mBlockNumber, mWidgetNumber, row() - 1, mView };
//
//  Q_ASSERT(mRow == 0);
//
//  if (isWidget())
//  {
//    return Line{ number() - 1, mBlockNumber, -1, block().span() - 1, mView };
//  }
//  else
//  {
//    /* Check for folds */
//    for (int i = mView->folds.size() - 1; i >= 0; --i)
//    {
//      const auto & fold_info = mView->folds.at(i);
//      if (fold_info.isActive() && fold_info.end().line == mBlockNumber - 1)
//      {
//        const view::Block dest_block = iterate_fold_backward(mView, i);
//        return Line{ number() - 1, dest_block.number(), -1, dest_block.span() - 1, mView };
//      }
//    }
//
//    /* Check for widgets */
//    for (int i(0); i < mView->widgets.size(); ++i)
//    {
//      if (mView->widgets.at(i).block == mBlockNumber - 1)
//        return Line{ number() - 1, mBlockNumber - 1, i, mView->widgets.at(i).span - 1, mView };
//    }
//
//    return Line{ number() - 1, mBlockNumber - 1, -1, view::Block{mBlockNumber - 1, mView}.span() - 1, mView };
//  }
//}
//
//void Line::seekNext()
//{
//  *this = next();
//}
//
//void Line::seekPrevious()
//{
//  *this = previous();
//}
//
//void Line::seek(int num)
//{
//  num = std::max(0, std::min(num, mView->linecount - 1));
//
//  while (num < mNumber)
//    seekPrevious();
//  while (num > mNumber)
//    seekNext();
//}
//
//bool Line::isFirst() const
//{
//  return mNumber == 0;
//}
//
//bool Line::isLast() const
//{
//  return mNumber == mView->linecount - 1;
//}
//
//QWidget* Line::widget() const
//{
//  Q_ASSERT(isWidget());
//  return mView->widgets.at(mWidgetNumber).widget;
//}
//
//int Line::widgetSpan() const
//{
//  Q_ASSERT(isWidget());
//  return mView->widgets.at(mWidgetNumber).span;
//}
//
//bool Line::isBlock() const
//{
//  if (isWidget() || mRow != 0)
//    return false;
//
//  return mView->blockInfo(mBlockNumber).display.empty();
//}
//
//int Line::blockNumber() const
//{
//  return mBlockNumber;
//}
//
//view::Block Line::block() const
//{
//  return view::Block{ mBlockNumber, mView };
//}
//
//bool Line::isComplex() const
//{
//  return !isWidget() && !mView->blockInfo(mBlockNumber).display.empty();
//}
//
//LineElements Line::elements() const
//{
//  return LineElements(mView, mBlockNumber, mRow);
//}
//
//int Line::colcount() const
//{
//  if (isWidget())
//  {
//    return 0;
//  }
//  else if (isComplex())
//  {
//    int n = 0;
//    for (auto it = elements().begin(); it != elements().end(); ++it)
//      n += it.colcount();
//    return n;
//  }
//  else
//  {
//    return block().text().length() + block().text().count('\t') * (mView->tabreplace.size() - 1);
//  }
//}
//
//Line & Line::operator++()
//{
//  seekNext();
//  return *this;
//}
//
//void Line::notifyCharsAddedOrRemoved(const Position & pos, int added, int removed, int spanBefore)
//{
//  if (pos.line > mBlockNumber)
//    return;
//
//  if (mBlockNumber > pos.line)
//  {
//    mNumber += view::Block{ mBlockNumber, mView }.span() - spanBefore;
//  }
//  else
//  {
//    Q_ASSERT(mBlockNumber == pos.line);
//
//    const int span = view::Block{ pos.line, mView }.span();
//
//    if (isWidget())
//    {
//      const int diff = span - spanBefore;
//      mNumber += diff;
//    }
//    else
//    {
//      if (mRow >= span)
//      {
//        const int diff = 1 + mRow - span;
//        mNumber -= diff;
//        mRow -= span;
//      }
//    }
//  }
//}
//
//void Line::notifyBlockDestroyed(int linenum, const int destroyed_span, const int prev_old_span, const int prev_new_span)
//{
//  if (mBlockNumber < linenum)
//    return;
//
//  if (mBlockNumber > linenum)
//  {
//    mBlockNumber -= 1;
//    mNumber -= destroyed_span;
//    mNumber += prev_new_span - prev_old_span;
//  }
//  else
//  {
//    Q_ASSERT(mBlockNumber == linenum);
//
//    mBlockNumber -= 1;
//    mNumber -= destroyed_span;
//    mNumber += prev_new_span - prev_old_span;
//
//    if (!isWidget())
//      mRow = view::Block{ mBlockNumber, mView }.span() - 1;
//  }
//}
//
//void Line::notifyBlockInserted(const Position & pos, const int spanBefore)
//{
//  if (mBlockNumber < pos.line)
//    return;
//
//  if (mBlockNumber > pos.line)
//  {
//    mBlockNumber += 1;
//
//    const int span_diff = spanBefore - view::Block{ pos.line, mView }.span();
//    Q_ASSERT(span_diff >= 0);
//    mNumber += span_diff + view::Block{ pos.line + 1, mView }.span();
//  }
//  else
//  {
//    Q_ASSERT(mBlockNumber == pos.line);
//
//    // Things get complicated here...
//    const int span = view::Block{ pos.line, mView }.span();
//
//    if (isWidget())
//    {
//      const int diff = spanBefore - span;
//      Q_ASSERT(diff >= 0);
//      mNumber -= diff;
//    }
//    else
//    {
//      if (mRow >= span)
//      {
//        const int diff = 1 + mRow - span;
//        mNumber -= diff;
//        mRow -= span;
//      }
//    }
//  }
//}
//
//Lines::Lines(TextViewImpl *view, int block)
//  : mView(view), mBlock(block)
//{
//
//}
//
//int Lines::count() const
//{
//  if (mBlock == AllLines)
//  {
//    return mView->linecount;
//  }
//  else if (mBlock == VisibleLines)
//  {
//    int howmany = mView->viewport.height() / mView->metrics.lineheight;
//    if (mView->metrics.lineheight * howmany < mView->viewport.height())
//      howmany++;
//    auto line = mView->firstLine;
//    howmany = std::min(howmany, mView->linecount - line.number());
//    return howmany;
//  }
//
//  return view::Block{ mBlock, mView }.span();
//}
//
//Line Lines::begin() const
//{
//  if (mBlock == VisibleLines)
//    return mView->firstLine;
//
//  return Line{ 0, std::max(0, mBlock), -1, 0, mView };
//}
//
//Line Lines::end() const
//{
//  if (mBlock == AllLines)
//  {
//    const auto last_block = view::Block{ mView->blocks.size() - 1, mView };
//
//    /* Check for widgets */
//    if(!mView->widgets.empty())
//    {
//      if(mView->widgets.back().block == last_block.number())
//        return Line{ mView->linecount - 1, last_block.number(), mView->widgets.size() - 1, mView->widgets.last().span - 1, mView };
//    }
//
//    /* Check for folds */
//    if (mView->folds.activeCount() > 0)
//    {
//      if (mView->folds.activeBack().end().line == last_block.number())
//      {
//        auto block = iterate_fold_backward(mView, mView->folds.lastActiveIndex());
//        return Line{ mView->linecount - 1, block.number(), -1, block.span() - 1, mView };
//      }
//    }
//
//    return Line{ mView->linecount - 1, last_block.number(), -1, last_block.span() - 1, mView };
//  }
//  else if (mBlock == VisibleLines)
//  {
//    auto line = mView->firstLine;
//    line.seek(line.number() + count());
//    return line;
//  }
//  else
//  {
//    auto b = view::Block{ mBlock, mView };
//    return Line{ b.span() - 1, b.number(), -1, b.span() - 1, mView };
//  }
//}
//
//Line Lines::at(int index) const
//{
//  auto result = begin();
//  result.seek(index);
//  return result;
//}
//
//} // namespace view
//
//
//TextViewImpl::TextViewImpl(TextDocument *doc)
//  : document(doc)
//  , hpolicy(Qt::ScrollBarAsNeeded)
//  , vpolicy(Qt::ScrollBarAlwaysOn)
//  , firstDirtyBlock{-1, 0}
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
//void TextViewImpl::calculateMetrics(const QFont & f)
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
//void TextViewImpl::setLongestLine(const view::Line & line)
//{
//  this->longestLine = line;
//  this->hscrollbar->setRange(0, line.colcount() * this->metrics.charwidth);
//}
//
//int TextViewImpl::getFold(int blocknum, int from) const
//{
//  for (int i(from); i < folds.size(); ++i)
//  {
//    const auto & fi = folds.at(i);
//    if (fi.start().line > blocknum)
//      return -1;
//    else if (fi.start().line == blocknum)
//      return i;
//  }
//  return -1;
//}
//
//void TextViewImpl::addFold(const TextFold & f)
//{
//  this->folds.insert(f);
//
//  for (const auto & fold : this->folds)
//  {
//    if(fold.isActive())
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
//  for (const auto & fold : this->folds)
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
//  for (const auto & fold : this->folds)
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
//
//TextView::TextView(TextDocument *document)
//  : d(new TextViewImpl(document))
//{
//  init();
//}
//
//TextView::TextView(std::unique_ptr<TextViewImpl> && impl)
//  : d(std::move(impl))
//{
//  init();
//}
//
//TextView::~TextView()
//{
//
//}
//
//void TextView::init()
//{
//  connect(document(), &TextDocument::blockDestroyed, this, &TextView::onBlockDestroyed);
//  connect(document(), &TextDocument::blockInserted, this, &TextView::onBlockInserted);
//  connect(document(), &TextDocument::contentsChange, this, &TextView::onContentsChange);
//
//  d->hscrollbar = new QScrollBar(Qt::Horizontal, this);
//  connect(d->hscrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));
//
//  d->vscrollbar = new QScrollBar(Qt::Vertical, this);
//  d->vscrollbar->setRange(0, document()->lineCount() - 1);
//  d->vscrollbar->setValue(0);
//  connect(d->vscrollbar, &QScrollBar::valueChanged, this, &TextView::setFirstVisibleLine);
//
//  d->gutter = new Gutter{ this };
//  d->gutter->impl()->view = this;
//
//  d->setLongestLine(d->findLongestLine());
//
//  d->syntaxHighlighter = new SyntaxHighlighter(this);
//
//  {
//    QFont font{ "Courier" };
//    font.setKerning(false);
//    font.setStyleHint(QFont::TypeWriter);
//    font.setFixedPitch(true);
//    font.setPixelSize(40);
//    setFont(font);
//  }
//}
//
//TextDocument* TextView::document() const
//{
//  return d->document;
//}
//
//view::Blocks TextView::blocks() const
//{
//  return view::Blocks{ this->d.get() };
//}
//
//view::Lines TextView::lines() const
//{
//  return view::Lines{ d.get() };
//}
//
//view::Lines TextView::visibleLines() const
//{
//  return view::Lines{ d.get(), view::Lines::VisibleLines };
//}
//
//void TextView::scroll(int delta)
//{
//  if (delta == 0)
//    return;
//
//  while (delta > 0 && d->firstLine.number() < document()->lineCount() - 1)
//  {
//    d->firstLine.seekNext();
//    delta -= 1;
//  }
//
//  while (delta < 0 && d->firstLine.number() > 0)
//  {
//    d->firstLine.seekPrevious();
//    delta += 1;
//  }
//
//  d->vscrollbar->setValue(d->firstLine.number());
//
//  update();
//}
//
//void TextView::setFirstVisibleLine(int n)
//{
//  if (d->firstLine.number() == n)
//    return;
//
//  scroll(n - visibleLines().begin().number());
//}
//
//void TextView::setFont(const QFont & f)
//{
//  /// TODO: check typewriter 
//  d->font = f;
//  d->calculateMetrics(d->font);
//
//  update();
//}
//
//const QFont & TextView::font() const
//{
//  return d->font;
//}
//
//
//void TextView::setTabSize(int n)
//{
//  if (tabSize() == n)
//    return;
//
//  d->tabreplace = QString(" ").repeated(std::max(n, 0));
//  update();
//}
//
//int TextView::tabSize() const
//{
//  return d->tabreplace.size();
//}
//
//QScrollBar* TextView::horizontalScrollBar() const
//{
//  return d->hscrollbar;
//}
//
//void TextView::setHorizontalScrollBar(QScrollBar *scrollbar)
//{
//  int hscroll = d->hscrollbar->value();
//
//  d->hscrollbar->deleteLater();
//  d->hscrollbar = scrollbar;
//
//  d->hscrollbar->setRange(0, d->longestLine.colcount() * d->metrics.charwidth);
//  d->hscrollbar->setValue(hscroll);
//
//  connect(d->hscrollbar, SIGNAL(valueChanged(int)), this, SLOT(update()));
//
//  updateLayout();
//}
//
//Qt::ScrollBarPolicy TextView::horizontalScrollBarPolicy() const
//{
//  return d->hpolicy;
//}
//
//void TextView::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
//{
//  if (d->hpolicy == policy)
//    return;
//
//  d->hpolicy = policy;
//  updateLayout();
//}
//
//
//QScrollBar* TextView::verticalScrollBar() const
//{
//  return d->vscrollbar;
//}
//
//void TextView::setVerticalScrollBar(QScrollBar *scrollbar)
//{
//  d->vscrollbar->deleteLater();
//  d->vscrollbar = scrollbar;
//  d->vscrollbar->setRange(0, document()->lineCount() - 1);
//  d->vscrollbar->setValue(d->firstLine.number());
//  connect(d->vscrollbar, &QScrollBar::valueChanged, this, &TextView::setFirstVisibleLine);
//  updateLayout();
//}
//
//Qt::ScrollBarPolicy TextView::verticalScrollBarPolicy() const
//{
//  return d->vpolicy;
//}
//
//void TextView::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
//{
//  if (d->vpolicy == policy)
//    return;
//
//  d->vpolicy = policy;
//  updateLayout();
//}
//
//int TextView::hscroll() const
//{
//  return d->hscrollbar->value();
//}
//
//const QRect & TextView::viewport() const
//{
//  return d->viewport;
//}
//
//Position TextView::hitTest(const QPoint & pos) const
//{
//  const int line_offset = (pos.y() - viewport().top()) / d->metrics.lineheight;
//  const int column_offset = std::round((pos.x() + hscroll() - viewport().left()) / float(d->metrics.charwidth));
//
//  /* Seek visible line */
//  auto it = d->firstLine;
//  for (int i(0); i < line_offset; ++i)
//  {
//    if (it.isLast())
//      break;
//
//    it = it.next();
//  }
//
//  /* Take into account tabulations & folds */
//  if (it.isComplex())
//  {
//    int target_block = 0;
//    int target_col = 0;
//    int counter = column_offset;
//
//    for (auto elem = it.elements().begin(); elem != it.elements().end(); ++elem)
//    {
//      if (elem.isFold())
//      {
//        counter -= 3;
//        if (counter <= 0)
//          return Position{ target_block, target_col };
//      }
//      else
//      {
//        target_block = elem.block();
//        target_col = elem.blockBegin();
//        const int count = elem.blockEnd() - elem.blockBegin();
//        for (int i(0); i < count; ++i)
//        {
//          if (elem.text().at(i) == QChar('\t'))
//          {
//            counter -= tabSize();
//            target_col += counter <= -tabSize() / 2 ? 0 : 1;
//          }
//          else
//          {
//            counter -= 1;
//            target_col += 1;
//          }
//
//          if (counter <= 0)
//            return Position{ target_block, target_col };
//        }
//      }
//    }
//
//    return Position{ target_block, target_col };
//  }
//  else
//  {
//    const QString & text = it.block().text();
//    int col = 0;
//    for (int i(0), counter(column_offset); i < text.length() && counter > 0; ++i)
//    {
//      if (text.at(i) == QChar('\t'))
//      {
//        counter -= tabSize();
//        col += counter <= -tabSize() / 2 ? 0 : 1;
//      }
//      else
//      {
//        counter -= 1;
//        col += 1;
//      }
//    }
//
//    return Position{ it.number(), col };
//  }
//}
//
//static bool map_pos_complex(TextViewImpl const *view, const Position & pos, view::Line line, int & column_offset)
//{
//  for (auto elem = line.elements().begin(); elem != line.elements().end(); ++elem)
//  {
//    if (elem.isBlockFragment())
//    {
//      if (elem.block() != pos.line)
//      {
//        if (elem.blockBegin() <= pos.column && pos.column <= elem.blockEnd())
//        {
//          int count = pos.column - elem.blockBegin();
//          for (int i(0); i < count; ++i)
//            column_offset += view->ncol(elem.text().at(i));
//          return true;
//        }
//      }
//      else
//      {
//        column_offset += elem.colcount();
//      }
//    }
//    else
//    {
//      const auto & fold_info = view->folds.at(elem.foldid());
//      if (fold_info.start() < pos && pos < fold_info.end())
//        return true;
//      column_offset += elem.colcount();
//    }
//  }
//
//  return false;
//}
//
//QPoint TextView::mapToViewport(const Position & pos) const
//{
//  if (pos.line < visibleLines().begin().blockNumber())
//    return QPoint{ 0, -d->metrics.descent };
//  
//  int line_offset = 0;
//  int column_offset = 0;
//
//  for (auto line = visibleLines().begin(); line_offset < visibleLines().count(); ++line, ++line_offset)
//  {
//    if (line.isWidget())
//      continue;
//
//    if (line.isComplex())
//    {
//      if (map_pos_complex(d.get(), pos, line, column_offset))
//        break;
//    }
//    else
//    {
//      if (line.blockNumber() == pos.line)
//      {
//        column_offset = pos.column;
//        const int increment = line.block().text().leftRef(pos.column).count(QChar('\t')) * (tabSize() - 1);
//        column_offset += increment;
//        break;
//      }
//    }
//  }
//
//  int dy = line_offset * d->metrics.lineheight + d->metrics.ascent;
//  int dx = column_offset * metrics().charwidth;
//
//  return QPoint{ dx - hscroll(), dy };
//}
//
//QPoint TextView::map(const Position & pos) const
//{
//  return mapToViewport(pos) + viewport().topLeft();
//}
//
//bool TextView::isVisible(const Position &pos) const
//{
//  return viewport().contains(map(pos));
//}
//
//void TextView::fold(int line)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//void TextView::unfold(int line)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//const TextFoldList& TextView::folds() const
//{
//  return d->folds;
//}
//
//bool TextView::hasActiveFolds() const
//{
//  return d->folds.lastActiveIndex() != -1;
//}
//
//const TextFormat & TextView::defaultFormat() const
//{
//  return d->defaultFormat;
//}
//
//void TextView::setDefaultFormat(const TextFormat & format)
//{
//  d->defaultFormat = format;
//  update();
//}
//
//
//SyntaxHighlighter* TextView::syntaxHighlighter() const
//{
//  return d->syntaxHighlighter;
//}
//
//void TextView::setSyntaxHighlighter(SyntaxHighlighter *highlighter)
//{
//  d->syntaxHighlighter->deleteLater();
//  d->syntaxHighlighter = highlighter;
//  d->syntaxHighlighter->impl()->view = this;
//}
//
//void TextView::insertWidget(int line, int num, QWidget *w)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//const QMap<LineRange, QWidget*> & TextView::insertedWidgets() const
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//void TextView::insertFloatingWidget(QWidget *widget, const QPoint & pos)
//{
//  /// TODO
//  throw std::runtime_error{ "Not implemented" };
//}
//
//void TextView::onBlockDestroyed(int line, const TextBlock & block)
//{
//  //  process destroyed block
//  const int destroyed_span = view::Block{ line, d.get() }.span();
//  d->blocks.removeAt(line);
//  d->linecount -= destroyed_span;
//  const int prev_old_span = view::Block{ line - 1, d.get() }.span();
//  if(block.length() > 0)
//    d->relayout(line - 1);
//  const int prev_new_span = block.length() > 0 ? view::Block{ line - 1, d.get() }.span() : prev_old_span;
//  d->linecount += prev_new_span - prev_old_span;
//
//  d->firstLine.notifyBlockDestroyed(line, destroyed_span, prev_old_span, prev_new_span);
//
//  if (d->longestLine.blockNumber() == line)
//    d->setLongestLine(d->findLongestLine());
//  else
//    d->longestLine.notifyBlockDestroyed(line, destroyed_span, prev_old_span, prev_new_span);
//
//  TextDocument::updatePositionOnBlockDestroyed(d->firstDirtyBlock, line, block);
//  d->firstDirtyBlock.column = 0;
//
//  d->vscrollbar->setMaximum(d->vscrollbar->maximum() - 1);
//  updateLayout();
//
//  const int first_line = visibleLines().begin().number();
//  const int last_line = first_line + visibleLines().count();
//  if (line <= last_line)
//  {
//    update();
//  }
//}
//
//void TextView::onBlockInserted(const Position & pos, const TextBlock & block)
//{
//  const int span = view::Block{ pos.line, d.get() }.span();
//  d->blocks.insert(pos.line + 1, std::make_shared<view::BlockInfo>(block));
//
//  if (block.length() > 0)
//  {
//    d->relayout(pos.line);
//    d->relayout(pos.line + 1);
//    d->linecount += view::Block{ pos.line + 1, d.get() }.span() + view::Block{ pos.line, d.get() }.span() - span;
//  }
//  else
//  {
//    d->linecount += 1;
//  }
//
//  d->firstLine.notifyBlockInserted(pos, span);
//
//  TextDocument::updatePositionOnInsert(d->firstDirtyBlock, pos, block);
//  d->firstDirtyBlock.column = 0;
//
//  d->vscrollbar->setMaximum(d->vscrollbar->maximum() + 1);
//  updateLayout();
//
//  const int first_line = visibleLines().begin().number();
//  const int last_line = first_line + visibleLines().count();
//  if (pos.line <= last_line)
//  {
//    update();
//  }
//}
//
//void TextView::onContentsChange(const TextBlock & block, const Position & pos, int charsRemoved, int charsAdded)
//{
//  const int span = view::Block{ pos.line, d.get() }.span();
//  d->relayout(pos.line);
//  d->linecount += view::Block{ pos.line, d.get() }.span() - span;
//
//  d->firstLine.notifyCharsAddedOrRemoved(pos, charsAdded, charsRemoved, span);
//
//  if (d->firstDirtyBlock.line == -1 || d->firstDirtyBlock.line > pos.line)
//    d->firstDirtyBlock = Position{ pos.line, 0 };
//
//  if (d->longestLine.blockNumber() == pos.line && charsRemoved > charsAdded)
//  {
//    d->setLongestLine(d->findLongestLine());
//    updateLayout();
//  }
//  else if (view::Line{ 0, pos.line, -1, 0, d.get() }.colcount() > d->longestLine.colcount())
//  {
//    d->setLongestLine(view::Line{ 0, pos.line, -1, 0, d.get() });
//    updateLayout();
//  }
//  else
//  {
//    d->longestLine.notifyCharsAddedOrRemoved(pos, charsAdded, charsRemoved, span);
//  }
//
//  const int first_line = visibleLines().begin().number();
//  const int last_line = first_line + visibleLines().count();
//  if (pos.line >= first_line && pos.line <= last_line)
//  {
//    update();
//  }
//}
//
//void TextView::paintEvent(QPaintEvent *e)
//{
//  QPainter painter{ this };
//  setupPainter(&painter);
//  paint(&painter);
//}
//
//void TextView::resizeEvent(QResizeEvent *e)
//{
//  updateLayout();
//}
//
//void TextView::wheelEvent(QWheelEvent *e)
//{
//  scroll(-3 * e->delta() / (15 * 8));
//}
//
//void TextView::setupPainter(QPainter *painter)
//{
//  painter->setFont(d->font);
//  painter->setClipRect(d->viewport);
//}
//
//void TextView::paint(QPainter *painter)
//{
//  painter->setBrush(QBrush(d->defaultFormat.backgroundColor()));
//  painter->setPen(Qt::NoPen);
//  painter->drawRect(d->viewport);
//
//  auto it = d->firstLine;
//
//  const int firstline = it.number();
//  const int numline = visibleLines().count();
//
//  for (int i(0); i < numline; ++i)
//  {
//    it.seek(firstline + i);
//
//    if (it.block().needsRehighlight())
//      it.block().rehighlight();
//
//    const int baseline = i * d->metrics.lineheight + d->metrics.ascent;
//    drawLine(painter, QPoint{ d->viewport.left() - d->hscrollbar->value(), baseline }, it);
//  }
//}
//
//void TextView::drawLine(QPainter *painter, const QPoint & offset, view::Line line)
//{
//  if (line.isWidget())
//    return;
//  else if (!line.isComplex())
//    return drawBlock(painter, offset, line.block());
//  else
//    return drawLineElements(painter, offset, line.elements());
//}
//
//void TextView::drawBlock(QPainter *painter, const QPoint & offset, view::Block block)
//{
//  QPoint pt = offset;
//
//  const auto end = block.end();
//  for (auto it = block.begin(); it != end; it = it.next())
//  {
//    drawFragment(painter, pt, it);
//  }
//}
//
//void TextView::drawLineElements(QPainter *painter, const QPoint & offset, view::LineElements elements)
//{
//  QPoint pt = offset;
//
//  const auto end = elements.end();
//  for (auto it = elements.begin(); it != end; ++it)
//  {
//    if (it.isFold())
//      drawFoldSymbol(painter, pt, it.foldid());
//    else
//      drawBlockFragment(painter, pt, it.block(), it.blockBegin(), it.blockEnd());
//  }
//}
//
//void TextView::drawFoldSymbol(QPainter *painter, const QPoint & offset, int foldid)
//{
//  /// TODO: 
//  throw std::runtime_error{ "Not implemented" };
//}
//
//void TextView::drawBlockFragment(QPainter *painter, QPoint & offset, int blocknum, int begin, int end)
//{
//  view::Block block{ blocknum, d.get() };
//
//  view::Fragment frag = block.begin();
//  while (!(frag.position() <= begin && begin < frag.position() + frag.length()))
//    frag = frag.next();
//
//  int range_begin = begin;
//  int range_end = std::min(end, frag.position() + frag.length());
//
//  do
//  {
//    drawText(painter, offset, replaceTabs(block.text().mid(range_begin, range_end - range_begin)), frag.format());
//
//    range_begin = range_end;
//    frag = frag.next();
//    range_end = std::min(end, frag.position() + frag.length());
//  } while (range_begin < end);
//}
//
//void TextView::drawStrikeOut(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
//{
//  QPen pen{ fmt.strikeOutColor() };
//  const int penwidth = std::max(1, metrics().ascent / 10);
//  pen.setWidth(penwidth);
//
//  painter->setPen(pen);
//
//  painter->drawLine(offset.x(), offset.y() - metrics().strikeoutpos, offset.x() + count * metrics().charwidth, offset.y() - metrics().strikeoutpos);
//}
//
//void TextView::drawUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
//{
//  if (fmt.underlineStyle() == TextFormat::NoUnderline)
//  {
//    return;
//  }
//  else if (fmt.underlineStyle() == TextFormat::WaveUnderline)
//  {
//    drawWaveUnderline(painter, offset, fmt, count);
//  }
//  else
//  {
//    QPen pen{ fmt.underlineColor() };
//
//    const int penwidth = std::max(1, metrics().ascent / 10);
//    pen.setWidth(penwidth);
//
//    switch (fmt.underlineStyle())
//    {
//    case TextFormat::SingleUnderline:
//      pen.setStyle(Qt::SolidLine);
//      break;
//    case TextFormat::DashUnderline:
//      pen.setStyle(Qt::DashLine);
//      break;
//    case TextFormat::DotLine:
//      pen.setStyle(Qt::DotLine);
//      break;
//    case TextFormat::DashDotLine:
//      pen.setStyle(Qt::DashDotLine);
//      break;
//    case TextFormat::DashDotDotLine:
//      pen.setStyle(Qt::DashDotDotLine);
//      break;
//    default:
//      break;
//    }
//
//    painter->setPen(pen);
//    //painter->drawLine(offset.x(), offset.y() + metrics().underlinepos, offset.x() + count * metrics().charwidth, offset.y() + metrics().underlinepos);
//    painter->drawLine(offset.x(), offset.y() + metrics().descent - penwidth - 1, offset.x() + count * metrics().charwidth, offset.y() + metrics().descent - penwidth - 1);
//  }
//}
//
//static QPixmap generate_wave_pattern(const QPen & pen, const view::Metrics & metrics)
//{
//  /// TODO: add cache
//
//  const int pattern_height = metrics.descent * 2 / 3;
//  const int pattern_width = metrics.charwidth * 13 / 16;
//
//  QPixmap img{ pattern_width, pattern_height };
//  img.fill(QColor{ 255, 255, 255, 0 });
//
//  QPainterPath path;
//  path.moveTo(0, img.height() - 1);
//  path.lineTo(pattern_width / 2.f, 0.5);
//  path.lineTo(pattern_width, img.height() - 1);
//
//  QPainter painter{ &img };
//  painter.setPen(pen);
//  painter.setRenderHint(QPainter::Antialiasing);
//  painter.drawPath(path);
//
//  return img;
//}
//
//static void fill_with_pattern(QPainter *painter, const QRect & rect, const QPixmap & pattern)
//{
//  const int y = rect.top();
//  
//  int x = rect.left();
//
//  const int full = rect.width() / pattern.width();
//  for (int i(0); i < full; ++i)
//  {
//    painter->drawPixmap(x, y, pattern);
//    x += pattern.width();
//  }
//
//  const int partial_width = rect.width() - pattern.width() * full;
//
//  if (partial_width > 0)
//    painter->drawPixmap(QPoint(x, y), pattern, QRect(0, 0, partial_width, pattern.height()));
//}
//
//void TextView::drawWaveUnderline(QPainter *painter, const QPoint & offset, const TextFormat & fmt, int count)
//{
//  /// TODO:
//  QPen pen{ fmt.underlineColor() };
//
//  const int penwidth = std::max(1, metrics().ascent / 10);
//  pen.setWidth(penwidth);
//
//  pen.setJoinStyle(Qt::RoundJoin);
//
//  QPixmap pattern = generate_wave_pattern(pen, metrics());  
//  QRect rect{ offset.x(), offset.y() + metrics().descent - pattern.height(), count * metrics().charwidth, pattern.height() };
//
//  fill_with_pattern(painter, rect, pattern);
//}
//
//void TextView::drawFragment(QPainter *painter, QPoint & offset, view::Fragment fragment)
//{
//  const TextFormat & fmt = fragment.format();
//  applyFormat(painter, fmt);
//
//  QString text = fragment.text();
//  painter->drawText(offset, text);
//
//  if (fmt.strikeOut())
//    drawStrikeOut(painter, offset, fmt, text.length());
//
//  drawUnderline(painter, offset, fragment.format(), text.length());
//
//  offset.rx() += text.length() * d->metrics.charwidth;
//}
//
//void TextView::drawText(QPainter *painter, QPoint & offset, const QString & text, const TextFormat & format)
//{
//  applyFormat(painter, format);
//
//  painter->drawText(offset, text);
//
//  if (format.strikeOut())
//    drawStrikeOut(painter, offset, format, text.length());
//
//  drawUnderline(painter, offset, format, text.length());
//
//  offset.rx() += text.length() * d->metrics.charwidth;
//}
//
//void TextView::applyFormat(QPainter *painter, const TextFormat & fmt)
//{
//  QFont f = painter->font();
//  f.setBold(fmt.bold());
//  f.setItalic(fmt.italic());
//  painter->setFont(f);
//  painter->setPen(QPen(fmt.textColor()));
//  painter->setBrush(QBrush(fmt.backgroundColor()));
//}
//
//QString TextView::replaceTabs(QString text) const
//{
//  return text.replace(QChar('\t'), d->tabreplace);
//}
//
//const view::Metrics & TextView::metrics() const
//{
//  return d->metrics;
//}
//
//void TextView::updateLayout()
//{
//  int available_width = this->width();
//  int available_height = this->height();
//
//  QRect rect_vscrollbar{ this->width(), 0, 0, 0 };
//  bool vscrollbar_visible = true;
//  if (d->vpolicy != Qt::ScrollBarAlwaysOff)
//  {
//    rect_vscrollbar = QRect(this->width() - d->vscrollbar->sizeHint().width(), 0, d->vscrollbar->sizeHint().width(), this->height());
//    available_width -= rect_vscrollbar.width();
//  }
//  else
//  {
//    vscrollbar_visible = false;
//  }
//
//  QRect rect_gutter{ 0, 0, 0, 0 };
//  if (d->gutter->isVisible())
//  {
//    rect_gutter = QRect(0, 0, d->gutter->sizeHint().width(), available_height);
//    available_width -= d->gutter->sizeHint().width();
//  }
//
//  QRect rect_hscrollbar;
//  bool hscrollbar_visible = true;
//  if (d->hpolicy == Qt::ScrollBarAlwaysOn)
//  {
//    rect_hscrollbar = QRect(0, this->height() - d->hscrollbar->sizeHint().height(), available_width, d->hscrollbar->sizeHint().height());
//    rect_vscrollbar.adjust(0, 0, 0, -rect_hscrollbar.height());
//    rect_gutter.adjust(0, 0, 0, -rect_hscrollbar.height());
//    available_height -= rect_hscrollbar.height();
//  }
//  else if (d->hpolicy == Qt::ScrollBarAsNeeded)
//  {
//    if (available_width > d->longestLine.colcount() * d->metrics.charwidth)
//    {
//      hscrollbar_visible = false;
//    }
//    else
//    {
//      rect_hscrollbar = QRect(0, this->height() - d->hscrollbar->sizeHint().height(), available_width, d->hscrollbar->sizeHint().height());
//      rect_vscrollbar.adjust(0, 0, 0, -rect_hscrollbar.height());
//      rect_gutter.adjust(0, 0, 0, -rect_hscrollbar.height());
//    }
//  }
//  else
//  {
//    hscrollbar_visible = false;
//  }
//
//  if (vscrollbar_visible)
//  {
//    d->vscrollbar->setVisible(true);
//    d->vscrollbar->setGeometry(rect_vscrollbar);
//  }
//  else
//  {
//    d->vscrollbar->setVisible(false);
//  }
//
//  if (d->gutter->isVisible())
//  {
//    d->gutter->setGeometry(rect_gutter);
//  }
//
//  if (hscrollbar_visible)
//  {
//    d->hscrollbar->setVisible(true);
//    d->hscrollbar->setGeometry(rect_hscrollbar);
//  }
//  else
//  {
//    d->hscrollbar->setVisible(false);
//  }
//
//  QRect vp = d->viewport;
//  d->viewport = QRect(rect_gutter.right() + 1, 0, rect_vscrollbar.left() - rect_gutter.right() - 1, available_height);
//
//  if (vp != d->viewport)
//    update();
//}
//
//} // namespace textedit
