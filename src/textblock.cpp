// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textblock.h"
#include "textedit/private/textblock_p.h"

#include "textedit/textdocument.h"
#include "textedit/private/textdocument_p.h"

namespace textedit
{

TextBlockRef::TextBlockRef()
  : d(nullptr)
{

}

TextBlockRef::TextBlockRef(TextBlockImpl *data)
  : d(data)
{
  if (d)
    d->ref++;
}

TextBlockRef::TextBlockRef(const TextBlockRef & other)
  : d(other.d)
{
  if (d)
    d->ref++;
}

TextBlockRef::~TextBlockRef()
{
  if(d != nullptr && --d->ref == 0)
    delete d;
}

TextBlockRef & TextBlockRef::operator=(const TextBlockRef & other)
{
  if (other.d)
  {
    other.d->ref += 1;
  }

  if (d)
  {
    if (--d->ref == 0)
    {
      delete d;
    }
  }

  d = other.d;

  return *this;
}

TextBlockRef & TextBlockRef::operator=(TextBlockRef && other)
{
  if (d)
  {
    if (--d->ref == 0)
    {
      delete d;
    }
  }

  d = other.d;
  other.d = nullptr;

  return *this;
}

TextBlockRef & TextBlockRef::operator=(TextBlockImpl *ptr)
{
  if (d)
  {
    if (--d->ref == 0)
    {
      delete d;
    }
  }

  d = ptr;
  d->ref += 1;

  return *this;
}

TextBlockRef & TextBlockRef::operator=(nullptr_t)
{
  if (d)
  {
    if (--d->ref == 0)
    {
      delete d;
    }
  }
  
  d = nullptr;

  return *this;
}

bool TextBlockRef::isValid() const
{
  return !isNull() && !d->isGarbage();
}

TextBlockImpl::TextBlockImpl()
  : ref(0)
  , revision(0)
{

}

TextBlockImpl::TextBlockImpl(const QString & text)
  : ref(0)
  , revision(0)
  , content(text)
{

}

void TextBlockImpl::setGarbage()
{
  this->revision = -1;
}

TextBlock::TextBlock()
  : mDocument(nullptr)
  , mImpl(nullptr)
{

}

TextBlock::TextBlock(const TextBlock & other)
  : mDocument(other.mDocument)
  , mImpl(other.mImpl)
{
  if (mImpl)
    mImpl->ref += 1;
}

TextBlock::TextBlock(const TextDocument *doc, TextBlockImpl *impl)
  : mDocument(doc)
  , mImpl(impl)
{
  if (mImpl)
  {
    mImpl->ref += 1;
  }
}

TextBlock::~TextBlock()
{
  if (!mImpl)
    return;

  if (--mImpl->ref == 0)
    delete mImpl;
}

bool TextBlock::isValid() const
{
  return !isNull() && !mImpl->isGarbage();
}

const QString & TextBlock::text() const
{
  return mImpl->content;
}

int TextBlock::length() const
{
  return mImpl->content.length();
}

int TextBlock::blockNumber() const
{
  return mImpl == nullptr ? -1 : document()->impl()->blockNumber(mImpl);
}

int TextBlock::offset() const
{
  return document()->impl()->blockOffset(mImpl);
}

int TextBlock::blockId() const
{
  return mImpl->id;
}

void TextBlock::setBlockId(int id)
{
  mImpl->id = id;
}

TextBlock TextBlock::next() const
{
  if (mImpl == nullptr)
    return *this;

  if (mImpl->next.isNull())
    return TextBlock{};

  return TextBlock{ document(), mImpl->next.get() };
}

TextBlock TextBlock::previous() const
{
  if (mImpl == nullptr)
    return *this;

  if (mImpl->previous.isNull())
    return TextBlock{};

  return TextBlock{ document(), mImpl->previous.get() };
}

int TextBlock::revision() const
{
  return mImpl->revision;
}

TextBlock & TextBlock::operator=(const TextBlock & other)
{
  if (&other == this)
    return *this;

  if (mImpl != nullptr)
  {
    if (--mImpl->ref == 0)
    {
      delete mImpl;
    }
  }

  mDocument = other.mDocument;
  mImpl = other.mImpl;
  if (mImpl)
  {
    mImpl->ref++;
  }

  return *this;
}

bool TextBlock::operator==(const TextBlock & other) const
{
  return mDocument == other.mDocument && blockNumber() == other.blockNumber();
}

bool TextBlock::operator!=(const TextBlock & other) const
{
  return !(*this == other);
}

bool TextBlock::operator<(const TextBlock & other) const
{
  return document() < other.document() || (document() == other.document() && blockNumber() < other.blockNumber());
}

TextBlock next(TextBlock block, int n)
{
  if (n < 0)
    return prev(block, -n);

  while (n > 0)
  {
    block = block.next();
    --n;
  }
  return block;
}

TextBlock prev(TextBlock block, int n)
{
  if (n < 0)
    return next(block, -n);

  while (n > 0)
  {
    block = block.previous();
    --n;
  }
  return block;
}

} // namespace textedit
