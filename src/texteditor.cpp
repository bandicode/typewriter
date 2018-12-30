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
  : cursor(doc)
  , timer(-1)
  , cursorblink(true)
{

}

TextEditor::TextEditor()
  : TextView(new TextDocument)
{
  d.reset(new TextEditorImpl{ const_cast<TextDocument*>(TextView::document()) });

  if (document()->parent() == nullptr)
    document()->setParent(this);
}

TextEditor::TextEditor(TextDocument *doc)
  : TextView(doc)
  , d(new TextEditorImpl{doc})
{

}

TextEditor::~TextEditor()
{

}

TextDocument* TextEditor::document() const
{
  return d->cursor.document();
}

const TextCursor & TextEditor::cursor() const
{
  return d->cursor;
}

void TextEditor::mousePressEvent(QMouseEvent *e)
{
  d->cursor.setPosition(hitTest(e->pos()));
  /// TODO: scroll if cursor not fully visible
  update();
}

void TextEditor::mouseMoveEvent(QMouseEvent *e)
{
  auto pos = d->cursor.position();
  d->cursor.setPosition(hitTest(e->pos()), TextCursor::KeepAnchor);
  if (d->cursor.position() != pos)
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

  drawCursor(&painter, d->cursor);
}

void TextEditor::keyPressEvent(QKeyEvent *e)
{
  const bool shift_modifier = e->modifiers() & Qt::ShiftModifier;
  const bool alt_modifier = e->modifiers() & Qt::AltModifier;

  switch (e->key())
  {
  case Qt::Key_Down:
    d->cursor.movePosition(TextCursor::Down, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Left:
    d->cursor.movePosition(TextCursor::Left, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Right:
    d->cursor.movePosition(TextCursor::Right, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Up:
    d->cursor.movePosition(TextCursor::Up, shift_modifier ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
    break;
  case Qt::Key_Enter:
  case Qt::Key_Return:
    d->cursor.insertBlock();
    break;
  case Qt::Key_Delete:
    d->cursor.deleteChar();
    break;
  case Qt::Key_Backspace:
    d->cursor.deletePreviousChar();
    break;
  default:
  {
    QString text = e->text();
    if (!text.isEmpty())
      d->cursor.insertText(text);
  }
  break;
  }

  d->cursorblink = true;
  update();
}

void TextEditor::keyReleaseEvent(QKeyEvent *e)
{
  /// TODO
}

void TextEditor::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == d->timer)
  {
    d->cursorblink = !d->cursorblink;
    update();
  }

  TextView::timerEvent(e);
}

void TextEditor::showEvent(QShowEvent *e)
{
  if (d->timer == -1)
  {
    d->timer = startTimer(750);
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

  if (!d->cursorblink)
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
    const int colcount = end.column - begin.column;
    painter->drawRect(QRect(pt, QSize(colcount * metrics().charwidth, metrics().lineheight)));
  }
  else
  {
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

} // namespace textedit
