#include "MainWindow.h"
#include "../core/TranslationManager.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/PageSetupDialog.h"
#include "dialogs/ParagraphDialog.h"
#include "dialogs/DateTimeDialog.h"
#include "dialogs/ImageResizeDialog.h"
#include "dialogs/PaintDrawingDialog.h"
#include <QVBoxLayout>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QCloseEvent>
#include <QFileInfo>

namespace OpenWordPad {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setAcceptDrops(true);
    resize(1024, 768);
    setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.svg")));

    setupUI();
    setupConnections();

    m_docManager->newDocument();
    updateTitle();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    auto centralWidget = new QWidget(this);
    auto centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    // 1. Ribbon Bar
    m_ribbon = new RibbonBar(this);
    centralLayout->addWidget(m_ribbon);

    // 2. Ruler Widget
    m_ruler = new RulerWidget(this);
    m_ruler->setOriginX(10);
    m_ruler->setUnit(m_currentUnit);
    centralLayout->addWidget(m_ruler);

    // 3. Document Editor
    m_editor = new DocumentEditor(this);
    centralLayout->addWidget(m_editor, 1);

    setCentralWidget(centralWidget);

    // Document Manager
    m_docManager = new DocumentManager(m_editor->document(), this);

    // 4. Status Bar
    auto sb = statusBar();
    m_statusLabel = new QLabel(tr("Ready"), sb);
    m_posLabel = new QLabel(QStringLiteral("Ln 1, Col 1"), sb);
    m_posLabel->setMinimumWidth(100);
    m_zoomWidget = new ZoomSliderWidget(sb);

    sb->addWidget(m_statusLabel, 1);
    sb->addPermanentWidget(m_posLabel);
    sb->addPermanentWidget(m_zoomWidget);
}

