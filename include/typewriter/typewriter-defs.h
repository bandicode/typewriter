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

struct LineRange
{
  int start;
  int end;
};

} // namespace typewriter

#endif // !TYPEWRITER_DEFS_H
