// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTFOLD_H
#define TEXTEDIT_TEXTFOLD_H

#include "textedit/textcursor.h"

#include <QList>
#include <QString>

namespace textedit
{

class TEXTEDIT_API TextFold
{
public:
  explicit TextFold(const TextCursor & c, const QString & description = QString());
  TextFold(const TextFold &) = default;
  ~TextFold() = default;

  inline bool isActive() const { return mActive; }
  void setActive(bool active = true);

  inline const TextCursor& cursor() const { return mCursor; }
  inline const QString& description() const { return mDescription; }

  const Position& start() const;
  const Position& end() const;

  TextFold& operator=(const TextFold &) = default;

private:
  bool mActive;
  TextCursor mCursor;
  QString mDescription;
};

bool operator==(const TextFold & lhs, const TextFold & rhs);
inline bool operator!=(const TextFold & lhs, const TextFold & rhs) { return !(lhs == rhs); }

class TEXTEDIT_API TextFoldList
{
public:
  TextFoldList() = default;
  TextFoldList(const TextFoldList &) = default;
  ~TextFoldList() = default;

  typedef QList<TextFold>::iterator iterator;
  typedef QList<TextFold>::const_iterator const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  inline const int size() const { return mList.size(); }
  inline const bool empty() const { return mList.isEmpty(); }

  const TextFold& at(int index) const;
  TextFold& at(int index);
  inline const TextFold& front() const { return mList.front(); }
  inline const TextFold& back() const { return mList.back(); }

  void insert(const TextFold & fold);

  void remove(iterator pos);
  void remove(const TextFold & fold);

  const int activeCount() const;
  const int lastActiveIndex() const;
  const TextFold& activeBack() const;

  TextFoldList& operator=(const TextFoldList &) = default;

private:
  QList<TextFold> mList;
};


} // namespace textedit

#endif // !TEXTEDIT_TEXTFOLD_H
