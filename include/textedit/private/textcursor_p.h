// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTCURSOR_P_H
#define TEXTEDIT_TEXTCURSOR_P_H

#include "textedit/textedit.h"

#include "textedit/textblock.h"

namespace textedit
{

class TEXTEDIT_API TextCursorImpl
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

} // namespace textedit

#endif // !TEXTEDIT_TEXTCURSOR_P_H
