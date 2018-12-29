// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_METRICS_H
#define TEXTEDIT_VIEW_METRICS_H

#include "textedit/textformat.h"

namespace textedit
{

namespace view
{

struct Metrics
{
  int charwidth;
  int lineheight;
  int ascent;
  int descent;
  int underlinepos;
  int strikeoutpos;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_METRICS_H
