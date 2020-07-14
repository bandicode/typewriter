// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_CONTRIBUTOR_H
#define TYPEWRITER_CONTRIBUTOR_H

#include "typewriter/textcursor.h"

#include <memory>
#include <vector>

namespace typewriter
{

class TYPEWRITER_API Contributor
{
private:
  std::string m_name;
  std::vector<std::unique_ptr<TextCursor>> m_cursors;

public:

  Contributor();
  Contributor(const Contributor&) = delete;
  ~Contributor();

  explicit Contributor(std::string name);

  const std::vector<std::unique_ptr<TextCursor>>& cursors() const;
  std::vector<TextCursor*> cursors(TextDocument* document) const;

  TextCursor* getCursor(TextDocument* document);

  void undo(TextDocument* document);
  void redo(TextDocument* document);

  void beginEdit(TextDocument* document);
  void endEdit(TextDocument* document);
};

} // namespace typewriter

#endif // !TYPEWRITER_CONTRIBUTOR_H
