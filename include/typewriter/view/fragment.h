// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_VIEW_FRAGMENT_H
#define TEXTEDIT_VIEW_FRAGMENT_H

#include "typewriter/textblock.h"
#include "typewriter/view/formatrange.h"

#include <vector>

namespace typewriter
{

class TextView;
class TextViewImpl;

namespace view
{

class Block;

class TYPEWRITER_API StyledFragment
{
public:
  StyledFragment();
  StyledFragment(const StyledFragment&) = default;
  ~StyledFragment();

  StyledFragment(TextViewImpl const* view, Line const* line, LineElement elem);
  StyledFragment(TextViewImpl const* view, Line const* line, const TextBlock& block, int begin, int end);
  StyledFragment(TextViewImpl const* view, Line const* line, const TextBlock& block, int begin, int end, std::vector<FormatRange>::const_iterator iter);

  inline bool isNull() const { return mView == nullptr; }

  int format() const;
  int position() const;
  int length() const;

  TextBlock block() const;
  std::string text() const;

  StyledFragment next() const;

  StyledFragment& operator=(const StyledFragment&) = default;
  bool operator==(const StyledFragment& other) const;
  bool operator!=(const StyledFragment& other) const;

protected:
  friend class TextView;
  friend class TextViewImpl;

private:
  TextViewImpl const* mView = nullptr;
  Line const* mLine = nullptr;
  int mColumn = -1;
  int mEnd = -1;
  std::shared_ptr<view::Block> m_block;
  std::vector<FormatRange>::const_iterator mIterator;
};

class TYPEWRITER_API StyledFragments
{
public:
  StyledFragments() = delete;
  StyledFragments(const StyledFragments&) = default;
  ~StyledFragments() = default;

  StyledFragments(TextViewImpl const* view, Line const* line, LineElement elem);

  StyledFragment begin() const;
  StyledFragment end() const;

protected:
  friend class TextView;
  friend class TextViewImpl;

private:
  TextViewImpl const* m_view = nullptr;
  Line const* m_line = nullptr;
  TextBlock m_block;
  int m_begin = -1;
  int m_end = -1;
};

} // namespace view

} // namespace typewriter

#endif // !TEXTEDIT_VIEW_FRAGMENT_H
