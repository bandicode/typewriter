// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_SYNTAXHIGHLIGHTER_H
#define TEXTEDIT_SYNTAXHIGHLIGHTER_H

#include "textedit/textblock.h"
#include "textedit/textformat.h"

#include <QObject>

namespace textedit
{

class SyntaxHighlighterImpl;

class TEXTEDIT_API SyntaxHighlighter : public QObject
{
  Q_OBJECT
public:
  SyntaxHighlighter(QObject *parent = nullptr);
  virtual ~SyntaxHighlighter();

  enum FoldKind {
    NoFold = 0,
    LeftParFold = 1,
    RightParFold = -1,
    LeftBraceFold = 2,
    RightBraceFold = -2,
    LeftBracketFold = 3,
    RightBracketFold = -3,
  };

  virtual void highlightBlock(const QString & text);
  virtual bool usesBlockState() const;

  inline SyntaxHighlighterImpl* impl() { return d.get(); }

protected:
  TextBlock currentBlock() const;
  int currentBlockState() const;
  void setCurrentBlockState(int state);
  int previousBlockState() const;

  void createFoldPoint(int pos, int kind);
  void setFormat(int start, int count, const TextFormat & fmt);

private:
  std::unique_ptr<SyntaxHighlighterImpl> d;
};

} // namespace textedit

#endif // !TEXTEDIT_SYNTAXHIGHLIGHTER_H
