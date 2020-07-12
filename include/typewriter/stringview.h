// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_STRINGVIEW_H
#define TYPEWRITER_STRINGVIEW_H

#include "typewriter/typewriter-defs.h"

#include <unicode/utf8.h>

namespace typewriter
{

class StringView
{
private:
  const char* m_str;
  size_t m_size;

public:
  StringView(const char* str, size_t s)
    : m_str(str), m_size(s)
  {

  }

  size_t size() const { return m_size; }

  size_t length() const
  {
    unicode::Utf8Iterator it = unicode::utf8::begin(m_str);
    unicode::Utf8Iterator end = unicode::utf8::end(m_str+m_size);

    size_t result = 0;

    while (it != end)
      ++result, ++it;

    return result;
  }
};

} // namespace typewriter

#endif // !TYPEWRITER_STRINGVIEW_H
