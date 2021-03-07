// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_SYNTAXHIGHLIGHTER_H
#define TYPEWRITER_SYNTAXHIGHLIGHTER_H

#include "typewriter/textview.h"
#include "typewriter/view/formatrange.h"

namespace typewriter
{

class TYPEWRITER_API SyntaxHighlighter
{
private:
  TextView& m_view;
  TextBlock m_current_block;
  std::shared_ptr<view::Block> m_current_block_view;
  int m_current_line = -1;

public:
  explicit SyntaxHighlighter(TextView& v);
  ~SyntaxHighlighter();

  void clear();
  void clear(TextBlock block);

  TextBlock currentBlock() const;
  int currentLine() const;
  void rehighlight(TextBlock block);
  void rehighlightNextBlock();

  void setFormat(int start, int length, int format);
  void setFormat(int line, int start, int length, int format);

  void setBlockFormat(int format);
  void setBlockFormat(int line, int format);

  int blockState() const;
  void setBlockState(int state);
  void resetBlockState();
  int previousBlockState() const;

protected:
  void seekLine(int l);
};

} // namespace typewriter

#endif // !TYPEWRITER_SYNTAXHIGHLIGHTER_H
