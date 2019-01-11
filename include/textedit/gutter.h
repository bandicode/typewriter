// Copyright (C) 2018 Vincent Chambrin
// This file is part of the textedit library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TEXTEDIT_GUTTER_H
#define TEXTEDIT_GUTTER_H

#include "textedit/textblock.h"

#include <QWidget>

namespace textedit
{

namespace view
{
class Block;
struct Metrics;
} // namespace view

class TextView;

class GutterImpl;

class TEXTEDIT_API Gutter : public QWidget
{
  Q_OBJECT
public:
  Gutter(QWidget *parent = nullptr);
  virtual ~Gutter();

  TextView* view() const;
  const TextDocument* document() const;

  inline GutterImpl* impl() { return d.get(); }

protected:
  QSize sizeHint() const override;

  void paintEvent(QPaintEvent *e);

  int columnCount() const;
  void writeNumber(QString & str, int n);

protected:
  friend class TextView;

  const view::Metrics & metrics() const;
  view::Block firstVisibleLine() const;

private:
  std::unique_ptr<GutterImpl> d;
};

} // namespace textedit

#endif // !TEXTEDIT_GUTTER_H