void MainWindow::setupConnections() {
    // Document Manager signals
    connect(m_docManager, &DocumentManager::documentPathChanged, this, &MainWindow::updateTitle);
    connect(m_docManager, &DocumentManager::documentModifiedChanged, this, &MainWindow::onDocumentModifiedChanged);
    connect(m_docManager, &DocumentManager::recentFilesChanged, this, [this]() {
        m_ribbon->updateRecentFilesMenu(m_docManager->recentFiles());
    });
    connect(m_docManager, &DocumentManager::errorOccurred, this, [this](const QString &title, const QString &msg) {
        QMessageBox::critical(this, title, msg);
    });
    connect(m_ribbon, &RibbonBar::recentFileTriggered, this, &MainWindow::openFile);

    // Ribbon File & QAT actions
    connect(m_ribbon->actNew, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(m_ribbon->actOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(m_ribbon->actSave, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(m_ribbon->actSaveAsRtf, &QAction::triggered, this, [this]() { onSaveAs(DocumentFormat::Rtf); });
    connect(m_ribbon->actSaveAsDocx, &QAction::triggered, this, [this]() { onSaveAs(DocumentFormat::Docx); });
    connect(m_ribbon->actSaveAsOdt, &QAction::triggered, this, [this]() { onSaveAs(DocumentFormat::Odt); });
    connect(m_ribbon->actSaveAsTxt, &QAction::triggered, this, [this]() { onSaveAs(DocumentFormat::PlainText); });
    connect(m_ribbon->actSaveAsOther, &QAction::triggered, this, [this]() { onSaveAs(DocumentFormat::Unknown); });
    connect(m_ribbon->actPrint, &QAction::triggered, this, &MainWindow::onPrint);
    connect(m_ribbon->actQuickPrint, &QAction::triggered, this, &MainWindow::onQuickPrint);
    connect(m_ribbon->actPrintPreview, &QAction::triggered, this, &MainWindow::onPrintPreview);
    connect(m_ribbon->actPageSetup, &QAction::triggered, this, &MainWindow::onPageSetup);
    connect(m_ribbon->actEmail, &QAction::triggered, this, &MainWindow::onSendEmail);
    connect(m_ribbon->actAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(m_ribbon->actExit, &QAction::triggered, this, &QWidget::close);

    // Undo / Redo
    connect(m_ribbon->actUndo, &QAction::triggered, m_editor, &QTextEdit::undo);
    connect(m_ribbon->actRedo, &QAction::triggered, m_editor, &QTextEdit::redo);

    // Clipboard
    connect(m_ribbon->actPaste, &QAction::triggered, m_editor, &QTextEdit::paste);
    connect(m_ribbon->actPasteSpecial, &QAction::triggered, m_editor, &QTextEdit::paste);
    connect(m_ribbon->actCut, &QAction::triggered, m_editor, &QTextEdit::cut);
    connect(m_ribbon->actCopy, &QAction::triggered, m_editor, &QTextEdit::copy);

    // Font formatting
    connect(m_ribbon->cbFontFamily, &QComboBox::currentTextChanged, this, &MainWindow::onFontFamilyChanged);
    connect(m_ribbon->cbFontSize, &QComboBox::currentTextChanged, this, &MainWindow::onFontSizeChanged);
    connect(m_ribbon->actGrowFont, &QAction::triggered, m_editor, &DocumentEditor::growFontSize);
    connect(m_ribbon->actShrinkFont, &QAction::triggered, m_editor, &DocumentEditor::shrinkFontSize);
    connect(m_ribbon->actBold, &QAction::triggered, m_editor, &DocumentEditor::toggleBold);
    connect(m_ribbon->actItalic, &QAction::triggered, m_editor, &DocumentEditor::toggleItalic);
    connect(m_ribbon->actUnderline, &QAction::triggered, m_editor, &DocumentEditor::toggleUnderline);
    connect(m_ribbon->actStrike, &QAction::triggered, m_editor, &DocumentEditor::toggleStrikethrough);
    connect(m_ribbon->actSubscript, &QAction::triggered, m_editor, &DocumentEditor::toggleSubscript);
    connect(m_ribbon->actSuperscript, &QAction::triggered, m_editor, &DocumentEditor::toggleSuperscript);
    connect(m_ribbon, &RibbonBar::fontColorSelected, this, &MainWindow::onTextColorSelected);
    connect(m_ribbon, &RibbonBar::highlightColorSelected, this, &MainWindow::onHighlightColorSelected);

    // Paragraph formatting
    connect(m_ribbon->actDecreaseIndent, &QAction::triggered, m_editor, &DocumentEditor::decreaseIndent);
    connect(m_ribbon->actIncreaseIndent, &QAction::triggered, m_editor, &DocumentEditor::increaseIndent);
    
    // Lists
    connect(m_ribbon->actListNone, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListStyleUndefined); });
    connect(m_ribbon->actListBullet, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListDisc); });
    connect(m_ribbon->actListNumber, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListDecimal); });
    connect(m_ribbon->actListAlphaLower, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListLowerAlpha); });
    connect(m_ribbon->actListAlphaUpper, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListUpperAlpha); });
    connect(m_ribbon->actListRomanLower, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListLowerRoman); });
    connect(m_ribbon->actListRomanUpper, &QAction::triggered, this, [this]() { m_editor->setListStyle(QTextListFormat::ListUpperRoman); });

    // Line Spacing
    connect(m_ribbon->actLineSpacing10, &QAction::triggered, this, [this]() { m_editor->setLineSpacing(1.0, m_ribbon->actLineSpacingAdd10pt->isChecked() ? 10 : 0); });
    connect(m_ribbon->actLineSpacing115, &QAction::triggered, this, [this]() { m_editor->setLineSpacing(1.15, m_ribbon->actLineSpacingAdd10pt->isChecked() ? 10 : 0); });
    connect(m_ribbon->actLineSpacing15, &QAction::triggered, this, [this]() { m_editor->setLineSpacing(1.5, m_ribbon->actLineSpacingAdd10pt->isChecked() ? 10 : 0); });
    connect(m_ribbon->actLineSpacing20, &QAction::triggered, this, [this]() { m_editor->setLineSpacing(2.0, m_ribbon->actLineSpacingAdd10pt->isChecked() ? 10 : 0); });
    connect(m_ribbon->actParagraphDialog, &QAction::triggered, this, &MainWindow::onParagraphDialog);

    // Alignment
    connect(m_ribbon->actAlignLeft, &QAction::triggered, this, [this]() { m_editor->setAlignment(Qt::AlignLeft); });
    connect(m_ribbon->actAlignCenter, &QAction::triggered, this, [this]() { m_editor->setAlignment(Qt::AlignHCenter); });
    connect(m_ribbon->actAlignRight, &QAction::triggered, this, [this]() { m_editor->setAlignment(Qt::AlignRight); });
    connect(m_ribbon->actAlignJustify, &QAction::triggered, this, [this]() { m_editor->setAlignment(Qt::AlignJustify); });

    // Insert actions
    connect(m_ribbon->actInsertPicture, &QAction::triggered, this, &MainWindow::onInsertPicture);
    connect(m_ribbon->actChangePicture, &QAction::triggered, this, &MainWindow::onChangePicture);
    connect(m_ribbon->actResizePicture, &QAction::triggered, this, &MainWindow::onResizePicture);
    connect(m_ribbon->actPaintDrawing, &QAction::triggered, this, &MainWindow::onPaintDrawing);
    connect(m_ribbon->actDateTime, &QAction::triggered, this, &MainWindow::onDateTime);
    connect(m_ribbon->actInsertObject, &QAction::triggered, this, &MainWindow::onInsertObject);

    // Editing actions
    connect(m_ribbon->actFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(m_ribbon->actReplace, &QAction::triggered, this, &MainWindow::onReplace);
    connect(m_ribbon->actSelectAll, &QAction::triggered, m_editor, &QTextEdit::selectAll);

    // View tab actions
    connect(m_ribbon->actZoomIn, &QAction::triggered, this, [this]() {
        int current = m_zoomWidget->zoomPercentage();
        int next = (current / 10 + 1) * 10;
        m_zoomWidget->setZoomPercentage(next);
    });
    connect(m_ribbon->actZoomOut, &QAction::triggered, this, [this]() {
        int current = m_zoomWidget->zoomPercentage();
        int prev = (current % 10 == 0) ? current - 10 : (current / 10) * 10;
        m_zoomWidget->setZoomPercentage(prev);
    });
    connect(m_ribbon->actZoom100, &QAction::triggered, this, [this]() {
        m_zoomWidget->setZoomPercentage(100);
    });
    connect(m_ribbon->chkRuler, &QCheckBox::toggled, this, &MainWindow::onRulerToggled);
    connect(m_ribbon->chkStatusBar, &QCheckBox::toggled, this, &MainWindow::onStatusBarToggled);
    connect(m_ribbon->cbWordWrap, &QComboBox::currentIndexChanged, this, &MainWindow::onWordWrapChanged);
    connect(m_ribbon->cbUnits, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitsChanged);
    connect(m_ribbon, &RibbonBar::languageChanged, this, &MainWindow::onLanguageChanged);

    // Ruler events
    connect(m_ruler, &RulerWidget::leftMarginChanged, this, &MainWindow::onRulerLeftMarginChanged);
    connect(m_ruler, &RulerWidget::rightMarginChanged, this, &MainWindow::onRulerRightMarginChanged);

    // Editor & Status bar events
    connect(m_editor, &DocumentEditor::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);
    connect(m_editor, &DocumentEditor::currentFormatChanged, m_ribbon, &RibbonBar::updateFormattingState);
    connect(m_editor, &DocumentEditor::zoomFactorChanged, this, [this](double factor) {
        int pct = qBound(10, qRound(factor * 100.0), 500);
        if (m_zoomWidget->zoomPercentage() != pct) {
            m_zoomWidget->setZoomPercentage(pct);
        }
    });
    connect(m_zoomWidget, &ZoomSliderWidget::zoomChanged, this, &MainWindow::onZoomChanged);

    // Ensure clean initial state
    m_docManager->setModified(false);
    m_editor->document()->setModified(false);
    updateTitle();
    m_ribbon->updateRecentFilesMenu(m_docManager->recentFiles());
}

