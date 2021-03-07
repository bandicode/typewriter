// Copyright (C) 2018-2021 Vincent Chambrin
// This file is part of the typeset project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <QApplication>

#include "typewriter/textdocument.h"
#include "typewriter/qt/codeeditor-widget.h"

#include <QMouseEvent>
#include <QVBoxLayout>

//class CustomSyntaxHighlighter : public textedit::SyntaxHighlighter
//{
//public:
//
//  static QSet<QString> getKeywords()
//  {
//    QSet<QString> result;
//    result << "bool" << "char" << "int" << "return";
//    return result;
//  }
//
//  static textedit::TextFormat keywordFormat()
//  {
//    textedit::TextFormat fmt;
//    fmt.setTextColor(QColor(100, 100, 255));
//    return fmt;
//  }
//
//  static textedit::TextFormat underlineFormat()
//  {
//    textedit::TextFormat fmt;
//    fmt.setUnderlineStyle(textedit::TextFormat::DashDotLine);
//    fmt.setUnderlineColor(Qt::red);
//    return fmt;
//  }
//
//  static textedit::TextFormat waveUnderlineFormat()
//  {
//    textedit::TextFormat fmt;
//    fmt.setUnderlineStyle(textedit::TextFormat::WaveUnderline);
//    fmt.setUnderlineColor(Qt::green);
//    return fmt;
//  }
//
//  static textedit::TextFormat strikeOutFormat()
//  {
//    textedit::TextFormat fmt;
//    fmt.setStrikeOut();
//    fmt.setStrikeOutColor(Qt::blue);
//    return fmt;
//  }
//
//  void highlightBlock(const QString & text) override
//  {
//    static const QSet<QString> keywords = getKeywords();
//
//    QStringList tokens = text.split(" ", QString::KeepEmptyParts);
//    int offset = 0;
//    for (auto t : tokens)
//    {
//      if (keywords.contains(t))
//        setFormat(offset, t.length(), keywordFormat());
//      else if (t == "underline")
//        setFormat(offset, t.length(), underlineFormat());
//      else if (t == "wave_underline")
//        setFormat(offset, t.length(), waveUnderlineFormat());
//      else if (t == "strikeout")
//        setFormat(offset, t.length(), strikeOutFormat());
//      offset += t.length() + 1;
//    }
//  }
//
//  bool usesBlockState() const override
//  {
//    return false;
//  }
//};

void text_view()
{
  using namespace typewriter;

  std::string content = 
   "int underline = 0;\n"
   "int abs(int a)\n"
   "{\n"
   "  return a < 0 ? -a : a;\n"
   "}\n"
   "bool wave_underline = false;\n"
   "\n"
   "strikeout ();\n";

  TextDocument *doc = new TextDocument{ content };

  QTypewriter* widget = new QTypewriter(doc);

  //TextView *view = new TextView(doc);
  //doc->setParent(view);
  //view->setSyntaxHighlighter<CustomSyntaxHighlighter>();

  widget->resize(400, 200);
  widget->show();
}

//void text_editor()
//{
//  textedit::TextEditor *editor = new textedit::TextEditor();
//  editor->setSyntaxHighlighter<CustomSyntaxHighlighter>();
//  editor->show();
//}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //text_editor();
  text_view();

  return app.exec();
}
