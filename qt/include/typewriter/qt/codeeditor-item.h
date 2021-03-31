// Copyright (C) 2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_QML_H
#define TYPEWRITER_QTYPEWRITER_QML_H

#include "typewriter/qt/codeeditor-qt-common.h"

#include <QQuickPaintedItem>

namespace typewriter
{

class TYPEWRITER_QAPI QTypewriterGutterItem : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged)
  Q_PROPERTY(int minimumWidth READ minimumWidth NOTIFY minimumWidthChanged)
public:
  QTypewriterGutterItem(QQuickItem* parent = nullptr);
  QTypewriterGutterItem(QTypewriterView* context, QQuickItem* parent = nullptr);
  ~QTypewriterGutterItem();

  QTypewriterView* view() const;
  void setView(QObject* v);
  void setView(QTypewriterView* v);

  int minimumWidth() const;

  void addMarker(int line, MarkerType m);
  void clearMarkers();
  void removeMarkers(int line);
  void removeMarker(int line, MarkerType m);

Q_SIGNALS:
  void viewChanged();
  void minimumWidthChanged();
  void clicked(int line);

protected:
  int columnCount() const;

protected:
  void paint(QPainter* painter) override;
 
  void mousePressEvent(QMouseEvent* e) override;

protected:
  void drawMarkers(QPainter& painter, QPoint pt, int markers);
  
  void writeNumber(QString& str, int n);
  bool find_marker(int line, std::vector<Marker>::const_iterator& it) const;

private:
  QTypewriterView* d = nullptr;
  std::vector<Marker> m_markers;
};

class TYPEWRITER_QAPI QTypewriterItem : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged)
public:
  explicit QTypewriterItem(QQuickItem* parent = nullptr);
  ~QTypewriterItem();

  QTypewriterView* view() const;
  void setView(QObject* v);
  void setView(QTypewriterView* v);

  TextDocument* document() const;

  Q_INVOKABLE void scroll(int delta);

public Q_SLOTS:
  Q_INVOKABLE void setFirstVisibleLine(int n);

public:
  Position hitTest(const QPoint& pos) const;
  QPoint map(const Position& pos) const;
  bool isVisible(const Position& pos) const;

  const TextFormat& defaultFormat() const;
  void setDefaultFormat(const TextFormat& format);

  const TextFormat& textFormat(int id) const;
  void setTextFormat(int id, const TextFormat& fmt);

Q_SIGNALS:
  void viewChanged();

protected:
  void paint(QPainter* painter) override;
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

  void wheelEvent(QWheelEvent* e) override;

  void setupPainter(QPainter* painter);

  const QTypewriterFontMetrics& metrics() const;

private:
  void init();
  void requestUpdate();

protected:
  QTypewriterView* m_view = nullptr;
};

} // namespace typewriter

#endif // !TYPEWRITER_QTYPEWRITER_QML_H
