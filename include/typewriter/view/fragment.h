// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_FRAGMENT_H
#define TEXTEDIT_VIEW_FRAGMENT_H

#include "typewriter/textblock.h"
#include "typewriter/view/formatrange.h"

#include <vector>

namespace typewriter
{

class TextView;
class TextViewImpl;

namespace view
{

class Block;

class TYPEWRITER_API Fragment
{
public:
  Fragment();
  Fragment(const Fragment & other) = default;
  ~Fragment();

  Fragment(Block const *line, int col, std::vector<FormatRange>::const_iterator iter, std::vector<FormatRange>::const_iterator sentinel, TextViewImpl const *view);
  
  inline bool isNull() const { return mView == nullptr; }

  int format() const;
  int position() const;
  int length() const;

  TextBlock block() const;
  std::string text() const;

  Fragment next() const;

  Fragment & operator=(const Fragment &) = default;
  bool operator==(const Fragment & other) const;
  bool operator!=(const Fragment & other) const;

protected:
  friend class TextView;
  friend class TextViewImpl;

private:
  Block const *mLine;
  int mColumn;
  std::vector<FormatRange>::const_iterator mIterator;
  std::vector<FormatRange>::const_iterator mSentinel;
  TextViewImpl const *mView;
};

} // namespace view

} // namespace typewriter

#endif // !TEXTEDIT_VIEW_FRAGMENT_H
