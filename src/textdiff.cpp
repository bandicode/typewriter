// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/textdiff.h"

#include <cassert>

namespace textedit
{

TextRange::TextRange(const QString & text, const Position & start) :
  mRange(start, TextRange::end(start, text)),
  mText(text)
{

}

const Position & TextRange::begin() const
{
  return mRange.begin();
}

const Position & TextRange::end() const
{
  return mRange.end();
}

void TextRange::move(const Position & start)
{
  mRange.move(start);
}

Position TextRange::end(const Position & start, const QString & text)
{
  const int line_count = text.count('\n');

  if (line_count == 0)
    return Position{ start.line, start.column + text.size() };

  const int last_lf = text.lastIndexOf('\n');
  return Position{ start.line + line_count, text.length() - 1 - last_lf };
}

TextRange& TextRange::operator+=(const TextRange & other)
{
  if (other.begin() < begin() || other.begin() > end())
    return *this;

  const int insert_pos = seek(other.begin());

  mText.insert(insert_pos, other.text());

  /// TODO: compute this more efficiently
  mRange = Range(begin(), TextRange::end(begin(), mText));

  return *this;
}

TextRange& TextRange::operator-=(const Range & other)
{
  if (other.end() <= begin() || other.begin() >= end())
    return *this;

  const Position p0 = std::max(begin(), other.begin());
  const Position p1 = std::min(end(), other.end());

  SeekHint hint{ 0, begin() };
  const int erase_begin = seek(p0, hint);
  hint.pos = p0;
  hint.index = erase_begin;
  const int erase_end = seek(p1, hint);

  mText.remove(erase_begin, erase_end - erase_begin);

  /// TODO: compute this more efficiently
  mRange = Range(begin(), TextRange::end(begin(), mText));

  return *this;
}


int TextRange::seek(const Position & pos)
{
  SeekHint hint{ 0, begin() };
  return seek(pos, hint);
}

int TextRange::seek(const Position & pos, SeekHint hint)
{
  while (hint.pos != pos)
  {
    const QChar c = mText.at(hint.index);
    if (c == '\n')
    {
      hint.pos.line += 1;
      hint.pos.column = 0;
    }
    else
    {
      hint.pos.column += 1;
    }

    hint.index += 1;
  }

  return hint.index;
}

Position TextRange::map(int offset) const
{
  Position ret = begin();
  int i = 0;

  while (i < offset)
  {
    const QChar c = mText.at(i++);

    if (c == '\n')
    {
      ret.line += 1;
      ret.column = 0;
    }
    else
    {
      ret.column += 1;
    }
  }

  return ret;
}

void TextDiff::Diff::clear()
{
  content = TextRange(QString(), content.begin());
}

Position TextDiff::Diff::endEditPos() const
{
  return isRemoval() ? begin() : end();
}

void TextDiff::Diff::mapTo(const Diff & other)
{
  if (other.isInsertion())
  {
    const Diff & insert = other;

    if (this->begin().line == insert.end().line)
    {
      const int col_diff = this->begin().column - insert.end().column;
      assert(col_diff >= 0);
      this->content.move(Position{ insert.begin().line, insert.begin().column + col_diff });
    }
    else
    {
      assert(this->begin().line > insert.end().line);
      const int line_diff = insert.end().line - insert.begin().line;
      const auto new_begin_pos = Position{ this->begin().line - line_diff, this->begin().column };
      this->content.move(new_begin_pos);
    }
  }
  else if (other.isRemoval())
  {
    if (this->begin().line == other.begin().line)
    {
      const int col_diff = this->begin().column - other.begin().column;
      this->content.move(Position{ other.end().line, other.end().column + col_diff });
    }
    else
    {
      assert(this->begin().line > other.begin().line);
      const int line_diff = other.end().line - other.begin().line;
      const auto new_begin_pos = Position{ this->begin().line + line_diff, this->begin().column };
      this->content.move(new_begin_pos);
    }
  }
}

TextDiff::Diff TextDiff::takeFirst() const
{
  throw std::runtime_error{ "Not implemented" };
}

void TextDiff::simplify()
{
  if (mDiffs.size() <= 1)
    return;

  for (int i(0); i < mDiffs.size() - 1; ++i)
  {
    if (!mDiffs[i].isRemoval() || !mDiffs[i + 1].isInsertion() || mDiffs[i].end() != mDiffs[i + 1].begin())
      continue;

    auto& rm = mDiffs[i];
    auto& ins = mDiffs[i + 1];

    const int s = std::min(rm.content.text().length(), ins.content.text().length());
    int nb_common = 0;

    while (nb_common < s && rm.content.text().at(nb_common) == ins.content.text().at(nb_common))
      ++nb_common;

    if (nb_common == 0)
      continue;

    const Position new_begin = rm.content.map(nb_common);
    ins.content = TextRange(ins.text().mid(nb_common), ins.begin());
    rm.content = TextRange(rm.text().mid(nb_common), new_begin);

    if (ins.isEmpty())
      mDiffs.removeAt(i + 1);

    if (rm.isEmpty())
      mDiffs.removeAt(i);
  }
}

TextDiff& TextDiff::operator<<(Diff d)
{
  if (mDiffs.isEmpty())
  {
    mDiffs.append(d);
    return *(this);
  }

  if (d.isInsertion())
    return add_insertion(d);
  else
    return add_removal(d);
}

TextDiff& TextDiff::operator<<(const TextDiff & other)
{
  TextDiff temp = other;
  while (!temp.diffs().isEmpty())
  {
    auto diff = temp.takeFirst();
    (*this) << diff;
  }

  return *this;
}

TextDiff& TextDiff::add_insertion(Diff d)
{
  assert(d.isInsertion());

  int index = 0;
  while (index < mDiffs.size())
  {
    Diff & current_diff = mDiffs[index];
    if (d.begin() < current_diff.begin())
    {
      mDiffs.insert(index, d);
      return *(this);
    }
    else if (d.begin() == current_diff.begin()) 
    {
      if (current_diff.kind == Insertion)
      {
        current_diff.content.move(d.end());
        std::swap(current_diff, d);
        current_diff.content += d.content;
        return *(this);
      }
      else
      {
        d.content.move(current_diff.end());
      }
    }
    else if (d.begin() < current_diff.end())
    {
      if (current_diff.isInsertion())
      {
        current_diff.content += d.content;
        return *(this);
      }
      else
      {
        d.mapTo(current_diff);
      }
    }
    else if (d.begin() == current_diff.end())
    {
      if (current_diff.isInsertion())
      {
        current_diff.content += d.content;
        return *(this);
      }
      else
      {
        d.mapTo(current_diff);
      }
    }
    else
    {
      assert(d.begin() > current_diff.end());

      if (current_diff.isInsertion())
      {
        d.mapTo(current_diff);
      }
      else
      {
        d.mapTo(current_diff);
      }
    }

    ++index;
  }

  mDiffs.append(d);
  return *this;
}

TextDiff& TextDiff::add_removal(Diff rm)
{
  assert(rm.isRemoval());

  // Modify existing diff to take into account the removal
  for (auto& d : mDiffs)
  {
    if (apply_removal(d, rm))
      break;
  }

  // Insert the removal into the list & remove empty diffs
  bool inserted = false;
  for (int i(0); i < mDiffs.size(); ++i)
  {
    if (mDiffs.at(i).isEmpty())
    {
      mDiffs.removeAt(i);
      --i;
    }
    else
    {
      if (rm.begin() < mDiffs.at(i).begin())
      {
        if (!rm.isEmpty() && !inserted)
        {
          mDiffs.insert(i, rm);
          ++i;
        }

        inserted = true;
      }
    }
  }

  if (!inserted && !rm.isEmpty())
  {
    mDiffs.append(rm);
  }

  return *this;
}

bool TextDiff::apply_removal(Diff& d, Diff& removal)
{
  const Range::ComparisonResult rcomp = Range::comp(d.content, removal.content);

  if (d.isInsertion())
  {
    auto& cinsert = d;

    switch (rcomp)
    {
    case Range::C:
    {
      d.clear();
      removal.clear();
      return true;
    }
    case Range::AB:
    case Range::A_B:
    {
      removal.mapTo(cinsert);
      return false;
    }
    case Range::BA:
    case Range::B_A:
    {
      return true;
    }
    case Range::CA:
    case Range::AC:
    case Range::BC:
    case Range::CB:
    case Range::ACA:
    case Range::ACB:
    case Range::BCA:
    case Range::BCB:
    {
      Range overlap = cinsert.content & removal.content;
      removal.content -= overlap;
      cinsert.content -= overlap;

      removal.content.move(std::min(removal.begin(), cinsert.begin()));
      cinsert.content.move(removal.end());

      return false;
    }
    default:
      return false;
    }
  }
  else
  {
    auto& crm = d;

    switch (rcomp)
    {
    case Range::C:
    case Range::CA:
    case Range::CB:
    {
      removal.content = TextRange(crm.text() + removal.text(), crm.begin());
      crm.clear();
      return false;
    }
    case Range::BA:
    {
      removal.content = TextRange(removal.text() + crm.text(), removal.begin());
      crm.clear();
      return false;
    }
    case Range::BC:
    case Range::BCB:
    case Range::BCA:
    {
      removal.content += crm.content;
      crm.clear();
    }
    case Range::A_B:
    case Range::AB:
    case Range::ACB:
    case Range::ACA:
    case Range::AC:
    {
      removal.mapTo(crm);
      return false;
    }
    case Range::B_A:
    default:
      return false;
    }
  }
}

namespace diff
{

TextDiff::Diff insert(const Position & pos, const QString & text)
{
  return TextDiff::Diff{ TextDiff::Insertion, TextRange{ text, pos } };
}

TextDiff::Diff remove(const Position & pos, const QString & text)
{
  return TextDiff::Diff{ TextDiff::Removal, TextRange{ text, pos } };
}

} // namespace diff

bool operator==(const TextDiff::Diff & lhs, const TextDiff::Diff & rhs)
{
  return lhs.isInsertion() == rhs.isInsertion() && lhs.content == rhs.content;
}

} // namespace textedit
