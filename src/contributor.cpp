// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/contributor.h"

#include "typewriter/textdocument.h"
#include "typewriter/private/textdocument_p.h"

namespace typewriter
{

Contributor::Contributor()
{

}

Contributor::~Contributor()
{

}

Contributor::Contributor(std::string name)
  : m_name(std::move(name))
{

}

const std::vector<std::unique_ptr<TextCursor>>& Contributor::cursors() const
{
  return m_cursors;
}

std::vector<TextCursor*> Contributor::cursors(TextDocument* document) const
{
  std::vector<TextCursor*> r;

  for (const auto& c : cursors())
  {
    if (c->document() == document)
      r.push_back(c.get());
  }

  return r;
}

TextCursor* Contributor::getCursor(TextDocument* document)
{
  for (const auto& c : cursors())
  {
    if (c->document() == document)
      return c.get();
  }

  TextCursor* c = new TextCursor(document);
  m_cursors.push_back(std::unique_ptr<TextCursor>(c));
  return c;
}

void Contributor::undo(TextDocument* document)
{
  document->impl()->undo(this);
}

void Contributor::redo(TextDocument* document)
{
  document->impl()->redo(this);
}

void Contributor::beginEdit(TextDocument* document)
{
  document->impl()->beginTransaction(this);
}

void Contributor::endEdit(TextDocument* document)
{
  document->impl()->endTransaction(this);
}

} // namespace typewriter
