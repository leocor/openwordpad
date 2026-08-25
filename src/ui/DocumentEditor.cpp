#include "DocumentEditor.h"
#include <QPainter>
#include <QScrollBar>
#include <QPageLayout>
#include <QDateTime>
#include <QFileInfo>
#include <QFileDialog>
#include <QImageReader>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <cmath>

namespace OpenWordPad {

DocumentEditor::DocumentEditor(QWidget *parent)
    : QTextEdit(parent)
{
    setAcceptRichText(true);
    setTabStopDistance(40.0); // Standard tab stop
    viewport()->setCursor(Qt::IBeamCursor);
    
    // Set modern Windows WordPad styling
    setStyleSheet(QStringLiteral(
        "DocumentEditor { background-color: #d8e5f2; border: none; }"
        "DocumentEditor QScrollBar:vertical { width: 14px; background: #f0f0f0; }"
        "DocumentEditor QScrollBar::handle:vertical { background: #cdcdcd; min-height: 20px; border-radius: 2px; }"
        "DocumentEditor QScrollBar::handle:vertical:hover { background: #a6a6a6; }"
    ));

    QFont defaultFont(QStringLiteral("Calibri"), 11);
    if (!QFontInfo(defaultFont).exactMatch()) {
        defaultFont.setFamily(QStringLiteral("DejaVu Sans"));
    }
    setFont(defaultFont);
    document()->setDefaultFont(defaultFont);

    // Initial page layout
    updatePageLayout(m_pageSettings);

    connect(this, &QTextEdit::cursorPositionChanged, this, &DocumentEditor::onCursorPositionChanged);
}

void DocumentEditor::setWrapMode(WrapMode mode) {
    m_wrapMode = mode;
    updateMargins();
}

void DocumentEditor::setZoomFactor(double factor) {
    if (factor <= 0.05 || factor >= 10.0) return;
    m_zoomFactor = factor;
    
    // Zoom QTextDocument
    QFont f = document()->defaultFont();
    f.setPointSizeF(m_baseFontSize * m_zoomFactor);
    document()->setDefaultFont(f);

    updateMargins();
    viewport()->update();
    update();
    emit zoomFactorChanged(m_zoomFactor);
}

void DocumentEditor::updatePageLayout(const PageSettings &settings) {
    m_pageSettings = settings;
    updateMargins();
}

void DocumentEditor::updateMargins() {
    double pageWidthInches = 8.27; // Default A4
    if (m_pageSettings.pageSizeId == QPageSize::Letter) pageWidthInches = 8.5;
    else if (m_pageSettings.pageSizeId == QPageSize::Legal) pageWidthInches = 8.5;
    else if (m_pageSettings.pageSizeId == QPageSize::A3) pageWidthInches = 11.69;
    else if (m_pageSettings.pageSizeId == QPageSize::A5) pageWidthInches = 5.83;

    if (m_pageSettings.orientation == QPageLayout::Landscape) {
        pageWidthInches = 11.69; // A4 Landscape
    }

    double totalPageWidthPx = pageWidthInches * Units::DPI * m_zoomFactor;
    int originX = std::max(20, (width() - static_cast<int>(totalPageWidthPx)) / 2);
    int rightMarginWidth = std::max(20, width() - (originX + static_cast<int>(totalPageWidthPx)));

    double printableWidthPx = (pageWidthInches - m_pageSettings.leftMargin - m_pageSettings.rightMargin) * Units::DPI * m_zoomFactor;
    int leftMarginPx = static_cast<int>(m_pageSettings.leftMargin * Units::DPI * m_zoomFactor);
    int rightMarginPx = static_cast<int>(m_pageSettings.rightMargin * Units::DPI * m_zoomFactor);
    int topMarginPx = static_cast<int>(m_pageSettings.topMargin * Units::DPI * m_zoomFactor);
    int bottomMarginPx = static_cast<int>(m_pageSettings.bottomMargin * Units::DPI * m_zoomFactor);

    // Frame margins inside the white sheet
    QTextFrameFormat frameFmt = document()->rootFrame()->frameFormat();
    frameFmt.setLeftMargin(leftMarginPx);
    frameFmt.setRightMargin(rightMarginPx);
    frameFmt.setTopMargin(topMarginPx);
    frameFmt.setBottomMargin(bottomMarginPx);
    document()->rootFrame()->setFrameFormat(frameFmt);

    if (m_wrapMode == WrapMode::WrapToRuler) {
        setLineWrapColumnOrWidth(leftMarginPx + static_cast<int>(printableWidthPx));
        setLineWrapMode(QTextEdit::FixedPixelWidth);
    } else if (m_wrapMode == WrapMode::WrapToWindow) {
        setLineWrapMode(QTextEdit::WidgetWidth);
    } else {
        setLineWrapMode(QTextEdit::NoWrap);
    }

    document()->setDocumentMargin(0);

    // Position the white viewport sheet in the center
    setViewportMargins(originX, 8, rightMarginWidth, 16);
    viewport()->setStyleSheet("background-color: #ffffff;");
}

void DocumentEditor::applyCharFormat(const QTextCharFormat &format) {
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    mergeCurrentCharFormat(format);
}

void DocumentEditor::toggleBold() {
    QTextCharFormat fmt;
    fmt.setFontWeight(fontWeight() > QFont::Normal ? QFont::Normal : QFont::Bold);
    applyCharFormat(fmt);
}

void DocumentEditor::toggleItalic() {
    QTextCharFormat fmt;
    fmt.setFontItalic(!fontItalic());
    applyCharFormat(fmt);
}

void DocumentEditor::toggleUnderline() {
    QTextCharFormat fmt;
    fmt.setFontUnderline(!fontUnderline());
    applyCharFormat(fmt);
}

void DocumentEditor::toggleStrikethrough() {
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(!currentCharFormat().fontStrikeOut());
    applyCharFormat(fmt);
}

void DocumentEditor::toggleSubscript() {
    QTextCharFormat fmt;
    fmt.setVerticalAlignment(currentCharFormat().verticalAlignment() == QTextCharFormat::AlignSubScript
                             ? QTextCharFormat::AlignNormal
                             : QTextCharFormat::AlignSubScript);
    applyCharFormat(fmt);
}

void DocumentEditor::toggleSuperscript() {
    QTextCharFormat fmt;
    fmt.setVerticalAlignment(currentCharFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript
                             ? QTextCharFormat::AlignNormal
                             : QTextCharFormat::AlignSuperScript);
    applyCharFormat(fmt);
}

void DocumentEditor::setFontFamily(const QString &family) {
    QTextCharFormat fmt;
    fmt.setFontFamilies({family});
    applyCharFormat(fmt);
}

void DocumentEditor::setFontSize(double pt) {
    if (pt <= 0) return;
    m_baseFontSize = pt;
    QTextCharFormat fmt;
    fmt.setFontPointSize(pt * m_zoomFactor);
    applyCharFormat(fmt);
}

void DocumentEditor::growFontSize() {
    qreal curSize = fontPointSize();
    if (curSize <= 0) curSize = m_baseFontSize;
    setFontSize(curSize + 2.0);
}

void DocumentEditor::shrinkFontSize() {
    qreal curSize = fontPointSize();
    if (curSize <= 0) curSize = m_baseFontSize;
    if (curSize > 2.0) {
        setFontSize(curSize - 2.0);
    }
}

void DocumentEditor::setTextColor(const QColor &color) {
    if (!color.isValid()) return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    applyCharFormat(fmt);
}

void DocumentEditor::setTextHighlightColor(const QColor &color) {
    QTextCharFormat fmt;
    fmt.setBackground(color.isValid() ? QBrush(color) : Qt::NoBrush);
    applyCharFormat(fmt);
}

void DocumentEditor::setAlignment(Qt::Alignment align) {
    QTextEdit::setAlignment(align);
}

void DocumentEditor::increaseIndent() {
    QTextCursor cursor = textCursor();
    QTextBlockFormat fmt = cursor.blockFormat();
    fmt.setIndent(fmt.indent() + 1);
    cursor.setBlockFormat(fmt);
}

void DocumentEditor::decreaseIndent() {
    QTextCursor cursor = textCursor();
    QTextBlockFormat fmt = cursor.blockFormat();
    if (fmt.indent() > 0) {
        fmt.setIndent(fmt.indent() - 1);
        cursor.setBlockFormat(fmt);
    }
}

void DocumentEditor::setListStyle(QTextListFormat::Style style) {
    QTextCursor cursor = textCursor();
    if (style == QTextListFormat::ListStyleUndefined) {
        // Remove list formatting
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(0);
        cursor.setBlockFormat(blockFmt);
        return;
    }

    cursor.beginEditBlock();
    QTextListFormat listFmt;
    listFmt.setStyle(style);
    cursor.createList(listFmt);
    cursor.endEditBlock();
}

void DocumentEditor::setLineSpacing(double multiplier, double spaceAfterPt) {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat fmt = cursor.blockFormat();
    fmt.setLineHeight(multiplier * 100.0, QTextBlockFormat::ProportionalHeight);
    if (spaceAfterPt >= 0) {
        fmt.setBottomMargin(spaceAfterPt);
    }
    cursor.setBlockFormat(fmt);
    cursor.endEditBlock();
}

void DocumentEditor::insertImage(const QImage &image, const QString &name) {
    if (image.isNull()) return;
    QString imgName = name.isEmpty() ? QStringLiteral("image_%1.png").arg(QDateTime::currentMSecsSinceEpoch()) : name;
    document()->addResource(QTextDocument::ImageResource, QUrl(imgName), image);
    textCursor().insertImage(imgName);
}

void DocumentEditor::insertImageFromFile(const QString &filePath) {
    QImage img(filePath);
    if (!img.isNull()) {
        insertImage(img, QFileInfo(filePath).fileName());
    }
}

void DocumentEditor::resizeSelectedImage(int width, int height) {
    QTextCursor cursor = textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    if (fmt.isImageFormat()) {
        QTextImageFormat imgFmt = fmt.toImageFormat();
        imgFmt.setWidth(width);
        imgFmt.setHeight(height);
        cursor.mergeCharFormat(imgFmt);
    }
}

void DocumentEditor::insertDateTime(const QString &text) {
    textCursor().insertText(text);
}

bool DocumentEditor::findText(const QString &text, QTextDocument::FindFlags flags) {
    return find(text, flags);
}

bool DocumentEditor::replaceText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags) {
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection() && cursor.selectedText().compare(findStr, (flags & QTextDocument::FindCaseSensitively) ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0) {
        cursor.insertText(replaceStr);
    }
    return find(findStr, flags);
}

int DocumentEditor::replaceAll(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags) {
    int count = 0;
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);

    while (find(findStr, flags)) {
        textCursor().insertText(replaceStr);
        count++;
    }
    return count;
}

void DocumentEditor::onCursorPositionChanged() {
    QTextCursor cursor = textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.positionInBlock() + 1;
    int chars = cursor.selectedText().length();
    emit cursorPositionChanged(line, col, chars);
    emit currentFormatChanged(cursor.charFormat(), cursor.blockFormat());
}

void DocumentEditor::resizeEvent(QResizeEvent *event) {
    QTextEdit::resizeEvent(event);
    updateMargins();
}

void DocumentEditor::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        if (delta > 0) {
            setZoomFactor(m_zoomFactor + 0.1);
        } else if (delta < 0) {
            setZoomFactor(m_zoomFactor - 0.1);
        }
        event->accept();
        return;
    }
    QTextEdit::wheelEvent(event);
}

void DocumentEditor::paintEvent(QPaintEvent *event) {
    QTextEdit::paintEvent(event);
}

} // namespace OpenWordPad
