// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/syntaxhighlighter.h"
#include "textedit/private/syntaxhighlighter_p.h"

namespace textedit
{

SyntaxHighlighter::SyntaxHighlighter(QObject *parent)
  : QObject(parent)
  , d(new SyntaxHighlighterImpl)
{

}

SyntaxHighlighter::~SyntaxHighlighter()
{

}

void SyntaxHighlighter::highlightBlock(const QString & text)
{
  
}

bool SyntaxHighlighter::usesBlockState() const
{
  return false;
}

TextBlock SyntaxHighlighter::currentBlock() const
{
  return d->block.block();
}

int SyntaxHighlighter::currentBlockState() const
{
  return d->block.userState();
}

void SyntaxHighlighter::setCurrentBlockState(int state)
{
  d->block.impl().userstate = state;
}

int SyntaxHighlighter::previousBlockState() const
{
  return d->block.previous().userState();
}

void SyntaxHighlighter::createFoldPoint(int pos, int kind)
{
  // Insert into 'folds' while maintaining the list ordered
  auto & folds = d->block.impl().folds;

  auto it = folds.end();
  while (it != folds.begin())
  {
    auto prev_it = std::prev(it);
    if (pos > prev_it->pos)
    {
      folds.insert(it, view::FoldPosition{ pos, kind });
      return;
    }
    else
    {
      it = prev_it;
    }
  }

  folds.insert(it, view::FoldPosition{ pos, kind });
}

void SyntaxHighlighter::setFormat(int start, int count, const TextFormat & fmt)
{
  // Insert into 'formats' while maintaining the list ordered
  auto & formats = d->block.impl().formats;

  auto it = formats.end();
  while (it != formats.begin())
  {
    auto prev_it = std::prev(it);
    if (start > prev_it->start)
    {
      formats.insert(it, view::FormatRange{ fmt, start, count });
      return;
    }
    else
    {
      it = prev_it;
    }
  }

  formats.insert(it, view::FormatRange{ fmt, start, count });
}

} // namespace textedit
