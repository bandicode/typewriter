// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_LINE_H
#define TEXTEDIT_VIEW_LINE_H

#include "textedit/textblock.h"
#include "textedit/view/formatrange.h"

#include <QLinkedList>
#include <QVector>

namespace textedit
{

class TextView;
class TextViewImpl;

namespace view
{

class Block;
class Fragment;

class TEXTEDIT_API LineElements
{
public:
  LineElements(TextViewImpl*view, int bn, int row);

  class Iterator
  {
  public:
    Iterator(TextViewImpl *view, int bn, int tb, int ei);
    Iterator(const Iterator &) = default;
    ~Iterator() = default;

    bool isFold() const;

    bool isBlockFragment() const;
    int block() const;
    int blockBegin() const;
    int blockEnd() const;

    Iterator & operator=(const Iterator &) = default;
    bool operator==(const Iterator & other) const;
    bool operator!=(const Iterator & other) const;

  private:
    TextViewImpl *mView;
    int mBlockNumber;
    int mTargetBlock;
    int mElementIndex;
  };

  int count() const;
  Iterator begin();
  Iterator end();

private:
  TextViewImpl *mView;
  int mBlockNumber;
  int mRow;
};

class TEXTEDIT_API Line
{
public:
  Line() = delete;
  Line(const Line & other) = default;
  ~Line() = default;
  
  inline int number() const { return mNumber; }
  inline int row() const { return mRow; }

  Line next() const;
  Line previous() const;
  void seekNext();
  void seekPrevious();
  void seek(int num);

  bool isFirst() const;
  bool isLast() const;

  inline bool isWidget() const { return mWidgetNumber >= 0; }
  QWidget* widget() const;
  int widgetSpan() const;

  bool isBlock() const;
  int blockNumber() const;
  view::Block block() const;

  bool isComplex() const;
  LineElements elements() const;

  Line & operator=(const Line & other) = default;

  inline bool operator==(const Line & other) const { return mNumber == other.mNumber; }
  inline bool operator!=(const Line & other) const { return mNumber != other.mNumber; }
  inline bool operator<(const Line & other) const { return mNumber < other.mNumber; }

protected:
  friend class Blocks;
  friend class Lines;
  friend class TextView;
  friend class TextViewImpl;

  Line(TextViewImpl *view);
  Line(int num, int blocknum, int widgetnum, int row, TextViewImpl *view);

  void notifyCharsAddedOrRemoved(const Position & pos, int added, int removed, int spanBefore);
  void notifyBlockDestroyed(int linenum, const int span);
  void notifyBlockInserted(const Position & pos, const int spanBefore);

private:
  TextViewImpl *mView;
  int mNumber;
  int mBlockNumber;
  int mWidgetNumber;
  int mRow;
};

class TEXTEDIT_API Lines
{
public:
  Lines(TextViewImpl *view, int block = -1);
  Lines(const Lines & other) = default;
  ~Lines() = default;

  int count() const;
  Line begin() const;
  Line end() const;
  Line at(int index) const;

  Lines & operator=(const Lines & other) = default;

private:
  TextViewImpl *mView;
  int mBlock; // -1 if no Block
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_BLOCK_H
