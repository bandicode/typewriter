// Copyright (C) 2020 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "typewriter/qt/codeeditor-widget.h"

#include "typewriter/textblock.h"
#include "typewriter/view/fragment.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <QPainter>
#include <QPainterPath>

#include <QScrollBar>

#include <algorithm>
#include <stdexcept>

namespace typewriter
{

QTypewriterFontMetrics::QTypewriterFontMetrics(const QFont& f)
{
  QFontMetrics fm{ f };
  this->charwidth = fm.averageCharWidth();
  this->lineheight = fm.height();
  this->ascent = fm.ascent();
  this->descent = fm.descent();
  this->strikeoutpos = fm.strikeOutPos();
  this->underlinepos = fm.underlinePos();
}

namespace details
{

QTypewriterContext::QTypewriterContext(typewriter::TextDocument* doc)
  : document(doc),
    view(doc)
{
  this->formats.resize(16);
}

QTypewriterVisibleLines QTypewriterContext::visibleLines() const
{
  size_t count = availableHeight() / this->metrics.lineheight;
  return QTypewriterVisibleLines(this->view.lines(), this->first_visible_line, count);
}

} // namespace details

} // namespace typewriter