void MainWindow::onLanguageChanged(const QString &langCode) {
    TranslationManager::instance().setLanguage(langCode);
    retranslateUi();
}

void MainWindow::retranslateUi() {
    m_ribbon->retranslateUi();
    m_statusLabel->setText(tr("Ready"));
    updateTitle();
}

void MainWindow::updateTitle() {
    QString title = m_docManager->documentTitle();
    if (m_docManager->isModified()) {
        title += QStringLiteral("*");
    }
    title += QStringLiteral(" - OpenWordPad");
    setWindowTitle(title);
}

void MainWindow::onDocumentModifiedChanged(bool) {
    updateTitle();
}

void MainWindow::onCursorPositionChanged(int line, int col, int charCount) {
    if (charCount > 0) {
        m_posLabel->setText(QStringLiteral("Ln %1, Col %2 (%3 chars selected)").arg(line).arg(col).arg(charCount));
    } else {
        m_posLabel->setText(QStringLiteral("Ln %1, Col %2").arg(line).arg(col));
    }
}

bool MainWindow::maybeSave() {
    if (!m_docManager) return true;

    // If not modified, no need to prompt
    if (!m_docManager->isModified()) return true;

    // If document is an unnamed empty document with no undo history
    if (m_docManager->currentFilePath().isEmpty() &&
        m_editor->document()->isEmpty() &&
        !m_editor->document()->isUndoAvailable()) {
        m_docManager->setModified(false);
        return true;
    }

    auto ret = QMessageBox::warning(this, QStringLiteral("OpenWordPad"),
        tr("Do you want to save changes to \"%1\"?").arg(m_docManager->documentTitle()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        if (m_docManager->currentFilePath().isEmpty()) {
            onSaveAs(m_docManager->currentFormat());
            return !m_docManager->isModified();
        }
        return m_docManager->saveFile();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::onNewFile() {
    if (maybeSave()) {
        m_docManager->newDocument();
        m_editor->document()->setModified(false);
        m_statusLabel->setText(QStringLiteral("Nuovo documento"));
    }
}

void MainWindow::onOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Apri"), QString(), DocumentManager::allSupportedFilters());
    if (fileName.isEmpty()) {
        return;
    }
    if (maybeSave()) {
        openFile(fileName);
    }
}

bool MainWindow::openFile(const QString &filePath) {
    bool ok = m_docManager->openFile(filePath);
    if (ok) {
        m_statusLabel->setText(QStringLiteral("Opened: ") + QFileInfo(filePath).fileName());
    }
    return ok;
}

void MainWindow::onSaveFile() {
    if (m_docManager->currentFilePath().isEmpty()) {
        onSaveAs(m_docManager->currentFormat());
    } else {
        m_docManager->saveFile();
        m_statusLabel->setText(QStringLiteral("Saved"));
    }
}

void MainWindow::onSaveAs(DocumentFormat format) {
    QString filter = (format == DocumentFormat::Unknown) ? DocumentManager::allSupportedFilters() : DocumentManager::filterForFormat(format);
    QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("Save As"), m_docManager->documentTitle(), filter);
    if (!fileName.isEmpty()) {
        m_docManager->saveFileAs(fileName, format);
        m_statusLabel->setText(QStringLiteral("Saved as ") + QFileInfo(fileName).fileName());
    }
}

void MainWindow::onPrint() {
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_editor->document()->print(&printer);
    }
}

