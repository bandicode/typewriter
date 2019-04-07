// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTEDITOR_P_H
#define TEXTEDIT_TEXTEDITOR_P_H

#include "textedit/private/textview_p.h"

#include "textedit/textcursor.h"

namespace textedit
{

class TextEditorImpl : public TextViewImpl
{
public:
  TextEditorImpl(TextDocument *doc);

  TextCursor cursor;
  int timer;
  bool cursorblink;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTEDITOR_P_H
