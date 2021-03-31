// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the typewriter library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef TYPEWRITER_QTYPEWRITER_COMMON_H
#define TYPEWRITER_QTYPEWRITER_COMMON_H

#include "typewriter/qt/codeeditor-qt-defs.h"

#include "typewriter/contributor.h"
#include "typewriter/textview.h"
#include "typewriter/syntaxhighlighter.h"

#include <QObject>

#include <QColor>
#include <QFont>
#include <QPen>

#include <vector>

class QPainter;
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

struct BlockFormat
{
  QColor background_color = QColor(255, 255, 255);
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

class TYPEWRITER_QAPI QTypewriterDocument : public QObject
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

class QTypewriterSyntaxHighlighter;

class TYPEWRITER_QAPI QTypewriterView : public QObject, public typewriter::TextDocumentListener
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

  const BlockFormat& blockFormat(int id) const;
  void setBlockFormat(int id, BlockFormat fmt);

  typewriter::Position hitTest(const QPoint& pos) const;
  QPoint map(const typewriter::Position& pos) const;
  bool isVisible(const typewriter::Position& pos) const;

  void install(QTypewriterSyntaxHighlighter* highlighter);
  void uninstallSyntaxHighlighter();

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
  bool event(QEvent* ev) override;

protected:
  void blockDestroyed(int line, const TextBlock& block) override;
  void blockInserted(const Position& pos, const TextBlock& block) override;
  void contentsChange(const TextBlock& block, const Position& pos, int charsRemoved, int charsAdded) override;

protected:

  details::QTypewriterVisibleLines visibleLines() const;

  void highlightView();

private:
  QTypewriterDocument* m_document = nullptr;
  typewriter::TextView m_view;
  QTypewriterFontMetrics m_metrics;
  TextFormat m_default_format;
  std::vector<TextFormat> m_text_formats;
  std::vector<BlockFormat> m_block_formats;
  QSize m_size;
  int m_hscroll = 0;
  int m_linescroll = 0;
  QFont m_font;
  QTypewriterSyntaxHighlighter* m_syntax_highlighter = nullptr;
};

class TYPEWRITER_QAPI QTypewriterSyntaxHighlighter : public QObject
{
  Q_OBJECT
public:
  explicit QTypewriterSyntaxHighlighter(QObject* parent = nullptr); 
  QTypewriterSyntaxHighlighter(QTypewriterView* view, QObject* parent = nullptr);
  ~QTypewriterSyntaxHighlighter();

  QTypewriterView* view() const;
  QTypewriterDocument* document() const;

protected:
  typewriter::TextBlock currentBlock() const;

  virtual void highlightBlock(const std::string& text) = 0;
  void setFormat(int start, int count, int formatId);

  int previousBlockState() const;
  void setCurrentBlockState(int state);

private:
  friend class QTypewriterView;
  void setView(QTypewriterView* view);
  void startHighlight(typewriter::TextBlock block);
  void highlightNextBlock();
  void endHighlight();

private:
  QTypewriterView* m_view = nullptr;
  std::unique_ptr<typewriter::SyntaxHighlighter> m_impl;
  int m_last_highlighted_line = -1;
};

namespace viewrendering
{

template<typename R>
void drawBlockFragment(QTypewriterView& view, R&& renderer, QPoint offset, const view::Line& line, const view::LineElement& fragment)
{
  view::StyledFragments fragments = view.view().fragments(line, fragment);

  for (auto it = fragments.begin(); it != fragments.end(); it = it.next())
  {
    //QString text = QString::fromStdString(fragment.block.text().substr(fragment.begin, fragment.width));
    //drawText(painter, offset, text, m_context->default_format);
    QString text = QString::fromStdString(it.text());
    renderer.drawText(offset, text, view.textFormat(it.format()));
    offset.rx() += it.length() * view.metrics().charwidth;
  }
}

template<typename R>
void drawLine(QTypewriterView& view, R&& renderer, const QPoint& offset, const view::Line& line)
{
  renderer.beginLine(offset, line);

  if (line.elements.empty() || line.elements.front().kind == view::LineElement::LE_Insert)
  {
    renderer.endLine();
    return;
  }

  QPoint pt = offset;

  for (const auto& e : line.elements)
  {
    if (e.kind == view::LineElement::LE_LineIndent || e.kind == view::LineElement::LE_CarriageReturn)
      continue;

    if (e.kind == view::LineElement::LE_Fold)
    {
      renderer.drawFoldSymbol(pt, e.id);
      pt.rx() += e.width * view.metrics().charwidth;
    }
    else if (e.kind == view::LineElement::LE_BlockFragment)
    {
      drawBlockFragment(view, std::forward<R>(renderer), pt, line, e);
      pt.rx() += e.width * view.metrics().charwidth;
    }
  }

  renderer.endLine();
}

} // namespace viewrendering

template<typename R>
void render(QTypewriterView& view, R&& renderer)
{
  auto begin = std::next(view.view().lines().begin(), view.linescroll());

  auto end = begin;
  int numline = 1 + view.size().height() / view.metrics().lineheight;

  while (numline > 0 && end != view.view().lines().end())
  {
    ++end;
    --numline;
  }

  int i = 0;
  for (auto it = begin; it != end; ++it, ++i)
  {
    const int baseline = i * view.metrics().lineheight + view.metrics().ascent;
    viewrendering::drawLine(view, std::forward<R>(renderer), QPoint{ -view.hscroll(), baseline }, *it);
  }
}

class TYPEWRITER_QAPI QTypewriterPainterRenderer
{
private:
  QTypewriterView& m_view;
  QPainter& m_painter;

public:
  QTypewriterPainterRenderer(QTypewriterView& view, QPainter& p);

  QPainter& painter();

  void beginLine(const QPoint& offset, const view::Line& line);
  void endLine();

  void drawFoldSymbol(const QPoint& offset, int foldid);
  void drawText(const QPoint& offset, const QString& text, const TextFormat& format);
  void drawStrikeOut(const QPoint& offset, const TextFormat& fmt, int count);
  void drawUnderline(const QPoint& offset, const TextFormat& fmt, int count);
  void drawWaveUnderline(const QPoint& offset, const TextFormat& fmt, int count);

  static void applyFormat(QPainter& painter, const TextFormat& fmt);
  static void applyFormat(QPainter& painter, const BlockFormat& fmt);
};

} // namespace typewriter

#endif // !TYPEWRITER_QTYPEWRITER_COMMON_H
