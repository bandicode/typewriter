// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"


#include "typewriter/textblock.h"
#include "typewriter/textcursor.h"
#include "typewriter/textview.h"

#include <iostream>

using namespace typewriter;

TEST_CASE("A view can be constructed from a document", "[view]")
{
  TextDocument document{
    "This is a simple document.\n"
    "It spans over 2 lines."
  };

  REQUIRE(document.lineCount() == 2);

  TextView view{ &document };

  REQUIRE(view.height() == 2);
}

TEST_CASE("Part of a view can be hidden with folds", "[view]")
{
  TextDocument document{
    "This is a simple document.\n"
    "It spans over 2 lines."
  };

  TextView view{ &document };

  TextCursor cursor{ &document };
  cursor.setPosition(Position{ 0, 5 });
  cursor.setPosition(Position{ 1, 6 }, TextCursor::KeepAnchor);

  view.addFold(1, cursor);

  REQUIRE(view.height() == 1);

  cursor.setPosition(Position{ 1, 7 });
  cursor.insertChar('*');

  REQUIRE(view.height() == 1);

  const auto& lines = view.lines();
  REQUIRE(lines.front().elements.size() == 3); // text + fold + text

  view.removeFold(1);

  REQUIRE(view.height() == 2);
}

TEST_CASE("Info can be inserted into a view with inserts", "[view]")
{
  TextDocument document{
    "This is a simple document.\n"
    "It spans over 2 lines."
  };

  TextView view{ &document };

  TextCursor cursor{ &document };
  cursor.setPosition(Position{ 0, 5 });

  view::InlineInsert ins;
  ins.cursor = cursor;
  ins.span = 3;

  view.addInlineInsert(ins);

  REQUIRE(view.height() == 2);

  const auto& lines = view.lines();
  REQUIRE(lines.front().elements.size() == 3); // text + insert + text
  REQUIRE(lines.front().elements.at(1).kind == view::SimpleLineElement::LE_InlineInsert); // text + insert + text

  view.clearInserts();

  REQUIRE(lines.front().elements.size() == 1); // text
}

TEST_CASE("TextView supports word-wrap", "[view]")
{
  TextDocument document{
    "This is a simple document."
  };

  TextView view{ &document };

  REQUIRE(view.height() == 1);

  view.setWrapMode(TextView::WrapMode::Word);
  view.setCharactersPerLine(7);

  std::vector<view::LineInfo> lines{ view.lines().begin(), view.lines().end() };

  REQUIRE(view.height() == 5);
  REQUIRE(lines.at(0).displayedText() == "This is");
  REQUIRE(lines.at(1).displayedText() == "a ");
  REQUIRE(lines.at(2).displayedText() == "simple ");
  REQUIRE(lines.at(3).displayedText() == "documen");
  REQUIRE(lines.at(4).displayedText() == "t.");

  view.setCharactersPerLine(10);

  lines = std::vector<view::LineInfo>(view.lines().begin(), view.lines().end());

  REQUIRE(view.height() == 3);
  REQUIRE(lines.at(0).displayedText() == "This is a ");
  REQUIRE(lines.at(1).displayedText() == "simple ");
  REQUIRE(lines.at(2).displayedText() == "document.");
}
