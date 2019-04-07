// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/texteditor.h"

#include "textedit/textblock.h"
#include "textedit/view/metrics.h"
#include "textedit/private/texteditor_p.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <QPainter>

namespace textedit
{

TextEditorImpl::TextEditorImpl(TextDocument *doc)
  : TextViewImpl(doc),
  cursor(doc),
  timer(-1),
  cursorblink(true)
{

}

TextEditor::TextEditor()
  : TextView(std::unique_ptr<TextViewImpl>(new TextEditorImpl(new TextDocument)))
{
  document()->setParent(this);
}

TextEditor::TextEditor(TextDocument *doc)
  : TextView(std::unique_ptr<TextViewImpl>(new TextEditorImpl(doc)))
{

}

TextEditor::~TextEditor()
{

}

const TextCursor & TextEditor::cursor() const
{
  return d_func()->cursor;
}

void TextEditor::mousePressEvent(QMouseEvent *e)
{
  d_func()->cursor.setPosition(hitTest(e->pos()));
  d_func()->cursorblink = true;
  /// TODO: scroll if cursor not fully visible
  update();
}

void TextEditor::mouseMoveEvent(QMouseEvent *e)
{
  auto pos = d_func()->cursor.position();
  d_func()->cursor.setPosition(hitTest(e->pos()), TextCursor::KeepAnchor);
  if (d_func()->cursor.position() != pos)
    update();
}

void TextEditor::mouseReleaseEvent(QMouseEvent *e)
{
  /// TODO
}

void TextEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
  /// TODO
}

void TextEditor::paintEvent(QPaintEvent *e)
{
  QPainter painter{ this };

  TextView::setupPainter(&painter);
  TextView::paint(&painter);

  drawCursor(&painter, d_func()->cursor);
}

void TextEditor::keyPressEvent(QKeyEvent *e)
{
  const bool shift_modifier = e->modifiers() & Qt::ShiftModifier;
  const bool alt_modifier = e->modifiers() & Qt::AltModifier;

  switch (e->key())
  {
  case Qt::Key_Down:
    d_func()->cursor.movePosition(TextCursor::Down, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Left:
    d_func()->cursor.movePosition(TextCursor::Left, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Right:
    d_func()->cursor.movePosition(TextCursor::Right, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Up:
    d_func()->cursor.movePosition(TextCursor::Up, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Enter:
  case Qt::Key_Return:
    d_func()->cursor.insertBlock();
    break;
  case Qt::Key_Delete:
    d_func()->cursor.deleteChar();
    break;
  case Qt::Key_Backspace:
    d_func()->cursor.deletePreviousChar();
    break;
  default:
  {
    QString text = e->text();
    if (!text.isEmpty())
      d_func()->cursor.insertText(text);
  }
  break;
  }

  d_func()->cursorblink = true;
  update();
}

void TextEditor::keyReleaseEvent(QKeyEvent *e)
{
  /// TODO
}

void TextEditor::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == d_func()->timer)
  {
    d_func()->cursorblink = !d_func()->cursorblink;
    update();
  }

  TextView::timerEvent(e);
}

void TextEditor::showEvent(QShowEvent *e)
{
  if (d_func()->timer == -1)
  {
    d_func()->timer = startTimer(750);
  }

  TextView::showEvent(e);
}

static TextBlock selection_start_block(const TextCursor & c)
{
  if (c.position() == c.selectionStart())
    return c.block();
  return prev(c.block(), c.selectionEnd().line - c.selectionStart().line);
}

void TextEditor::drawCursor(QPainter *painter, const TextCursor & c)
{
  if (c.hasSelection())
    drawSelection(painter, selection_start_block(c), c.selectionStart(), c.selectionEnd());

  if (!d_func()->cursorblink)
    return;

  painter->setPen(QPen(Qt::black));

  QPoint pt = map(c.position());
  pt.ry() -= metrics().ascent;

  painter->drawLine(pt, pt + QPoint(0, metrics().lineheight));
}

void TextEditor::drawSelection(QPainter *painter, TextBlock block, const Position & begin, const Position & end)
{
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(QColor(100, 100, 255, 100)));

  QPoint pt = map(begin);
  pt.ry() -= metrics().ascent;

  if (begin.line == end.line)
  {
    QPoint endpt = map(end);
    endpt.ry() -= metrics().ascent;

    const int colcount = end.column - begin.column;
    painter->drawRect(QRect(pt, QSize(endpt.x() - pt.x(), metrics().lineheight)));
  }
  else
  {
    /// TODO: take tabulations into account

    // Drawing first line selection
    int colcount = 1 + block.length() - begin.column;
    painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));

    // Drawing middle lines
    pt = map(Position{ begin.line, 0 });
    pt.ry() -= metrics().ascent;
    const int linecount = end.line - begin.line - 1;
    for (int i(0); i < linecount; ++i)
    {
      block = block.next();
      pt.ry() += metrics().lineheight;
      colcount = 1 + block.length();
      painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));
    }

    // Drawing last line
    block = block.next();
    pt.ry() += metrics().lineheight;
    colcount = end.column;
    painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));
  }
}

TextEditorImpl* TextEditor::d_func() const
{
  return reinterpret_cast<TextEditorImpl*>(d.get());
}

} // namespace textedit