void MainWindow::onQuickPrint() {
    QPrinter printer(QPrinter::HighResolution);
    m_editor->document()->print(&printer);
    m_statusLabel->setText(QStringLiteral("Printing sent to default printer"));
}

void MainWindow::onPrintPreview() {
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [this](QPrinter *p) {
        m_editor->document()->print(p);
    });
    preview.exec();
}

void MainWindow::onPageSetup() {
    PageSetupDialog dlg(m_docManager->pageSettings(), m_currentUnit, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_docManager->setPageSettings(dlg.pageSettings());
        m_editor->updatePageLayout(dlg.pageSettings());
        m_ruler->setMargins(dlg.pageSettings().leftMargin, dlg.pageSettings().rightMargin);
    }
}

void MainWindow::onSendEmail() {
    QString emailUrl = QStringLiteral("mailto:?subject=") + QUrl::toPercentEncoding(m_docManager->documentTitle())
                     + QStringLiteral("&body=") + QUrl::toPercentEncoding(m_editor->toPlainText());
    QDesktopServices::openUrl(QUrl(emailUrl));
}

void MainWindow::onAbout() {
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::onFontFamilyChanged(const QString &family) {
    m_editor->setFontFamily(family);
}

void MainWindow::onFontSizeChanged(const QString &sizeStr) {
    bool ok = false;
    double pt = sizeStr.toDouble(&ok);
    if (ok && pt > 0) {
        m_editor->setFontSize(pt);
    }
}

void MainWindow::onTextColorSelected(const QColor &color) {
    m_editor->setTextColor(color);
}

void MainWindow::onHighlightColorSelected(const QColor &color) {
    m_editor->setTextHighlightColor(color);
}

void MainWindow::onInsertPicture() {
    QString file = QFileDialog::getOpenFileName(this, QStringLiteral("Insert Picture"), QString(),
        QStringLiteral("Image Files (*.png *.jpg *.jpeg *.bmp *.svg *.webp *.gif);;All Files (*.*)"));
    if (!file.isEmpty()) {
        m_editor->insertImageFromFile(file);
    }
}

void MainWindow::onChangePicture() {
    onInsertPicture();
}

void MainWindow::onResizePicture() {
    ImageResizeDialog dlg(100, 100, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_editor->resizeSelectedImage(dlg.newWidth(), dlg.newHeight());
    }
}

void MainWindow::onPaintDrawing() {
    PaintDrawingDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_editor->insertImage(dlg.drawnImage(), QStringLiteral("drawing.png"));
    }
}

