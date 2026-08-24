#pragma once

#include <QMainWindow>
#include <QLabel>
#include "../core/DocumentManager.h"
#include "../core/Units.h"
#include "RibbonBar.h"
#include "RulerWidget.h"
#include "DocumentEditor.h"
#include "ZoomSliderWidget.h"
#include "dialogs/FindReplaceDialog.h"

namespace OpenWordPad {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool openFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    // File operations
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAs(DocumentFormat format = DocumentFormat::Unknown);
    void onPrint();
    void onQuickPrint();
    void onPrintPreview();
    void onPageSetup();
    void onSendEmail();
    void onAbout();

    // Formatting triggers
    void onFontFamilyChanged(const QString &family);
    void onFontSizeChanged(const QString &sizeStr);
    void onTextColorSelected(const QColor &color);
    void onHighlightColorSelected(const QColor &color);

    // Insert actions
    void onInsertPicture();
    void onChangePicture();
    void onResizePicture();
    void onPaintDrawing();
    void onDateTime();
    void onInsertObject();

    // Editing actions
    void onFind();
    void onReplace();
    void onFindNext(const QString &text, QTextDocument::FindFlags flags);
    void onReplaceText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags);
    void onReplaceAllText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags);

    // View actions
    void onZoomChanged(int percent);
    void onRulerToggled(bool visible);
    void onStatusBarToggled(bool visible);
    void onWordWrapChanged(int index);
    void onUnitsChanged(int index);

    // Ruler drag actions
    void onRulerLeftMarginChanged(double inches);
    void onRulerRightMarginChanged(double inches);

    // Document state updates
    void updateTitle();
    void onCursorPositionChanged(int line, int col, int charCount);
    void onDocumentModifiedChanged(bool modified);
    void onParagraphDialog();

private:
    bool maybeSave();
    void setupUI();
    void setupConnections();

    RibbonBar *m_ribbon;
    RulerWidget *m_ruler;
    DocumentEditor *m_editor;
    ZoomSliderWidget *m_zoomWidget;
    QLabel *m_statusLabel;
    QLabel *m_posLabel;
    DocumentManager *m_docManager;
    FindReplaceDialog *m_findReplaceDialog = nullptr;
    UnitType m_currentUnit = UnitType::Centimeters;
};

} // namespace OpenWordPad
