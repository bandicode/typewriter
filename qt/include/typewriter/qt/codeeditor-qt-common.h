// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_COMMON_H
#define TYPEWRITER_QTYPEWRITER_COMMON_H

#include "typewriter/contributor.h"
#include "typewriter/textview.h"

#include <QObject>

#include <QColor>
#include <QFont>
#include <QPen>

#include <vector>

class QScrollBar;

namespace typewriter
{

struct QTypewriterFontMetrics
{
  int charwidth;
  int lineheight;
  int ascent;
  int descent;
  int underlinepos;
  int strikeoutpos;

  QTypewriterFontMetrics() = default;
  QTypewriterFontMetrics(const QTypewriterFontMetrics&) = default;

  explicit QTypewriterFontMetrics(const QFont& f);

  QTypewriterFontMetrics& operator=(const QTypewriterFontMetrics&) = default;
};

struct TextFormat
{

  enum UnderlineStyle {
    NoUnderline = 0,
    SingleUnderline = 1,
    DashUnderline = 2,
    DotLine = 3,
    DashDotLine = 4,
    DashDotDotLine = 5,
    WaveUnderline = 6,
  };

  bool bold = false;
  bool italic = false;
  bool strikeout = false;
  QColor strikeout_color = QColor();
  QColor text_color = QColor(0, 0, 0);
  QColor background_color = QColor(255, 255, 255);
  QColor foreground_color = QColor(0, 0, 0, 0);
  QPen border_pen;
  UnderlineStyle underline = NoUnderline;
  QColor underline_color = QColor();
};

namespace details
{

class QTypewriterVisibleLines
{
public:

  typedef std::list<typewriter::view::Line> list;
  typedef std::list<typewriter::view::Line>::const_iterator iterator;

private:
  iterator m_begin;
  iterator m_end;
  size_t m_size;

public:

  QTypewriterVisibleLines(const list& lines, size_t b, size_t s)
  {
    m_begin = std::next(lines.begin(), b);
    m_size = std::min({ lines.size() - b, s });
    m_end = std::next(m_begin, m_size);
  }

  iterator begin() const { return m_begin; }
  iterator end() const { return m_end; }

  size_t size() const { return m_size; }
};

} // namespace details

struct Marker
{
  int line = -1;
  int markers = 0;
};


// @TODO: remove me
enum class MarkerType
{
  Breakpoint = 1,
  Breakposition = 2,
};

class QTypewriterDocument : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString filepath READ filepath WRITE setFilePath NOTIFY filepathChanged)
  Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged)
public:
  explicit QTypewriterDocument(QObject* parent = nullptr);
  explicit QTypewriterDocument(typewriter::TextDocument* document, QObject* parent = nullptr);
  explicit QTypewriterDocument(std::unique_ptr<typewriter::TextDocument> document, QObject* parent = nullptr);
  ~QTypewriterDocument();

  typewriter::TextDocument* document();
  void setDocument(typewriter::TextDocument* doc);
  void setDocument(std::unique_ptr<typewriter::TextDocument> doc);

  QString filepath() const;
  void setFilePath(const QString& filepath);

  int lineCount() const;

  void notifyBlockDestroyed(int line);
  void notifyBlockInserted(const Position& pos, const TextBlock& block);
  void notifyContentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded);

Q_SIGNALS:
  void filepathChanged();
  void lineCountChanged();
  void blockDestroyed();
  void blockInserted(int line);

private:
  std::unique_ptr<typewriter::TextDocument> m_document_ptr;
  typewriter::TextDocument* m_document = nullptr;
  std::unique_ptr<typewriter::TextDocumentListener> m_listener;
  QString m_filepath;
};

class QTypewriterView : public QObject, public typewriter::TextDocumentListener
{
  Q_OBJECT
  Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)
  Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged)
  Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged)
  Q_PROPERTY(int tabSize READ tabSize WRITE setTabSize NOTIFY tabSizeChanged)
  Q_PROPERTY(QSize size READ size WRITE resize NOTIFY sizeChanged)
  Q_PROPERTY(int effectiveWidth READ effectiveWidth NOTIFY columnCountChanged)
  Q_PROPERTY(int effectiveHeight READ effectiveHeight NOTIFY lineCountChanged)
  Q_PROPERTY(int hscroll READ hscroll WRITE setHScroll NOTIFY hscrollChanged)
  Q_PROPERTY(int linescroll READ linescroll WRITE setLineScroll NOTIFY linescrollChanged)
  Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
public:
  explicit QTypewriterView(QObject* parent = nullptr);
  explicit QTypewriterView(QTypewriterDocument* document, QObject* parent = nullptr);
  ~QTypewriterView();

  QTypewriterDocument* document();
  void setDocument(QObject* doc);
  void setDocument(QTypewriterDocument* doc);

  typewriter::TextView& view();
  const typewriter::TextView& view() const;

  int lineCount() const;
  int columnCount() const;
  int effectiveWidth();
  int effectiveHeight();

  int tabSize() const;
  void setTabSize(int colcount);

  QSize size() const;
  void resize(const QSize& s);

  int hscroll() const;
  void setHScroll(int hscroll);

  int displayedLineCount();

  int linescroll() const;
  void setLineScroll(int linescroll);

  const QFont& font() const;
  void setFont(const QFont& font);

  const QTypewriterFontMetrics& metrics() const;

  const TextFormat& defaultTextFormat() const;
  void setDefaultTextFormat(TextFormat fmt);

  const TextFormat& textFormat(int id) const;
  void setFormat(int id, TextFormat fmt);

  void paint(QPainter* painter);

  typewriter::Position hitTest(const QPoint& pos) const;
  QPoint map(const typewriter::Position& pos) const;
  bool isVisible(const typewriter::Position& pos) const;

Q_SIGNALS:
  void documentChanged();
  void lineCountChanged();
  void columnCountChanged();
  void tabSizeChanged();
  void sizeChanged();
  void hscrollChanged();
  void linescrollChanged();
  void fontChanged();
  void blockDestroyed();
  void blockInserted(int line);
  void invalidated();

protected:
  void blockDestroyed(int line, const TextBlock& block) override;
  void blockInserted(const Position& pos, const TextBlock& block) override;
  void contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded) override;

protected:
  QPainter& painter();
 
  void paint(std::list<typewriter::view::Line>::const_iterator begin, std::list<typewriter::view::Line>::const_iterator end);
  virtual void drawLine(const QPoint& offset, const view::Line& line);
  virtual void drawFoldSymbol(const QPoint& offset, int foldid);
  void drawBlockFragment(QPoint offset, const view::Line& line, const view::LineElement& fragment);
  void drawText(const QPoint& offset, const QString& text, const TextFormat& format);
  void drawStrikeOut(const QPoint& offset, const TextFormat& fmt, int count);
  void drawUnderline(const QPoint& offset, const TextFormat& fmt, int count);
  void drawWaveUnderline(const QPoint& offset, const TextFormat& fmt, int count);
  static void applyFormat(QPainter& painter, const TextFormat& fmt);

  details::QTypewriterVisibleLines visibleLines() const;

private:
  QTypewriterDocument* m_document = nullptr;
  typewriter::TextView m_view;
  QTypewriterFontMetrics m_metrics;
  TextFormat m_default_format;
  std::vector<TextFormat> m_formats;
  QSize m_size;
  int m_hscroll = 0;
  int m_linescroll = 0;
  QFont m_font;
  QPainter* m_painter = nullptr;
};

} // namespace typewriter

#endif // !TYPEWRITER_QTYPEWRITER_COMMON_H
