// Copyright (C) 2018 Vincent Chambrin
// This file is part of the liblayout project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <QApplication>

#include "textedit/syntaxhighlighter.h"
#include "textedit/textdocument.h"
#include "textedit/textview.h"

#include <QMouseEvent>
#include <QVBoxLayout>

class CustomSyntaxHighlighter : public textedit::SyntaxHighlighter
{
public:

  static QSet<QString> getKeywords()
  {
    QSet<QString> result;
    result << "bool" << "char" << "int" << "return";
    return result;
  }

  static textedit::TextFormat keywordFormat()
  {
    textedit::TextFormat fmt;
    fmt.setTextColor(QColor(100, 100, 255));
    return fmt;
  }

  void highlightBlock(const QString & text) override
  {
    static const QSet<QString> keywords = getKeywords();

    QStringList tokens = text.split(" ", QString::KeepEmptyParts);
    int offset = 0;
    for (auto t : tokens)
    {
      if (keywords.contains(t))
        setFormat(offset, t.length(), keywordFormat());
      offset += t.length() + 1;
    }
  }

  bool usesBlockState() const override
  {
    return false;
  }
};

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  using namespace textedit;

  QStringList lines;
  lines << "int n = 0;"
    << "int abs(int a)"
    << "{"
    << "  return a < 0 ? -a : a;"
    << "}";

  TextDocument *doc = new TextDocument{ lines.join("\n") };

  TextView *view = new TextView(doc);
  doc->setParent(view);
  view->setSyntaxHighlighter<CustomSyntaxHighlighter>();

  view->resize(400, 200);
  view->show();

  return app.exec();
}

#include "main.moc"