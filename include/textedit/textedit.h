// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_DEFS_H
#define TEXTEDIT_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(TEXTEDIT_STATIC_LINKING)
#if defined(TEXTEDIT_BUILD_LIB)
#  define TEXTEDIT_API __declspec(dllexport)
#else
#  define TEXTEDIT_API __declspec(dllimport)
#endif
#else
#define TEXTEDIT_API
#endif

namespace textedit
{

struct Position
{
  int line;
  int column;
};

inline bool operator==(const Position & lhs, const Position & rhs) { return lhs.line == rhs.line && lhs.column == rhs.column; }
inline bool operator!=(const Position & lhs, const Position & rhs) { return !(lhs == rhs); }
inline bool operator<(const Position & lhs, const Position & rhs) { return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column < rhs.column); }
inline bool operator<=(const Position & lhs, const Position & rhs) { return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column <= rhs.column); }
inline bool operator>(const Position & lhs, const Position & rhs) { return !(lhs <= rhs); }
inline bool operator>=(const Position & lhs, const Position & rhs) { return !(lhs < rhs); }

struct TEXTEDIT_API Range
{
  Position start;
  Position end;

  bool contains(const Position & pos) const;
  bool intersects(const Range & other) const;
};

TEXTEDIT_API bool operator==(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator!=(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator<(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator<=(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator>(const Range & lhs, const Range & rhs);
TEXTEDIT_API bool operator>=(const Range & lhs, const Range & rhs);

struct TEXTEDIT_API LineRange
{
  int start;
  int end;
};

} // namespace textedit

#endif // !TEXTEDIT_DEFS_H
