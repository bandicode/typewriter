// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTBLOCK_P_H
#define TEXTEDIT_TEXTBLOCK_P_H

#include "textedit/textedit.h"


namespace textedit
{

class TextBlockImpl;

class TEXTEDIT_API TextBlockRef
{
private:
  TextBlockImpl *d;
public:
  TextBlockRef();
  explicit TextBlockRef(TextBlockImpl *data);
  TextBlockRef(const TextBlockRef & other);
  ~TextBlockRef();

  inline bool isNull() const { return d == nullptr; }
  bool isValid() const;

  inline TextBlockImpl* get() const { return d; }

  TextBlockRef & operator=(const TextBlockRef & other);
  TextBlockRef & operator=(TextBlockRef && other);

  TextBlockRef & operator=(TextBlockImpl *ptr);
  TextBlockRef & operator=(nullptr_t);

  inline bool operator==(const TextBlockImpl *ptr) { return d == ptr; }
};

class TEXTEDIT_API TextBlockImpl
{
public:
  TextBlockImpl();
  explicit TextBlockImpl(const QString & text);

  int ref;
  int id;
  int revision;
  QString content;
  TextBlockRef previous;
  TextBlockRef next;

  inline bool isGarbage() const { return revision < 0; }
  void setGarbage();
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTBLOCK_P_H
