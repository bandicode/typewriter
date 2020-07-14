// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTDIFF_H
#define TYPEWRITER_TEXTDIFF_H

#include "typewriter/utils/range.h"

#include <string>
#include <vector>

namespace typewriter
{

class TYPEWRITER_API TextRange
{
public:
  explicit TextRange(const std::string& text, const Position & start = Position{ 0, 0 });
  TextRange(const TextRange &) = default;
  ~TextRange() = default;

  const Position& begin() const;
  const Position& end() const;

  const std::string& text() const { return m_text; }

  void move(const Position & start);

  int seek(const Position & pos);
  Position map(int offset) const;

  static Position end(const Position& start, const std::string& text);

  inline operator Range() const { return m_range; }

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
  Range m_range;
  std::string m_text;
};

class TYPEWRITER_API TextDiff
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
    inline const std::string& text() const { return content.text(); }
    void clear();
    Position endEditPos() const;

    void mapTo(const Diff & other);
  };

  inline const std::vector<Diff> & diffs() const { return mDiffs; }
  inline std::vector<Diff> & diffs() { return mDiffs; }

  void clear() { mDiffs.clear(); }
  
  Diff takeFirst();

  inline int agent() const { return mAgent; }

  void simplify();

  TextDiff& operator<<(Diff d);
  TextDiff& operator<<(const TextDiff & other);

private:
  TextDiff& add_insertion(Diff d);
  TextDiff& add_removal(Diff d);

  bool apply_removal(Diff& d, Diff& removal);

private:
  int mAgent;
  std::vector<Diff> mDiffs;
};

namespace diff
{

TextDiff::Diff insert(const Position& pos, const std::string& text);
TextDiff::Diff remove(const Position& pos, const std::string& text);

} // namespace diff

bool operator==(const TextDiff::Diff& lhs, const TextDiff::Diff& rhs);
inline bool operator!=(const TextDiff::Diff& lhs, const TextDiff::Diff& rhs) { return !(lhs == rhs); }

} // namespace typewriter

#endif // !TYPEWRITER_TEXTDIFF_H
