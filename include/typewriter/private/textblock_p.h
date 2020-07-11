// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTBLOCK_P_H
#define TYPEWRITER_TEXTBLOCK_P_H

#include "typewriter/typewriter-defs.h"


namespace typewriter
{

class TextBlockImpl;

class TYPEWRITER_API TextBlockRef
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

class TYPEWRITER_API TextBlockImpl
{
public:
  TextBlockImpl();
  explicit TextBlockImpl(const std::string& text);

  int ref;
  int id;
  int revision;
  std::string content;
  TextBlockRef previous;
  TextBlockRef next;

  inline bool isGarbage() const { return revision < 0; }
  void setGarbage();
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTBLOCK_P_H
