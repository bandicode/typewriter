// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTDIFF_H
#define TEXTEDIT_TEXTDIFF_H

#include "textedit/utils/range.h"

#include <QList>
#include <QString>

namespace textedit
{

class TEXTEDIT_API TextRange
{
public:
  explicit TextRange(const QString & text, const Position & start = Position{ 0, 0 });
  TextRange(const TextRange &) = default;
  ~TextRange() = default;

  const Position & begin() const;
  const Position & end() const;

  inline const QString & text() const { return mText; }

  void move(const Position & start);

  int seek(const Position & pos);

  static Position end(const Position & start, const QString & text);

  inline operator Range() const { return mRange; }

  TextRange & operator=(const TextRange &) = default;

  TextRange& operator+=(const TextRange & other);
  TextRange& operator-=(const Range & other);

private:

  struct SeekHint
  {
    int index;
    Position pos;
  };

  int seek(const Position & pos, SeekHint hint);

private:
  Range mRange;
  QString mText;
};

class TEXTEDIT_API TextDiff
{
public:
  TextDiff() = default;
  TextDiff(const TextDiff &) = default;
  ~TextDiff() = default;

  enum Kind {
    Insertion,
    Removal
  };

  struct Diff
  {
    Kind kind;
    TextRange content;

    inline bool isInsertion() const { return kind == TextDiff::Insertion; }
    inline bool isRemoval() const { return kind == TextDiff::Removal; }
    inline Position begin() const { return content.begin(); }
    inline Position end() const { return content.end(); }
    inline bool isEmpty() const { return begin() == end(); }
    inline const QString & text() const { return content.text(); }
    void clear();
    Position endEditPos() const;

    void mapTo(const Diff & other);
  };

  inline const QList<Diff> & diffs() const { return mDiffs; }
  inline QList<Diff> & diffs() { return mDiffs; }
  
  Diff takeFirst() const;

  inline int agent() const { return mAgent; }

  TextDiff& operator<<(Diff d);
  TextDiff& operator<<(const TextDiff & other);

private:
  TextDiff& add_insertion(Diff d);
  TextDiff& add_removal(Diff d);

  bool apply_removal(Diff& d, Diff& removal);

private:
  int mAgent;
  QList<Diff> mDiffs;
};

namespace diff
{

TextDiff::Diff insert(const Position & pos, const QString & text);
TextDiff::Diff remove(const Position & pos, const QString & text);

} // namespace diff

bool operator==(const TextDiff::Diff & lhs, const TextDiff::Diff & rhs);
inline bool operator!=(const TextDiff::Diff & lhs, const TextDiff::Diff & rhs) { return !(lhs == rhs); }

} // namespace textedit

#endif // !TEXTEDIT_TEXTDIFF_H
