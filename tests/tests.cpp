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

void TextEditTests::TestTextDiffComplex()
{
  QString text = "01a2b3c459012345";

  TextDiff td1;
  td1 << diff::insert(pos(2), QString("a"));
  td1 << diff::insert(pos(4), QString("b"));
  td1 << diff::insert(pos(6), QString("c"));
  td1 << diff::remove(pos(8), QString("678"));
  td1 << diff::insert(pos(9), QString("d"));

  QVERIFY(td1.diffs().size() == 5);
  QVERIFY(td1.diffs().front() == diff::insert(pos(2), QString("a")));
  QVERIFY(td1.diffs().at(1) == diff::insert(pos(3), QString("b")));
  QVERIFY(td1.diffs().at(2) == diff::insert(pos(4), QString("c")));
  QVERIFY(td1.diffs().at(3) == diff::remove(pos(5), QString("678")));
  QVERIFY(td1.diffs().at(4) == diff::insert(pos(9), QString("d")));

  TextDiff::Diff elem = td1.takeFirst();
  QVERIFY(elem == diff::insert(pos(2), QString("a")));
  QVERIFY(td1.diffs().size() == 4);
  QVERIFY(td1.diffs().front() == diff::insert(pos(4), QString("b")));
  QVERIFY(td1.diffs().at(1) == diff::insert(pos(5), QString("c")));
  QVERIFY(td1.diffs().at(2) == diff::remove(pos(6), QString("678")));
  QVERIFY(td1.diffs().at(3) == diff::insert(pos(10), QString("d")));

  elem = td1.takeFirst();
  QVERIFY(elem == diff::insert(pos(4), QString("b")));
  QVERIFY(td1.diffs().size() == 3);
  QVERIFY(td1.diffs().front() == diff::insert(pos(6), QString("c")));
  QVERIFY(td1.diffs().at(1) == diff::remove(pos(7), QString("678")));
  QVERIFY(td1.diffs().at(2) == diff::insert(pos(11), QString("d")));

  elem = td1.takeFirst();
  QVERIFY(elem == diff::insert(pos(6), QString("c")));
  QVERIFY(td1.diffs().size() == 2);
  QVERIFY(td1.diffs().at(0) == diff::remove(pos(8), QString("678")));
  QVERIFY(td1.diffs().at(1) == diff::insert(pos(12), QString("d")));


  elem = td1.takeFirst();
  QVERIFY(elem == diff::remove(pos(8), QString("678")));
  QVERIFY(td1.diffs().size() == 1);
  QVERIFY(td1.diffs().at(0) == diff::insert(pos(9), QString("d")));
}
