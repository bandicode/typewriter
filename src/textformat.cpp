// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textformat.h"

namespace textedit
{

TextFormat::TextFormat()
  : mBold(false)
  , mItalic(false)
  , mStrikedthrough(false)
  , mUnderline(NoUnderline)
  , mTextColor(QColor(0, 0, 0))
  , mBackgroundColor(QColor(255, 255, 255))
  , mForegroundColor(QColor(0, 0, 0, 0))
{

}

} // namespace textedit
