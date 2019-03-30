// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_UTILS_RANGE_H
#define TEXTEDIT_UTILS_RANGE_H

#include "textedit/textedit.h"

namespace textedit
{

class TEXTEDIT_API Range
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

TEXTEDIT_API bool operator==(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator!=(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator<(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator<=(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator>(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator>=(const Range & lhs, const Range & rhs);

TEXTEDIT_API Range operator|(const Range & lhs, const Range & rhs);
TEXTEDIT_API Range operator&(const Range & lhs, const Range & rhs);

} // namespace textedit

#endif // !TEXTEDIT_UTILS_RANGE_H
