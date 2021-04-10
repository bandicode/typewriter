// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTBLOCK_H
#define TYPEWRITER_TEXTBLOCK_H

#include "typewriter/typewriter-defs.h"

#include <unicode/utf8.h>

#include <string>

namespace typewriter
{

class TextBlockImpl;
class TextBlockIterator;
class TextDocument;

class TYPEWRITER_API TextBlock
{
public:
  TextBlock();
  TextBlock(const TextBlock & other);
  ~TextBlock();

  TextBlock(const TextDocument *doc, TextBlockImpl *impl);

  inline const TextDocument* document() const { return mDocument; }

  inline bool isNull() const { return mImpl == nullptr; }
  bool isValid() const;

  const std::string& text() const;
  int length() const;
  size_t size() const;

  int blockNumber() const;
  int offset() const;

  int blockId() const;
  void setBlockId(int id);

  TextBlock next() const; 
  TextBlock previous() const;

  TextBlockIterator begin() const;
  TextBlockIterator end() const;

  int revision() const;

  inline TextBlockImpl* impl() const { return mImpl; }

  TextBlock & operator=(const TextBlock & other);
  bool operator==(const TextBlock & other) const;
  bool operator!=(const TextBlock & other) const;
  bool operator<(const TextBlock & other) const;

private:
  const TextDocument *mDocument;
  TextBlockImpl *mImpl;
};

class TYPEWRITER_API TextBlockIterator
{
public:
  TextBlockIterator()
    : m_column(0),
      m_iterator(unicode::utf8::begin("")),
      m_end(m_iterator)
  {

  }

  TextBlockIterator(const TextBlockIterator&) = default;

  ~TextBlockIterator() = default;

  explicit TextBlockIterator(const TextBlock& b, bool end_iterator = false)
    : m_block(b),
      m_column(0),
      m_iterator(unicode::utf8::begin("")),
      m_end(m_iterator)
  {
    if (b.isValid())
    {
      m_iterator = unicode::utf8::begin(b.text());
      m_end = unicode::utf8::end(b.text());
    }

    if (end_iterator)
      m_iterator = m_end;
  }

  bool isNull() const { return m_block.isNull(); }
  bool isValid() const { return !isNull(); }

  const TextBlock& block() const { return m_block; }

  int column() const { return m_column; }

  void seekColumn(int c)
  {
    if (c < m_column)
      *this = block().begin();

    while (!atEnd() && m_column != c)
      ++(*this);
  }


  bool atEnd() const { return m_iterator == m_end; }

  unicode::Character current() const { return *m_iterator; }
  unicode::Character operator*() const { return *m_iterator; }

  unicode::Utf8Iterator unicodeIterator() const { return m_iterator; }

  TextBlockIterator& operator++()
  {
    ++m_column;
    ++m_iterator;
    return *(this);
  }

  TextBlockIterator operator++(int)
  {
    TextBlockIterator ret = *(this);
    ++(*this);
    return ret;
  }

  TextBlockIterator& operator=(const TextBlockIterator&) = default;

private:
  TextBlock m_block;
  int m_column = 0;
  unicode::Utf8Iterator m_iterator;
  unicode::Utf8Iterator m_end;
};

inline bool operator==(const TextBlockIterator& lhs, const TextBlockIterator& rhs)
{
  return lhs.block() == rhs.block() && (lhs.column() == rhs.column() || lhs.atEnd() == rhs.atEnd());
}

inline bool operator!=(const TextBlockIterator& lhs, const TextBlockIterator& rhs)
{
  return !(lhs == rhs);
}

TYPEWRITER_API TextBlock next(TextBlock block, int n = 1);
TYPEWRITER_API TextBlock prev(TextBlock block, int n = 1);

} // namespace typewriter

#endif // !TYPEWRITER_TEXTBLOCK_H
