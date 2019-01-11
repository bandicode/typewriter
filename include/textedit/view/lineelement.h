// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_LINE_ELEMENT_H
#define TEXTEDIT_VIEW_LINE_ELEMENT_H

#include "textedit/textblock.h"
#include "textedit/view/formatrange.h"

#include <QLinkedList>
#include <QVector>

namespace textedit
{

class TextView;
class TextViewImpl;

namespace view
{

class Fragment;

class LineElement_BlockFragment;
class LineElement_Fold;

class TEXTEDIT_API LineElement
{
public:
  virtual ~LineElement() = default;

  virtual bool isBlockFragment() const;
  virtual bool isFold() const;
  virtual bool isLineFeed() const;

  const LineElement_Fold & asFold() const;
  const LineElement_BlockFragment & asBlockFragment() const;
};

class TEXTEDIT_API LineElement_LineFeed : public LineElement
{
public:
  ~LineElement_LineFeed() = default;

  bool isLineFeed() const override;
};

class TEXTEDIT_API LineElement_BlockFragment : public LineElement
{
public:
  int begin;
  int end;

public:
  virtual ~LineElement_BlockFragment() = default;

  bool isBlockFragment() const override;
};

class TEXTEDIT_API LineElement_Fold : public LineElement
{
public:
  int foldid;

public:
  virtual ~LineElement_Fold() = default;

  bool isFold() const override;
};

} // namespace view

} // namespace textedit

#endif // !TEXTEDIT_VIEW_LINE_ELEMENT_H
