// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTBLOCK_H
#define TYPEWRITER_TEXTBLOCK_H

#include "typewriter/typewriter-defs.h"

#include <string>

namespace typewriter
{

class TextBlockImpl;
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

  int blockNumber() const;
  int offset() const;

  int blockId() const;
  void setBlockId(int id);

  TextBlock next() const; 
  TextBlock previous() const;

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

TYPEWRITER_API TextBlock next(TextBlock block, int n = 1);
TYPEWRITER_API TextBlock prev(TextBlock block, int n = 1);

} // namespace typewriter

#endif // !TYPEWRITER_TEXTBLOCK_H
