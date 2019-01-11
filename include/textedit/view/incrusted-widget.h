// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_INCRUSTED_WIDGET_H
#define TEXTEDIT_VIEW_INCRUSTED_WIDGET_H

#include "textedit/textedit.h"

class QWidget;

namespace textedit
{

namespace view
{

struct IncrustedWidget
{
  int block;
  int span;
  QWidget *widget;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_INCRUSTED_WIDGET_H