void MainWindow::onDateTime() {
    DateTimeDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_editor->insertDateTime(dlg.selectedDateTimeString());
    }
}

void MainWindow::onInsertObject() {
    onPaintDrawing();
}

void MainWindow::onFind() {
    if (!m_findReplaceDialog) {
        m_findReplaceDialog = new FindReplaceDialog(false, this);
        connect(m_findReplaceDialog, &FindReplaceDialog::findNextRequested, this, &MainWindow::onFindNext);
        connect(m_findReplaceDialog, &FindReplaceDialog::replaceRequested, this, &MainWindow::onReplaceText);
        connect(m_findReplaceDialog, &FindReplaceDialog::replaceAllRequested, this, &MainWindow::onReplaceAllText);
    }
    m_findReplaceDialog->setReplaceMode(false);
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
    m_findReplaceDialog->activateWindow();
}

void MainWindow::onReplace() {
    if (!m_findReplaceDialog) {
        m_findReplaceDialog = new FindReplaceDialog(true, this);
        connect(m_findReplaceDialog, &FindReplaceDialog::findNextRequested, this, &MainWindow::onFindNext);
        connect(m_findReplaceDialog, &FindReplaceDialog::replaceRequested, this, &MainWindow::onReplaceText);
        connect(m_findReplaceDialog, &FindReplaceDialog::replaceAllRequested, this, &MainWindow::onReplaceAllText);
    }
    m_findReplaceDialog->setReplaceMode(true);
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
    m_findReplaceDialog->activateWindow();
}

