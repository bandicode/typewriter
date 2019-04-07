// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textfold.h"

#include <cassert>

namespace textedit
{

TextFold::TextFold(const TextCursor & c, const QString & description)
  : mActive(false),
  mCursor(c),
  mDescription(description)
{

}

void TextFold::setActive(bool active)
{
  mActive = active;
}

const Position& TextFold::start() const
{
  return cursor().anchor();
}

const Position& TextFold::end() const
{
  return cursor().position();
}

bool operator==(const TextFold & lhs, const TextFold & rhs)
{
  return lhs.cursor().position() == rhs.cursor().position()
    && lhs.cursor().anchor() == rhs.cursor().anchor();
}

TextFoldList::const_iterator TextFoldList::begin() const
{
  return mList.begin();
}

TextFoldList::const_iterator TextFoldList::end() const
{
  return mList.end();
}

TextFoldList::iterator TextFoldList::begin()
{
  return mList.begin();
}

TextFoldList::iterator TextFoldList::end()
{
  return mList.end();
}

const TextFold& TextFoldList::at(int index) const
{
  return mList.at(index);
}

TextFold& TextFoldList::at(int index)
{
  return mList[index];
}

void TextFoldList::insert(const TextFold & fold)
{
  for (int i(0); i < mList.size(); ++i)
  {
    if (fold.cursor().anchor() < mList.at(i).cursor().anchor())
    {
      mList.insert(i, fold);
      return;
    }
  }

  mList.push_back(fold);
}

void TextFoldList::remove(iterator pos)
{
  mList.erase(pos);
}

void TextFoldList::remove(const TextFold & fold)
{
  for (auto it = begin(); it != end(); ++it)
  {
    if (*it == fold)
    {
      remove(it);
      return;
    }
  }
}

const int TextFoldList::activeCount() const
{
  int result = 0;

  for (const auto& f : mList)
  {
    result += f.isActive() ? 1 : 0;
  }

  return result;
}

const int TextFoldList::lastActiveIndex() const
{
  for (int i(size() - 1); i >= 0; --i)
  {
    if (at(i).isActive())
      return i;
  }

  return -1;
}

const TextFold& TextFoldList::activeBack() const
{
  const int last_active = lastActiveIndex();
  Q_ASSERT(last_active != -1);

  return at(last_active);
}

} // namespace textedit
