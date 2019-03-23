// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTDOCUMENT_H
#define TEXTEDIT_TEXTDOCUMENT_H

#include "textedit/textedit.h"

#include <QObject>

namespace textedit
{

class TextBlock;
class TextCursor;
class TextDiff;

class TextDocumentImpl;

class TEXTEDIT_API TextDocument : public QObject
{
  Q_OBJECT
public:
  TextDocument(QObject *parent = nullptr);
  TextDocument(const QString & text, QObject *parent = nullptr);
  ~TextDocument();

  const QString & text(int line) const;
  QString toString() const;
  int lineCount() const;

  TextBlock firstBlock() const;
  TextBlock lastBlock() const;
  TextBlock findBlockByNumber(int num) const;

  /// TODO: validate this candidate interface 
  void apply(const TextDiff & diff);
  int availableUndoSteps() const;
  inline bool isUndoAvailable() const { return availableUndoSteps() > 0; }
  int availableRedoSteps() const;
  inline bool isRedoAvailable() const { return availableRedoSteps() > 0; }
  void undo();
  void redo();

  static void updatePositionOnInsert(Position & pos, const Position & insertpos, const TextBlock & newblock);
  static void updatePositionOnBlockDestroyed(Position & pos, int linenum, const TextBlock & block);
  static void updatePositionOnContentsChange(Position & pos, const TextBlock & block, const Position & editpos, int charsRemoved, int charsAdded);

Q_SIGNALS:
  void blockInserted(const Position & pos, const TextBlock & newblock);
  void blockCountChanged(int newBlockCount);
  void blockDestroyed(int line, const TextBlock & block);
  void contentsChange(const TextBlock & block, const Position & pos, int charsRemoved, int charsAdded);
  void contentsChanged();

public:
  inline TextDocumentImpl * impl() const { return d.get(); }

private:
  std::unique_ptr<TextDocumentImpl> d;
};

} // namespace textedit

#endif // !TEXTEDIT_TEXTDOCUMENT_H