void MainWindow::onFindNext(const QString &text, QTextDocument::FindFlags flags) {
    bool found = m_editor->findText(text, flags);
    if (!found) {
        QMessageBox::information(this, QStringLiteral("OpenWordPad"), QStringLiteral("Finished searching the document."));
    }
}

void MainWindow::onReplaceText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags) {
    bool ok = m_editor->replaceText(findStr, replaceStr, flags);
    if (!ok) {
        QMessageBox::information(this, QStringLiteral("OpenWordPad"), QStringLiteral("Finished searching the document."));
    }
}

void MainWindow::onReplaceAllText(const QString &findStr, const QString &replaceStr, QTextDocument::FindFlags flags) {
    int count = m_editor->replaceAll(findStr, replaceStr, flags);
    QMessageBox::information(this, QStringLiteral("OpenWordPad"), QStringLiteral("Replaced %1 occurrence(s).").arg(count));
}

void MainWindow::onZoomChanged(int percent) {
    m_editor->setZoomFactor(percent / 100.0);
}

void MainWindow::onRulerToggled(bool visible) {
    m_ruler->setVisible(visible);
}

void MainWindow::onStatusBarToggled(bool visible) {
    statusBar()->setVisible(visible);
}

void MainWindow::onWordWrapChanged(int index) {
    WrapMode mode = static_cast<WrapMode>(m_ribbon->cbWordWrap->itemData(index).toInt());
    m_editor->setWrapMode(mode);
}

void MainWindow::onUnitsChanged(int index) {
    m_currentUnit = static_cast<UnitType>(m_ribbon->cbUnits->itemData(index).toInt());
    m_ruler->setUnit(m_currentUnit);
}

void MainWindow::onRulerLeftMarginChanged(double inches) {
    PageSettings s = m_docManager->pageSettings();
    s.leftMargin = inches;
    m_docManager->setPageSettings(s);
    m_editor->updatePageLayout(s);
}

void MainWindow::onRulerRightMarginChanged(double inches) {
    PageSettings s = m_docManager->pageSettings();
    s.rightMargin = inches;
    m_docManager->setPageSettings(s);
    m_editor->updatePageLayout(s);
}

void MainWindow::onParagraphDialog() {
    QTextBlockFormat bf = m_editor->textCursor().blockFormat();
    double leftInches = bf.leftMargin() * (1.0 / 96.0);
    double rightInches = bf.rightMargin() * (1.0 / 96.0);
    double firstLineInches = bf.textIndent() * (1.0 / 96.0);
    double ls = 1.0;
    if (bf.lineHeightType() == QTextBlockFormat::ProportionalHeight) {
        ls = bf.lineHeight() / 100.0;
    }
    bool spaceAfter = (bf.bottomMargin() > 0);

    ParagraphDialog dlg(leftInches, rightInches, firstLineInches, ls, spaceAfter, bf.alignment(), m_currentUnit, this);
    if (dlg.exec() == QDialog::Accepted) {
        bf.setLeftMargin(dlg.leftIndent() * 96.0);
        bf.setRightMargin(dlg.rightIndent() * 96.0);
        bf.setTextIndent(dlg.firstLineIndent() * 96.0);
        bf.setAlignment(dlg.alignment());
        m_editor->setLineSpacing(dlg.lineSpacing(), dlg.addSpaceAfter() ? 10 : 0);
        m_editor->textCursor().setBlockFormat(bf);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString localPath = urls.first().toLocalFile();
        if (!localPath.isEmpty()) {
            if (maybeSave()) {
                openFile(localPath);
            }
        }
    }
}

} // namespace OpenWordPad
