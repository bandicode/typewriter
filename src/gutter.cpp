// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "textedit/gutter.h"
#include "textedit/private/gutter_p.h"

#include "textedit/view/line.h"
#include "textedit/view/metrics.h"
#include "textedit/private/textview_p.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

namespace textedit
{

Gutter::Gutter(QWidget *parent)
  : QWidget(parent)
  , d(new GutterImpl)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

Gutter::~Gutter()
{

}

TextView* Gutter::view() const
{
  return d->view;
}

const TextDocument* Gutter::document() const
{
  return d->view->document();
}

const view::Metrics & Gutter::metrics() const
{
  return d->view->impl()->metrics;
}

view::Line Gutter::firstVisibleLine() const
{
  return d->view->impl()->firstLine;
}

QSize Gutter::sizeHint() const
{
  return QSize{ metrics().charwidth * columnCount() + 4, 66 };
}

void Gutter::paintEvent(QPaintEvent *e)
{
  QPainter painter{ this };

  painter.setBrush(QBrush(view()->defaultFormat().backgroundColor()));
  painter.setPen(Qt::NoPen);
  painter.drawRect(this->rect());

  painter.setFont(view()->font());
  painter.setPen(QPen(view()->defaultFormat().textColor()));

  painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());

  auto it = firstVisibleLine();
  QString label = QString(" ").repeated(columnCount());
  const int count = view()->visibleLineCount();

  for (int i(0); i < count; ++i)
  {
    writeNumber(label, it.number() + 1);

    painter.drawText(QPoint(0, i * metrics().lineheight + metrics().ascent), label);

    if (it.isLast())
      return;

    it = it.next();
  }
}

int Gutter::columnCount() const
{
  int n = document()->lineCount() + 1;
  int result = 1;
  while (n >= 10)
  {
    n /= 10;
    result += 1;
  }
  return result;
}

void Gutter::writeNumber(QString & str, int n)
{
  for (int i(1); i <= str.size(); ++i)
  {
    str[str.size() - i] = (n == 0 ? ' ' : '0' + (n % 10));
    n /= 10;
  }
}

} // namespace textedit
