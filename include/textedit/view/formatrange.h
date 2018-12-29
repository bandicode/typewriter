// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_FORMAT_RANGE_H
#define TEXTEDIT_VIEW_FORMAT_RANGE_H

#include "textedit/textformat.h"

namespace textedit
{

namespace view
{

struct FormatRange
{
  TextFormat format;
  int start;
  int length;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_FORMAT_RANGE_H
