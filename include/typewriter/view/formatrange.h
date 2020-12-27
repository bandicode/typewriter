// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_VIEW_FORMAT_RANGE_H
#define TYPEWRITER_VIEW_FORMAT_RANGE_H

namespace typewriter
{

namespace view
{

struct FormatRange
{
  int format_id = 0;
  int start;
  int length;
};

} // namespace view

} // namespace typewriter

#endif // !TYPEWRITER_VIEW_FORMAT_RANGE_H
