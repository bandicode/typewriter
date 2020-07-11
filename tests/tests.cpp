// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "typewriter/textdiff.h"

using namespace typewriter;

inline static Position pos(int n)
{
  return Position{ 0, n };
}

TEST_CASE("Simple text diffs are working", "[textdiff]")
{
  std::string text = "01234567890123456789";

  TextDiff td;
  td << diff::insert(pos(2), std::string("a"));
  td << diff::insert(pos(3), std::string("b"));

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::insert(pos(2), std::string("ab"))));

  td << diff::remove(pos(2), std::string("ab23"));
  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("23"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("abef"));
  td << diff::insert(pos(4), std::string("cd"));

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::insert(pos(2), std::string("abcdef"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("ab"));
  td << diff::insert(pos(4 + 2), std::string("cd"));
  td << diff::insert(pos(2), std::string("ab"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::insert(pos(2), std::string("abab"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(4), std::string("cd"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("ab"));
  td << diff::remove(pos(2), std::string("ab"));

  REQUIRE(td.diffs().empty());

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), std::string("23"));
  td << diff::insert(pos(2), std::string("ab"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("23"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(4), std::string("ab"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("ab"));
  td << diff::remove(pos(3), std::string("b"));

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::insert(pos(2), std::string("a"))));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), std::string("2"));
  td << diff::remove(pos(2), std::string("3"));

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("23"))));

  td << diff::remove(pos(1), std::string("1"));

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::remove(pos(1), std::string("123"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(1), std::string("ab"));
  td << diff::remove(pos(4 + 2), std::string("45"));
  td << diff::remove(pos(2 + 2), std::string("2367"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::insert(pos(1), std::string("ab"))));
  REQUIRE((td.diffs().back() == diff::remove(pos(2), std::string("234567"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("a"));
  td << diff::insert(pos(5), std::string("b"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE(td.diffs().back().begin() == pos(4));

  td << diff::remove(pos(1), "1a23b4");
  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::remove(pos(1), std::string("1234"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("abcd"));
  td << diff::remove(pos(1), std::string("1ab"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::remove(pos(1), std::string("1"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(2), std::string("cd"))));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), std::string("abcd"));
  td << diff::remove(pos(4), std::string("cd23"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("23"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(4), std::string("ab"))));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), std::string("23456"));
  td << diff::insert(pos(2), std::string("234"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("23456"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(7), std::string("234"))));

  td.simplify();

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::remove(pos(5), std::string("56"))));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), std::string("234"));
  td << diff::insert(pos(2), std::string("23456"));

  REQUIRE(td.diffs().size() == 2);
  REQUIRE((td.diffs().front() == diff::remove(pos(2), std::string("234"))));
  REQUIRE((td.diffs().back() == diff::insert(pos(5), std::string("23456"))));

  td.simplify();

  REQUIRE(td.diffs().size() == 1);
  REQUIRE((td.diffs().front() == diff::insert(pos(5), std::string("56"))));
}

TEST_CASE("Complex text diffs are working", "[textdiff]")
{
  std::string text = "01a2b3c459012345";

  TextDiff td1;
  td1 << diff::insert(pos(2), std::string("a"));
  td1 << diff::insert(pos(4), std::string("b"));
  td1 << diff::insert(pos(6), std::string("c"));
  td1 << diff::remove(pos(8), std::string("678"));
  td1 << diff::insert(pos(9), std::string("d"));

  REQUIRE(td1.diffs().size() == 5);
  REQUIRE((td1.diffs().front() == diff::insert(pos(2), std::string("a"))));
  REQUIRE((td1.diffs().at(1) == diff::insert(pos(3), std::string("b"))));
  REQUIRE((td1.diffs().at(2) == diff::insert(pos(4), std::string("c"))));
  REQUIRE((td1.diffs().at(3) == diff::remove(pos(5), std::string("678"))));
  REQUIRE((td1.diffs().at(4) == diff::insert(pos(9), std::string("d"))));

  TextDiff::Diff elem = td1.takeFirst();
  REQUIRE((elem == diff::insert(pos(2), std::string("a"))));
  REQUIRE(td1.diffs().size() == 4);
  REQUIRE((td1.diffs().front() == diff::insert(pos(4), std::string("b"))));
  REQUIRE((td1.diffs().at(1) == diff::insert(pos(5), std::string("c"))));
  REQUIRE((td1.diffs().at(2) == diff::remove(pos(6), std::string("678"))));
  REQUIRE((td1.diffs().at(3) == diff::insert(pos(10), std::string("d"))));

  elem = td1.takeFirst();
  REQUIRE((elem == diff::insert(pos(4), std::string("b"))));
  REQUIRE(td1.diffs().size() == 3);
  REQUIRE((td1.diffs().front() == diff::insert(pos(6), std::string("c"))));
  REQUIRE((td1.diffs().at(1) == diff::remove(pos(7), std::string("678"))));
  REQUIRE((td1.diffs().at(2) == diff::insert(pos(11), std::string("d"))));

  elem = td1.takeFirst();
  REQUIRE((elem == diff::insert(pos(6), std::string("c"))));
  REQUIRE(td1.diffs().size() == 2);
  REQUIRE((td1.diffs().at(0) == diff::remove(pos(8), std::string("678"))));
  REQUIRE((td1.diffs().at(1) == diff::insert(pos(12), std::string("d"))));


  elem = td1.takeFirst();
  REQUIRE((elem == diff::remove(pos(8), std::string("678"))));
  REQUIRE(td1.diffs().size() == 1);
  REQUIRE((td1.diffs().at(0) == diff::insert(pos(9), std::string("d"))));
}
