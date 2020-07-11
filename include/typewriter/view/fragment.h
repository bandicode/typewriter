// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_FRAGMENT_H
#define TEXTEDIT_VIEW_FRAGMENT_H

#include "textedit/textblock.h"
#include "textedit/view/formatrange.h"

#include <QVector>

namespace textedit
{

class TextView;
class TextViewImpl;

namespace view
{

struct BlockInfo;

class TEXTEDIT_API Fragment
{
public:
  Fragment();
  Fragment(const Fragment & other) = default;
  ~Fragment();

  Fragment(BlockInfo const *line, int col, QVector<FormatRange>::const_iterator iter, QVector<FormatRange>::const_iterator sentinel, TextViewImpl const *view);
  
  inline bool isNull() const { return mView == nullptr; }

  const TextFormat & format() const;
  int position() const;
  int length() const;

  TextBlock block() const;
  QString text() const;

  Fragment next() const;

  Fragment & operator=(const Fragment &) = default;
  bool operator==(const Fragment & other) const;
  bool operator!=(const Fragment & other) const;

protected:
  friend class TextView;
  friend class TextViewImpl;

private:
  BlockInfo const *mLine;
  int mColumn;
  QVector<FormatRange>::const_iterator mIterator;
  QVector<FormatRange>::const_iterator mSentinel;
  TextViewImpl const *mView;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_FRAGMENT_H
