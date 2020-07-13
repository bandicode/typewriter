// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "typewriter/textblock.h"
#include "typewriter/textcursor.h"
#include "typewriter/textdocument.h"

using namespace typewriter;

TEST_CASE("A document can be constructed from a string", "[document]")
{
  TextDocument document{
    "This is a simple document.\n"
    "It spans over 2 lines."
  };

  REQUIRE(document.lineCount() == 2);

  REQUIRE(document.toString() == "This is a simple document.\nIt spans over 2 lines.");
}

TEST_CASE("TextBlock can be used to iterate over the document", "[document]")
{
  TextDocument document{
  "ABC"
  };

  auto it = document.firstBlock().begin();
  auto end = document.firstBlock().end();

  REQUIRE(it.current() == 'A');

  ++it;

  REQUIRE(it.current() == 'B');

  ++it;

  REQUIRE(it.current() == 'C');

  ++it;

  REQUIRE(it == end);
}


TEST_CASE("Cursors can be used to edit a document", "[document]")
{
  TextDocument document;

  REQUIRE(document.lineCount() == 1);
  REQUIRE(document.firstBlock() == document.lastBlock());

  TextCursor cursor{ &document };

  cursor.insertText("Hello World!");

  REQUIRE(document.lineCount() == 1);
  REQUIRE(document.toString() == "Hello World!");

  cursor.insertBlock();
  REQUIRE(cursor.position() == Position{ 1, 0 });
  cursor.insertChar('o');
  REQUIRE(cursor.position() == Position{ 1, 1 });

  cursor.movePosition(TextCursor::Left);
  REQUIRE(cursor.position() == Position{ 1, 0 });
  cursor.insertChar('G');

  REQUIRE(document.toString() == "Hello World!\nGo");
}
