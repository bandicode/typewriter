// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_DEFS_H
#define TYPEWRITER_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(TYPEWRITER_STATIC_LINKING)
#if defined(TYPEWRITER_BUILD_LIB)
#  define TYPEWRITER_API __declspec(dllexport)
#else
#  define TYPEWRITER_API __declspec(dllimport)
#endif
#else
#define TYPEWRITER_API
#endif

namespace typewriter
{

class Position
{
public:
  int line;
  int column;

  Position(int l = 0, int col = 0)
    : line(l), column(col)
  {

  }

  Position& operator=(const Position&) = default;
};

inline bool operator==(const Position & lhs, const Position & rhs) { return lhs.line == rhs.line && lhs.column == rhs.column; }
inline bool operator!=(const Position & lhs, const Position & rhs) { return !(lhs == rhs); }
inline bool operator<(const Position & lhs, const Position & rhs) { return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column < rhs.column); }
inline bool operator<=(const Position & lhs, const Position & rhs) { return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column <= rhs.column); }
inline bool operator>(const Position & lhs, const Position & rhs) { return !(lhs <= rhs); }
inline bool operator>=(const Position & lhs, const Position & rhs) { return !(lhs < rhs); }

} // namespace typewriter

#endif // !TYPEWRITER_DEFS_H
