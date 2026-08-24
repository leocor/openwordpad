#include "DocumentEditor.h"
#include <QTextBlock>
#include <QTextList>
#include <QTextImageFormat>
#include <QWheelEvent>
#include <QPainter>
#include <QFileDialog>
#include <QFileInfo>
#include <QBuffer>
#include <QUrl>
#include <QScrollBar>
#include <QDebug>

namespace OpenWordPad {

DocumentEditor::DocumentEditor(QWidget *parent)
    : QTextEdit(parent)
{
    setAcceptRichText(true);
    setTabStopDistance(36.0); // 0.5 inch tab stop default

    // Set classic WordPad soft blue canvas background
    setStyleSheet(
        "QTextEdit { background-color: #d8e5f2; border: none; }"
    );
    viewport()->setStyleSheet("background-color: #d8e5f2;");

    QFont defaultFont(QStringLiteral("Calibri"), 11);
    if (!defaultFont.exactMatch()) {
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
    if (m_wrapMode == WrapMode::NoWrap) {
        setLineWrapMode(QTextEdit::NoWrap);
    } else if (m_wrapMode == WrapMode::WrapToWindow) {
        setLineWrapMode(QTextEdit::WidgetWidth);
    } else if (m_wrapMode == WrapMode::WrapToRuler) {
        setLineWrapMode(QTextEdit::FixedPixelWidth);
        updateMargins();
    }
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
    int originX = std::max(20, (viewport()->width() - static_cast<int>(totalPageWidthPx)) / 2);

    double printableWidthPx = (pageWidthInches - m_pageSettings.leftMargin - m_pageSettings.rightMargin) * Units::DPI * m_zoomFactor;
    
    if (m_wrapMode == WrapMode::WrapToRuler) {
        setLineWrapColumnOrWidth(static_cast<int>(printableWidthPx));
        setLineWrapMode(QTextEdit::FixedPixelWidth);
    } else if (m_wrapMode == WrapMode::WrapToWindow) {
        setLineWrapMode(QTextEdit::WidgetWidth);
    } else {
        setLineWrapMode(QTextEdit::NoWrap);
    }

    // Align page margins to ruler and center
    int leftMarginPx = static_cast<int>(m_pageSettings.leftMargin * Units::DPI * m_zoomFactor);
    int bottomMarginPx = static_cast<int>(m_pageSettings.bottomMargin * Units::DPI * m_zoomFactor);

    document()->setDocumentMargin(originX + leftMarginPx);
    setViewportMargins(0, 8, 0, bottomMarginPx / 2);
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
    if (currentCharFormat().verticalAlignment() == QTextCharFormat::AlignSubScript) {
        fmt.setVerticalAlignment(QTextCharFormat::AlignNormal);
    } else {
        fmt.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    }
    applyCharFormat(fmt);
}

void DocumentEditor::toggleSuperscript() {
    QTextCharFormat fmt;
    if (currentCharFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript) {
        fmt.setVerticalAlignment(QTextCharFormat::AlignNormal);
    } else {
        fmt.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    }
    applyCharFormat(fmt);
}

void DocumentEditor::setFontFamily(const QString &family) {
    QTextCharFormat fmt;
    fmt.setFontFamilies({family});
    applyCharFormat(fmt);
}

void DocumentEditor::setFontSize(double pt) {
    if (pt <= 0) return;
    QTextCharFormat fmt;
    fmt.setFontPointSize(pt);
    applyCharFormat(fmt);
}

void DocumentEditor::growFontSize() {
    qreal currentSize = fontPointSize();
    if (currentSize <= 0) currentSize = 11.0;
    setFontSize(currentSize + 2.0);
}

void DocumentEditor::shrinkFontSize() {
    qreal currentSize = fontPointSize();
    if (currentSize <= 0) currentSize = 11.0;
    if (currentSize > 2.0) {
        setFontSize(currentSize - 2.0);
    }
}

void DocumentEditor::setTextColor(const QColor &color) {
    QTextCharFormat fmt;
    fmt.setForeground(color);
    applyCharFormat(fmt);
}

void DocumentEditor::setTextHighlightColor(const QColor &color) {
    QTextCharFormat fmt;
    fmt.setBackground(color);
    applyCharFormat(fmt);
}

void DocumentEditor::setAlignment(Qt::Alignment align) {
    QTextEdit::setAlignment(align);
}

void DocumentEditor::increaseIndent() {
    QTextBlockFormat bf = textCursor().blockFormat();
    bf.setLeftMargin(bf.leftMargin() + 36.0); // 0.5 in
    textCursor().setBlockFormat(bf);
}

void DocumentEditor::decreaseIndent() {
    QTextBlockFormat bf = textCursor().blockFormat();
    bf.setLeftMargin(std::max(0.0, bf.leftMargin() - 36.0));
    textCursor().setBlockFormat(bf);
}

void DocumentEditor::setListStyle(QTextListFormat::Style style) {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    if (style == QTextListFormat::ListStyleUndefined) {
        QTextBlockFormat bf = cursor.blockFormat();
        bf.setObjectIndex(-1);
        cursor.setBlockFormat(bf);
    } else {
        QTextListFormat listFmt;
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }
    cursor.endEditBlock();
}

void DocumentEditor::setLineSpacing(double multiplier, double spaceAfterPt) {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat bf = cursor.blockFormat();
    bf.setLineHeight(multiplier * 100.0, QTextBlockFormat::ProportionalHeight);
    if (spaceAfterPt > 0) {
        bf.setBottomMargin(spaceAfterPt * (96.0 / 72.0));
    } else {
        bf.setBottomMargin(0);
    }
    cursor.setBlockFormat(bf);
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
    double pageWidthInches = 8.27; // Default A4
    if (m_pageSettings.pageSizeId == QPageSize::Letter) pageWidthInches = 8.5;
    else if (m_pageSettings.pageSizeId == QPageSize::Legal) pageWidthInches = 8.5;
    else if (m_pageSettings.pageSizeId == QPageSize::A3) pageWidthInches = 11.69;
    else if (m_pageSettings.pageSizeId == QPageSize::A5) pageWidthInches = 5.83;

    if (m_pageSettings.orientation == QPageLayout::Landscape) {
        pageWidthInches = 11.69;
    }

    int totalPageWidthPx = static_cast<int>(pageWidthInches * Units::DPI * m_zoomFactor);
    int originX = std::max(20, (viewport()->width() - totalPageWidthPx) / 2);
    int scrollY = verticalScrollBar()->value();
    int topY = (scrollY == 0) ? 8 : 0;

    // 1. Draw white page background and shadow first
    {
        QPainter p(viewport());
        p.fillRect(viewport()->rect(), Qt::white);
    }

    // 2. Render Text Document
    QTextEdit::paintEvent(event);

    // 3. Paint over outside margins with blue canvas to guarantee 0 bleeding
    {
        QPainter p(viewport());
        QColor blueCanvas(0xd8, 0xe5, 0xf2);

        // Left canvas
        p.fillRect(0, 0, originX, viewport()->height(), blueCanvas);

        // Right canvas
        int rightStartX = originX + totalPageWidthPx;
        int rightWidth = std::max(0, viewport()->width() - rightStartX);
        p.fillRect(rightStartX, 0, rightWidth, viewport()->height(), blueCanvas);

        // Top gap when at the very top of document
        if (topY > 0) {
            p.fillRect(originX, 0, totalPageWidthPx, topY, blueCanvas);
        }

        // Draw page borders
        p.setPen(QColor(0xa0, 0xb6, 0xcc));
        p.drawLine(originX, topY, originX, viewport()->height());
        p.drawLine(rightStartX, topY, rightStartX, viewport()->height());
        if (topY > 0) {
            p.drawLine(originX, topY, rightStartX, topY);
        }

        // Subtle drop shadow on right side
        p.fillRect(rightStartX + 1, topY + 2, 2, viewport()->height(), QColor(0x9a, 0xad, 0xc2, 90));
    }
}

} // namespace OpenWordPad
