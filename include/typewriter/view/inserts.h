// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_VIEW_INSERTS_H
#define TYPEWRITER_VIEW_INSERTS_H

#include "typewriter/typewriter-defs.h"

#include "typewriter/textcursor.h"

namespace typewriter
{

namespace view
{

struct Insert
{
  TextCursor cursor;
  int span;
  void* ptr = nullptr;
};

struct InlineInsert
{
  TextCursor cursor;
  int span;
  void* ptr = nullptr;
};

} // namespace view

} // namespace typewriter

#endif // !TYPEWRITER_VIEW_INSERTS_H
