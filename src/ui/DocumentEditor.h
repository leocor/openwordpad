#pragma once

#include <QTextEdit>
#include <QTextListFormat>
#include "../core/Units.h"
#include "../core/DocumentManager.h"

namespace OpenWordPad {

enum class WrapMode {
    NoWrap,
    WrapToWindow,
    WrapToRuler
};

class DocumentEditor : public QTextEdit {
    Q_OBJECT

public:
    explicit DocumentEditor(QWidget *parent = nullptr);

    void setWrapMode(WrapMode mode);
    WrapMode wrapMode() const { return m_wrapMode; }

    void setZoomFactor(double factor);
    double zoomFactor() const { return m_zoomFactor; }

    void updatePageLayout(const PageSettings &settings);

    // Rich Text Formatting Actions
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void toggleStrikethrough();
    void toggleSubscript();
    void toggleSuperscript();

    void setFontFamily(const QString &family);
    void setFontSize(double pt);
    void growFontSize();
    void shrinkFontSize();
    void setTextColor(const QColor &color);
    void setTextHighlightColor(const QColor &color);

    void setAlignment(Qt::Alignment align);
    void increaseIndent();
    void decreaseIndent();
    void setListStyle(QTextListFormat::Style style);
    void setLineSpacing(double multiplier, double spaceAfterPt = 0);

    void insertImage(const QImage &image, const QString &name = QString());
    void insertImageFromFile(const QString &filePath);
    void resizeSelectedImage(int width, int height);

    void insertDateTime(const QString &text);

    bool findText(const QString &text, QTextDocument::FindFlags flags);
    bool replaceText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags);
    int replaceAll(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags);

signals:
    void cursorPositionChanged(int line, int col, int charCount);
    void currentFormatChanged(const QTextCharFormat &charFmt, const QTextBlockFormat &blockFmt);
    void zoomFactorChanged(double factor);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCursorPositionChanged();

private:
    void applyCharFormat(const QTextCharFormat &format);
    void updateMargins();

    WrapMode m_wrapMode = WrapMode::WrapToRuler;
    double m_zoomFactor = 1.0;
    PageSettings m_pageSettings;
    double m_baseFontSize = 11.0;
};

} // namespace OpenWordPad
