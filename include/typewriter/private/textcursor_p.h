// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTCURSOR_P_H
#define TYPEWRITER_TEXTCURSOR_P_H

#include "typewriter/textblock.h"

namespace typewriter
{

class TYPEWRITER_API TextCursorImpl
{
public:
  int ref;
  TextBlock block;
  Position pos;
  Position anchor;
  /// TODO:
  // UndoStack
  // RedoStack

public:
  TextCursorImpl(const TextBlock & firstBlock);
  TextCursorImpl(const Position & pos, const TextBlock & b);

  /* Move operations */
  bool move_down(int n);
  bool move_left(int n);
  bool move_right(int n);
  bool move_up(int n);
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTCURSOR_P_H
