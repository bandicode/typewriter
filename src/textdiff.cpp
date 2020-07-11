// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/textdiff.h"

#include <algorithm>
#include <cassert>

namespace typewriter
{

TextRange::TextRange(const std::string& text, const Position& start) :
  m_range(start, TextRange::end(start, text)),
  m_text(text)
{

}

const Position & TextRange::begin() const
{
  return m_range.begin();
}

const Position & TextRange::end() const
{
  return m_range.end();
}

void TextRange::move(const Position & start)
{
  m_range.move(start);
}

Position TextRange::end(const Position & start, const std::string& text)
{
  const int line_count = std::count(text.begin(), text.end(), '\n');

  if (line_count == 0)
    return Position{ start.line, start.column + static_cast<int>(text.size()) };

  const int last_lf = text.find_last_of('\n');
  return Position{ start.line + line_count, static_cast<int>(text.length()) - 1 - last_lf };
}

TextRange& TextRange::operator+=(const TextRange & other)
{
  if (other.begin() < begin() || other.begin() > end())
    return *this;

  const int insert_pos = seek(other.begin());

  m_text.insert(insert_pos, other.text());

  /// TODO: compute this more efficiently
  m_range = Range(begin(), TextRange::end(begin(), m_text));

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

  m_text.erase(erase_begin, erase_end - erase_begin);

  /// TODO: compute this more efficiently
  m_range = Range(begin(), TextRange::end(begin(), m_text));

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
    char c = m_text.at(hint.index);
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
    const char c = m_text.at(i++);

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
  content = TextRange(std::string(), content.begin());
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

TextDiff::Diff TextDiff::takeFirst()
{
  Diff ret = std::move(mDiffs.front());
  mDiffs.erase(mDiffs.begin());
  ret.kind = ret.isInsertion() ? Removal : Insertion;
  for (auto & d : mDiffs)
  {
    d.mapTo(ret);
  }
  ret.kind = ret.isInsertion() ? Removal : Insertion;
  return ret;
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
    ins.content = TextRange(ins.text().substr(nb_common), ins.begin());
    rm.content = TextRange(rm.text().substr(nb_common), new_begin);

    if (ins.isEmpty())
      mDiffs.erase(mDiffs.begin() + i + 1);

    if (rm.isEmpty())
      mDiffs.erase(mDiffs.begin() + i);
  }
}

TextDiff& TextDiff::operator<<(Diff d)
{
  if (mDiffs.empty())
  {
    mDiffs.push_back(std::move(d));
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
  while (!temp.diffs().empty())
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
      mDiffs.insert(mDiffs.begin() + index, d);
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

  mDiffs.push_back(d);
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
      mDiffs.erase(mDiffs.begin() + i);
      --i;
    }
    else
    {
      if (rm.begin() < mDiffs.at(i).begin())
      {
        if (!rm.isEmpty() && !inserted)
        {
          mDiffs.insert(mDiffs.begin() + i, rm);
          ++i;
        }

        inserted = true;
      }
    }
  }

  if (!inserted && !rm.isEmpty())
  {
    mDiffs.push_back(rm);
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

TextDiff::Diff insert(const Position& pos, const std::string& text)
{
  return TextDiff::Diff{ TextDiff::Insertion, TextRange{ text, pos } };
}

TextDiff::Diff remove(const Position& pos, const std::string& text)
{
  return TextDiff::Diff{ TextDiff::Removal, TextRange{ text, pos } };
}

} // namespace diff

bool operator==(const TextDiff::Diff& lhs, const TextDiff::Diff& rhs)
{
  return lhs.isInsertion() == rhs.isInsertion() && lhs.content == rhs.content;
}

} // namespace typewriter
