// Copyright (C) 2018 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_VIEW_BLOCK_H
#define TYPEWRITER_VIEW_BLOCK_H

#include "typewriter/textblock.h"
#include "typewriter/view/formatrange.h"
#include "typewriter/view/line.h"

#include <list>
#include <vector>
#include <memory>

namespace typewriter
{

class TextView;
class TextViewImpl;

namespace view
{

class Fragment;

struct BlockInfo
{
  TextBlock block;
  std::vector<FormatRange> formats;
  int userstate;
  int revision;
  std::weak_ptr<BlockInfo> prev;
  std::weak_ptr<BlockInfo> next;
  std::list<view::LineInfo>::iterator line;

public:
  BlockInfo(const TextBlock & b);
  BlockInfo(const BlockInfo &) = delete;
  ~BlockInfo();
};

} // namespace view

} // namespace typewriter

#endif // !TYPEWRITER_VIEW_BLOCK_H
