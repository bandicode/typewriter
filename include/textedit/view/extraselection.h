// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_EXTRASELECTION_H
#define TEXTEDIT_VIEW_EXTRASELECTION_H

#include "textedit/textcursor.h"
#include "textedit/textformat.h"

namespace textedit
{

namespace view
{

struct ExtraSelection {
  TextCursor cursor;
  TextFormat format;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_EXTRASELECTION_H
