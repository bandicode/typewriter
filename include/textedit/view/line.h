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

class Fragment;

struct FoldPosition
{
  int pos;
  int kind;
};

struct TextLine
{
  TextBlock block;
  QVector<FormatRange> formats;
  int userstate;
  int revision;
  bool forceHighlighting;
  QVector<FoldPosition> folds;

public:
  TextLine(const TextBlock & b);
};

using LineList = QLinkedList<TextLine>;

struct ActiveFold;

class TEXTEDIT_API Line
{
public:
  Line();
  Line(const Line & other);
  ~Line();
  
  inline int number() const { return mNumber; }
  TextBlock block() const;
  inline const QString & text() const { return block().text(); }

  Line next() const;
  Line previous() const;
  void seekNext();
  void seekPrevious();
  void seek(int num);

  Line nextVisibleLine() const;

  bool isFirst() const;
  bool isLast() const;

  bool needsRehighlight() const;
  void rehighlight();
  void rehighlightLater();

  const QVector<FormatRange> & formats() const;

  const int userState() const;

  const QVector<FoldPosition> & foldPositions() const;
  bool hasActiveFold() const;
  std::pair<Position, Position> activeFold() const;

  /// TODO: for when we have word-wrap
  int span() const;
  QString displayedText() const;
  int columnWidth() const; // number of chars by line

  Fragment begin() const;
  Fragment end() const;

  TextLine & impl();

  Line & operator=(const Line & other) = default;

  bool operator==(const Line & other) const;
  bool operator!=(const Line & other) const;
  bool operator<(const Line & other) const;

protected:
  friend class TextView;
  friend class TextViewImpl;

  Line(TextViewImpl *view);
  Line(int num, TextViewImpl *view, LineList::iterator iter);

  void notifyBlockDestroyed(int linenum);
  void notifyBlockInserted(const Position & pos);

  ActiveFold getFold() const;

private:
  int mNumber;
  LineList::iterator mIterator;
  TextViewImpl *mView;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_LINE_H
