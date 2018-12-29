// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTBLOCK_H
#define TEXTEDIT_TEXTBLOCK_H

#include "textedit/textedit.h"

#include <QString>

namespace textedit
{

class TextBlockImpl;
class TextDocument;

class TEXTEDIT_API TextBlock
{
public:
  TextBlock();
  TextBlock(const TextBlock & other);
  ~TextBlock();

  TextBlock(const TextDocument *doc, TextBlockImpl *impl);

  inline const TextDocument* document() const { return mDocument; }

  inline bool isNull() const { return mImpl == nullptr; }
  bool isValid() const;

  const QString & text() const;
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

TEXTEDIT_API TextBlock next(TextBlock block, int n = 1);
TEXTEDIT_API TextBlock prev(TextBlock block, int n = 1);

} // namespace textedit

#endif // !TEXTEDIT_TEXTBLOCK_H
