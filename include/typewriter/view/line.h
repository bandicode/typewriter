// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_VIEW_LINE_H
#define TYPEWRITER_VIEW_LINE_H

#include "typewriter/textblock.h"
#include "typewriter/stringview.h"
#include "typewriter/view/formatrange.h"

#include <list>
#include <vector>

namespace typewriter
{

class TextView;
class TextViewImpl;

namespace view
{

class Block;
class Fragment;

struct SimpleLineElement
{
  enum Kind
  {
    LE_BlockFragment,
    LE_Fold,
    LE_Insert,
    LE_InlineInsert,
    LE_CarriageReturn,
    LE_LineIndent,
  };

  Kind kind = LE_BlockFragment;
  TextBlock block;
  int begin = 0;
  int width = 0;
  int id = -1; // fold-id or insert-id or inline-insert-id
  int nbrow = 0;
};

struct LineInfo
{
  std::vector<SimpleLineElement> elements;

  static int width(const std::vector<SimpleLineElement>& elems)
  {
    int w = 0;

    for (const auto& e : elems)
    {
      w += e.width;
    }

    return w;
  }

  int width() const
  {
    return width(this->elements);
  }

  std::string displayedText() const;
};

} // namespace view

} // namespace typewriter

#endif // !TYPEWRITER_VIEW_LINE_H
