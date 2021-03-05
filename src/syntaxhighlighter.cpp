// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/syntaxhighlighter.h"
#include "typewriter/private/textview_p.h"

#include <algorithm>
#include <cassert>

namespace typewriter
{

SyntaxHighlighter::SyntaxHighlighter(TextView& v)
  : m_view(v)
{

}

SyntaxHighlighter::~SyntaxHighlighter()
{

}

void SyntaxHighlighter::clear()
{
  for (const auto& e : m_view.blocks())
    e.second->formats.clear();
}

void SyntaxHighlighter::clear(TextBlock block)
{
  auto it = m_view.blocks().find(block.impl());

  if (it != m_view.blocks().end())
    it->second->formats.clear();
}

TextBlock SyntaxHighlighter::currentBlock() const
{
  return m_current_block;
}

int SyntaxHighlighter::currentLine() const
{
  return m_current_line;
}

void SyntaxHighlighter::rehighlight(TextBlock block)
{
  auto it = m_view.blocks().find(block.impl());

  if (it != m_view.blocks().end())
  {
    m_current_block = block;
    m_current_line = block.blockNumber();
    m_current_block_view = it->second;
    m_current_block_view->formats.clear();
    m_current_block_view->blockformat = 0;
  }
}

void SyntaxHighlighter::setFormat(int start, int length, int format)
{
  view::FormatRange fr;
  fr.format_id = format;
  fr.length = length;
  fr.start = start;

  std::vector<view::FormatRange>& formats = m_current_block_view->formats;

  if (formats.empty() || formats.back().start < start)
  {
    formats.push_back(fr);
  }
  else
  {
    auto comp = [](const view::FormatRange& lhs, const int& rhs) -> bool {
      return lhs.start < rhs;
    };

    auto it = std::lower_bound(formats.begin(), formats.end(), start, comp);
    formats.insert(it, fr);
  }
}

void SyntaxHighlighter::setFormat(int line, int start, int length, int format)
{
  seekLine(line);
  setFormat(start, length, format);
}

void SyntaxHighlighter::seekLine(int l)
{
  if (l != currentLine())
  {
    if (currentLine() == -1)
    {
      m_current_block = m_view.document()->findBlockByNumber(l);
    }
    else if (l < currentLine())
    {
      m_current_block = typewriter::prev(m_current_block, currentLine() - l);
    }
    else
    {
      m_current_block = typewriter::next(m_current_block, l - currentLine());
    }

    m_current_line = l;
    auto it = m_view.blocks().find(m_current_block.impl());
    m_current_block_view = it->second;
    m_current_block_view->formats.clear();
  }
}

void SyntaxHighlighter::setBlockFormat(int format)
{
  m_current_block_view->blockformat = format;
}

void SyntaxHighlighter::setBlockFormat(int line, int format)
{
  seekLine(line);
  setBlockFormat(format);
}

} // namespace typewriter
