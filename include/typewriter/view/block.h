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

class Block
{
public:
  TextBlock block;
  int blockformat = 0;
  int userstate = -1;
  std::vector<FormatRange> formats;
  // @TODO: see what to do with these variables
  // int revision;
  std::weak_ptr<Block> prev;
  std::weak_ptr<Block> next;
  std::list<view::Line>::iterator line;

public:
  Block(const TextBlock& b, std::list<view::Line>::iterator l);
  Block(const Block &) = delete;
  ~Block();
};

} // namespace view

} // namespace typewriter

#endif // !TYPEWRITER_VIEW_BLOCK_H
