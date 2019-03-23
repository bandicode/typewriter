// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include <QtTest/QtTest>

class TextEditTests : public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();
  void TestTextDiff();
};
