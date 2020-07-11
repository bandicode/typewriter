// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/utils/range.h"

#include <algorithm>
#include <cassert>

namespace typewriter
{

Range::Range() : 
  mBegin{ 0, 0 }, 
  mEnd{ 0, 0 }
{

}

Range::Range(const Position & start_pos, const Position & end_pos) :
  mBegin(start_pos),
  mEnd(end_pos)
{

}

bool Range::isEmpty() const
{
  return begin() == end();
}

bool Range::contains(const Position & pos) const
{
  return pos >= begin() && pos < end();
}

bool Range::contains(const Range & other) const
{
  return begin() <= other.begin() && end() >= other.end();
}

Range::ComparisonResult Range::comp(const Range & a, const Range & b)
{
  if (a.end() < b.begin())
    return A_B;
  else if (a.end() == b.begin())
    return AB;
  else if (b.end() < a.begin())
    return B_A;
  else if (b.end() == a.begin())
    return BA;

  // A & B overlap

  if (a.begin() <= b.begin())
  {
    if (a.begin() == b.begin())
    {
      if (a.end() < b.end())
        return CB;
      else if (a.end() == b.end())
        return C;
      else
        return CA;
    }
    else
    {
      assert(a.begin() < b.begin());

      if (a.end() < b.end())
        return ACB;
      else if (a.end() == b.end())
        return AC;
      else
        return ACA;
    }
  }
  else
  {
    assert(a.begin() > b.begin());

    if (a.end() < b.end())
      return BCB;
    else if (a.end() == b.end())
      return BC;
    else // a.end() > b.end()
      return BCA;
  }
}

void Range::move(const Position & pos)
{
  if (end().line == begin().line)
  {
    const int col_diff = end().column - begin().column;
    mBegin = pos;
    mEnd = Position{ mBegin.line, mBegin.column + col_diff };
  }
  else
  {
    const int line_diff = end().line - begin().line;
    mBegin = pos;
    mEnd.line += mBegin.line + line_diff;
  }
}

bool operator==(const Range & lhs, const Range & rhs)
{
  return lhs.begin() == rhs.begin() && rhs.end() == lhs.end();
}

bool operator!=(const Range & lhs, const Range & rhs)
{
  return lhs.begin() != rhs.begin() || rhs.end() != lhs.end();
}

bool operator<(const Range & lhs, const Range & rhs)
{
  return lhs.begin() < rhs.begin() || (lhs.begin() == rhs.begin() && lhs.end() < rhs.end());
}

bool operator<=(const Range & lhs, const Range & rhs)
{
  return lhs.begin() < rhs.begin() || (lhs.begin() == rhs.begin() && lhs.end() <= rhs.end());
}

bool operator>(const Range & lhs, const Range & rhs)
{
  return !(lhs <= rhs);
}

bool operator>=(const Range & lhs, const Range & rhs)
{
  return !(lhs < rhs);
}

Range operator|(const Range & lhs, const Range & rhs)
{
  return Range(std::min(lhs.begin(), rhs.begin()), std::max(lhs.end(), rhs.end()));
}

Range operator&(const Range & lhs, const Range & rhs)
{
  const Position start = std::max(lhs.begin(), rhs.begin());
  const Position end = std::min(lhs.end(), rhs.end());

  if (end < start)
    return Range{};

  return Range{ start, end };
}

} // namespace typewriter
