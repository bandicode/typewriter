// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"


#include "typewriter/textblock.h"
#include "typewriter/textcursor.h"
#include "typewriter/textview.h"

#include <chrono>
#include <fstream>
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

  REQUIRE(view.height() == 2);
  REQUIRE(view.width() == 26);

  TextCursor cursor{ &document };
  cursor.setPosition(Position{ 0, 5 });
  cursor.setPosition(Position{ 1, 6 }, TextCursor::KeepAnchor);

  view.addFold(1, cursor);

  REQUIRE(view.height() == 1);
  REQUIRE(view.width() == 24); // 5 + 16 + 3(fold)

  cursor.setPosition(Position{ 1, 7 });
  cursor.insertChar('*');

  REQUIRE(view.height() == 1);
  REQUIRE(view.width() == 25);

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
  REQUIRE(lines.front().elements.at(1).kind == view::LineElement::LE_InlineInsert); // text + insert + text

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

  std::vector<view::Line> lines{ view.lines().begin(), view.lines().end() };

  REQUIRE(view.height() == 5);
  REQUIRE(view.width() == 7);
  REQUIRE(lines.at(0).displayedText() == "This is");
  REQUIRE(lines.at(1).displayedText() == "a ");
  REQUIRE(lines.at(2).displayedText() == "simple ");
  REQUIRE(lines.at(3).displayedText() == "documen");
  REQUIRE(lines.at(4).displayedText() == "t.");

  view.setCharactersPerLine(10);

  lines = std::vector<view::Line>(view.lines().begin(), view.lines().end());

  REQUIRE(view.height() == 3);
  REQUIRE(view.width() == 10);
  REQUIRE(lines.at(0).displayedText() == "This is a ");
  REQUIRE(lines.at(1).displayedText() == "simple ");
  REQUIRE(lines.at(2).displayedText() == "document.");
}

TEST_CASE("TextView supports tabs", "[view]")
{
  TextDocument document{
    "\ta\tab\tabc\tabcd\ta"
  };

  TextView view{ &document };
  view.setTabSize(4);

  REQUIRE(view.height() == 1);
  REQUIRE(view.width() == 25);
}

TEST_CASE("TextView can handle catch.hpp", "[view-bench]")
{
  std::string content;
  
  {
    std::ifstream file{ "catch.hpp" };
    std::string line;

    while (!file.eof())
    {
      std::getline(file, line);
      content += line;
      content.push_back('\n');
    }

    content.pop_back();
  }

  auto start = std::chrono::high_resolution_clock::now();

  TextDocument document{ content };

  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "TextDocument constructor on catch.hpp " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

  REQUIRE(document.lineCount() == 17619);

  start = std::chrono::high_resolution_clock::now();

  TextView view{ &document };

  end = std::chrono::high_resolution_clock::now();

  std::cout << "TextView constructor on catch.hpp " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

  REQUIRE(view.height() == document.lineCount());
}
