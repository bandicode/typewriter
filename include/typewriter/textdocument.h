// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_TEXTDOCUMENT_H
#define TYPEWRITER_TEXTDOCUMENT_H

#include "typewriter/typewriter-defs.h"

#include <memory>
#include <string>

namespace typewriter
{

class TextBlock;
class TextCursor;
class TextDiff;

class TextDocumentImpl;

class TextDocument;

namespace str_utils
{

void replace_all(std::string& str, char c, const std::string& repl);

} // namespace str_utils

class TYPEWRITER_API TextDocumentListener
{
private:
  friend class TextDocument;
  TextDocument* m_document = nullptr;

public:
  TextDocumentListener();
  virtual ~TextDocumentListener();

  TextDocument* document() const;

  /*!
   * \fn virtual void blockInserted(const Position& pos, const TextBlock& newblock);
   * \param position in the previous block
   * \param newly inserted block
   * \brief notifies that a new block was inserted
   */
  virtual void blockInserted(const Position& pos, const TextBlock& newblock);
  virtual void blockCountChanged(int newBlockCount);
  virtual void blockDestroyed(int line, const TextBlock& block);
  virtual void contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded);
  virtual void contentsChanged();
};

class TYPEWRITER_API TextDocument
{
public:
  TextDocument();
  explicit TextDocument(const std::string& text);
  ~TextDocument();

  const std::string& text(int line) const;
  std::string toString() const;
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

  void addListener(TextDocumentListener* listener);
  void removeListener(TextDocumentListener* listener);

public:
  inline TextDocumentImpl * impl() const { return d.get(); }

private:
  std::unique_ptr<TextDocumentImpl> d;
};

} // namespace typewriter

#endif // !TYPEWRITER_TEXTDOCUMENT_H
