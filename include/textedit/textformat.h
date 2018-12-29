// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTFORMAT_H
#define TEXTEDIT_TEXTFORMAT_H

#include "textedit/textedit.h"

#include <QColor>
#include <QPen>

namespace textedit
{

class TEXTEDIT_API TextFormat
{
public:
  TextFormat();
  TextFormat(const TextFormat &) = default;
  ~TextFormat() = default;

  inline void setBold(bool on = true) { mBold = on; }
  inline bool isBold() const { return mBold; }
  inline void setItalic(bool on = true) { mItalic = on; }
  inline bool isItalic() const { return mItalic; }

  inline const QColor & textColor() const { return mTextColor; }
  inline void setTextColor(const QColor & color) { mTextColor = color; }

  inline const QColor & backgroundColor() const { return mBackgroundColor; }
  inline void setBackgroundColor(const QColor & color) { mBackgroundColor = color; }

  inline const QColor & foregroundColor() const { return mForegroundColor; }
  inline void setForegroundColor(const QColor & color) { mForegroundColor = color; }

  inline const QPen & borderPen() const { return mBorderPen; }
  inline void setBorderPen(const QPen & pen) { mBorderPen = pen; }

  enum UnderlineStyle {
    NoUnderline = 0,
    SingleUnderline = 1,
    DashUnderline = 2,
    DotLine = 3,
    DashDotLine = 4,
    DashDotDotLine = 5,
    WaveUnderline = 6,
  };

  inline UnderlineStyle underlineStyle()  const { return mUnderline; }
  inline void setUnderlineStyle(UnderlineStyle style) { mUnderline = style; }
  const QColor & underlineColor() const { return mUnderlineColor; }

  TextFormat & operator=(const TextFormat &) = default;

private:
  bool mBold;
  bool mItalic;
  bool mStrikedthrough;
  QColor mTextColor;
  QColor mBackgroundColor;
  QColor mForegroundColor;
  QPen mBorderPen;
  UnderlineStyle mUnderline;
  QColor mUnderlineColor;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTFORMAT_H
