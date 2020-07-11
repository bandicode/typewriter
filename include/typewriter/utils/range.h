// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_UTILS_RANGE_H
#define TYPEWRITER_UTILS_RANGE_H

#include "typewriter/typewriter-defs.h"

namespace typewriter
{

class TYPEWRITER_API Range
{
public:
  Range();
  Range(const Range &) = default;
  ~Range() = default;

  Range(const Position & start_pos, const Position & end_pos);

  inline const Position & begin() const { return mBegin; }
  inline const Position & end() const { return mEnd; }
  inline void setBegin(const Position & pos) { mBegin = pos; }
  inline void setEnd(const Position & pos) { mEnd = pos; }
  inline void setBegin(int line, int col) { mBegin = Position{ line, col }; }
  inline void setEnd(int line, int col) { mEnd = Position{ line, col }; }

  bool isEmpty() const;

  bool contains(const Position & pos) const;
  bool contains(const Range & other) const;

  enum ComparisonResult
  {
    AB,
    A_B,
    BA,
    B_A,
    Same,
    C = Same,
    ACB,
    BCA,
    ACA,
    BCB,
    AC,
    CA,
    BC,
    CB,
  };

  static ComparisonResult comp(const Range & a, const Range & b);

  void move(const Position & pos);

  Range & operator=(const Range &) = default;

private:
  Position mBegin;
  Position mEnd;
};

TYPEWRITER_API bool operator==(const Range & lhs, const Range & rhs);
TYPEWRITER_API bool operator!=(const Range & lhs, const Range & rhs);
TYPEWRITER_API bool operator<(const Range & lhs, const Range & rhs);
TYPEWRITER_API bool operator<=(const Range & lhs, const Range & rhs);
TYPEWRITER_API bool operator>(const Range & lhs, const Range & rhs);
TYPEWRITER_API bool operator>=(const Range & lhs, const Range & rhs);

TYPEWRITER_API Range operator|(const Range & lhs, const Range & rhs);
TYPEWRITER_API Range operator&(const Range & lhs, const Range & rhs);

} // namespace typewriter

#endif // !TYPEWRITER_UTILS_RANGE_H
