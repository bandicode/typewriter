// Copyright (C) 2018 Vincent Chambrin
// This file is part of the richui library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "tests.h"

#include "textedit/textdiff.h"

using namespace textedit;

inline static Position pos(int n)
{
  return Position{ 0, n };
}

void TextEditTests::initTestCase()
{

}

void TextEditTests::TestTextDiff()
{
  QString text = "01234567890123456789";

  TextDiff td;
  td << diff::insert(pos(2), QString("a"));
  td << diff::insert(pos(3), QString("b"));

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::insert(pos(2), QString("ab")));

  td << diff::remove(pos(2), QString("ab23"));
  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("23")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("abef"));
  td << diff::insert(pos(4), QString("cd"));

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::insert(pos(2), QString("abcdef")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("ab"));
  td << diff::insert(pos(4+2), QString("cd"));
  td << diff::insert(pos(2), QString("ab"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::insert(pos(2), QString("abab")));
  QVERIFY(td.diffs().back() == diff::insert(pos(4), QString("cd")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("ab"));
  td << diff::remove(pos(2), QString("ab"));

  QVERIFY(td.diffs().isEmpty());

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), QString("23"));
  td << diff::insert(pos(2), QString("ab"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("23")));
  QVERIFY(td.diffs().back() == diff::insert(pos(4), QString("ab")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("ab"));
  td << diff::remove(pos(3), QString("b"));

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::insert(pos(2), QString("a")));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), QString("2"));
  td << diff::remove(pos(2), QString("3"));

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("23")));

  td << diff::remove(pos(1), QString("1"));

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::remove(pos(1), QString("123")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(1), QString("ab"));
  td << diff::remove(pos(4 + 2), QString("45"));
  td << diff::remove(pos(2 + 2), QString("2367"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::insert(pos(1), QString("ab")));
  QVERIFY(td.diffs().back() == diff::remove(pos(2), QString("234567")));
  
  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("a"));
  td << diff::insert(pos(5), QString("b"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().back().begin() == pos(4));

  td << diff::remove(pos(1), "1a23b4");
  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::remove(pos(1), QString("1234")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("abcd"));
  td << diff::remove(pos(1), QString("1ab"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::remove(pos(1), QString("1")));
  QVERIFY(td.diffs().back() == diff::insert(pos(2), QString("cd")));

  /******/

  td = TextDiff();
  td << diff::insert(pos(2), QString("abcd"));
  td << diff::remove(pos(4), QString("cd23"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("23")));
  QVERIFY(td.diffs().back() == diff::insert(pos(4), QString("ab")));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), QString("23456"));
  td << diff::insert(pos(2), QString("234"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("23456")));
  QVERIFY(td.diffs().back() == diff::insert(pos(7), QString("234")));

  td.simplify();

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::remove(pos(5), QString("56")));

  /******/

  td = TextDiff();
  td << diff::remove(pos(2), QString("234"));
  td << diff::insert(pos(2), QString("23456"));

  QVERIFY(td.diffs().size() == 2);
  QVERIFY(td.diffs().front() == diff::remove(pos(2), QString("234")));
  QVERIFY(td.diffs().back() == diff::insert(pos(5), QString("23456")));

  td.simplify();

  QVERIFY(td.diffs().size() == 1);
  QVERIFY(td.diffs().front() == diff::insert(pos(5), QString("56")));
}
