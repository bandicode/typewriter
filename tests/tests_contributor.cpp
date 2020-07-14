// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "typewriter/textblock.h"
#include "typewriter/textcursor.h"
#include "typewriter/textdocument.h"
#include "typewriter/contributor.h"

using namespace typewriter;

TEST_CASE("Contributors ", "[contributor]")
{
  TextDocument document{
    "Hello !"
  };

  Contributor bob{ "Bob" };

  TextCursor* cursor = bob.getCursor(&document);

  bob.beginEdit(&document);

  cursor->setPosition(Position{ 0, 6 });
  cursor->insertText("World");

  REQUIRE(document.text(0) == "Hello World!");

  bob.endEdit(&document);

  REQUIRE_THROWS(cursor->undo());

  bob.undo(&document);

  REQUIRE(document.text(0) == "Hello !");

  bob.redo(&document);

  REQUIRE(document.text(0) == "Hello World!");
}
