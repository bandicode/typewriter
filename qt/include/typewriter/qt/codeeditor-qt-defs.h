// Copyright (C) 2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_DEFS_H
#define TYPEWRITER_QTYPEWRITER_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(TYPEWRITER_QT_BUILD_STATIC)

#if defined(TYPEWRITER_QT_BUILD_SHARED)
#  define TYPEWRITER_QAPI __declspec(dllexport)
#else
#  define TYPEWRITER_QAPI __declspec(dllimport)
#endif

#else

#  define TYPEWRITER_QAPI

#endif

#endif // !TYPEWRITER_QTYPEWRITER_DEFS_H
