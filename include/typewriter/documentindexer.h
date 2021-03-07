// Copyright (C) 2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_DOCUMENTINDEXER_H
#define TYPEWRITER_DOCUMENTINDEXER_H

#include "typewriter/textdocument.h"
#include "typewriter/textblock.h"

#include <unordered_map>

namespace typewriter
{

class TYPEWRITER_API DocumentIndexer
{
private:
  TextDocument* m_document = nullptr;
  std::unordered_map<TextBlockImpl*, int> m_linenumbers;

public:
  DocumentIndexer(TextDocument* doc);

  int linenum(const TextBlock& block) const;

protected:
  void reindex(TextDocument* doc);
};

inline DocumentIndexer::DocumentIndexer(TextDocument* doc)
  : m_document(doc)
{
  reindex(doc);
}

inline int DocumentIndexer::linenum(const TextBlock& block) const
{
  auto it = m_linenumbers.find(block.impl());
  return it != m_linenumbers.end() ? it->second : -1;
}

inline void DocumentIndexer::reindex(TextDocument* doc)
{
  int l = 0;

  TextBlock b = doc->firstBlock();

  while (b != doc->lastBlock())
  {
    m_linenumbers[b.impl()] = l++;
  }

  m_linenumbers[doc->lastBlock().impl()] = l;
}

} // namespace typewriter

#endif // !TYPEWRITER_DOCUMENTINDEXER_H
