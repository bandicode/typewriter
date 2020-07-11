// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_TEXTEDITOR_H
#define TEXTEDIT_TEXTEDITOR_H

//#include "textedit/textview.h"
//
//#include <QWidget>
//
//namespace textedit
//{
//
//class TextEditorImpl;
//
//class TEXTEDIT_API TextEditor : public TextView
//{
//  Q_OBJECT
//public:
//  TextEditor();
//  explicit TextEditor(TextDocument *doc);
//  ~TextEditor();
//
//  const TextCursor & cursor() const;
//
//protected:
//  void mousePressEvent(QMouseEvent *e) override;
//  void mouseMoveEvent(QMouseEvent *e) override;
//  void mouseReleaseEvent(QMouseEvent *e) override;
//  void mouseDoubleClickEvent(QMouseEvent *e) override;
//
//  void paintEvent(QPaintEvent *e) override;
//
//  void keyPressEvent(QKeyEvent *e) override;
//  void keyReleaseEvent(QKeyEvent *e) override;
//
//  void timerEvent(QTimerEvent *e) override;
//
//  void showEvent(QShowEvent *e) override;
//
//protected:
//  void drawCursor(QPainter *painter, const TextCursor & c);
//  void drawSelection(QPainter *painter, TextBlock block, const Position & begin, const Position & end);
//
//protected:
//  TextEditorImpl* d_func() const;
//};
//
//} // namespace textedit

#endif // !TEXTEDIT_TEXTEDITOR_H
