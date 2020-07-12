// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTFOLD_H
#define TYPEWRITER_TEXTFOLD_H

#include "typewriter/textcursor.h"

namespace typewriter
{

struct TYPEWRITER_API SimpleTextFold
{
  TextCursor cursor;
  int width;
  int id;
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTFOLD_H
