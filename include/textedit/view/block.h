// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_BLOCK_H
#define TEXTEDIT_VIEW_BLOCK_H

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
class LineElement;

struct FoldPosition
{
  int pos;
  int kind;
};

struct BlockInfo
{
  TextBlock block;
  QVector<FormatRange> formats;
  int userstate;
  int revision;
  bool forceHighlighting;
  QVector<FoldPosition> folds;
  std::vector<std::unique_ptr<LineElement>> display;

public:
  BlockInfo(const TextBlock & b);
  BlockInfo(const BlockInfo &) = delete;
  ~BlockInfo();
};

using BlockInfoList = QList<std::shared_ptr<BlockInfo>>;

struct ActiveFold;

class TEXTEDIT_API Block
{
public:
  Block();
  Block(const Block & other);
  ~Block();

  Block(TextViewImpl *view);
  Block(int num, TextViewImpl *view);
  
  inline int number() const { return mNumber; }
  TextBlock block() const;
  inline const QString & text() const { return block().text(); }

  Block next() const;
  Block previous() const;
  void seekNext();
  void seekPrevious();
  void seek(int num);

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

  int span() const;
  QString displayedText() const;

  Fragment begin() const;
  Fragment end() const;

  BlockInfo & impl();
  const BlockInfo & impl() const;

  Block & operator=(const Block & other) = default;

  bool operator==(const Block & other) const;
  bool operator!=(const Block & other) const;
  bool operator<(const Block & other) const;

protected:
  friend class Blocks;
  friend class Line;
  friend class TextView;
  friend class TextViewImpl;

  void notifyBlockDestroyed(int linenum);
  void notifyBlockInserted(const Position & pos);

  ActiveFold getFold() const;

private:
  int mNumber;
  TextViewImpl *mView;
};

class TEXTEDIT_API Blocks
{
public:
  explicit Blocks(TextViewImpl *view);
  Blocks(const Blocks & other) = default;
  ~Blocks() = default;

  int count() const;
  Block begin() const;
  Block end() const;
  Block at(int index) const;

  Blocks & operator=(const Blocks & other) = default;

private:
  TextViewImpl *mView;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_BLOCK_H
