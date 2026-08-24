#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QLayout>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include "../core/Units.h"
#include "DocumentEditor.h"

namespace OpenWordPad {

class RibbonBar : public QWidget {
    Q_OBJECT

public:
    explicit RibbonBar(QWidget *parent = nullptr);

    // QAT actions
    QAction *actSave;
    QAction *actUndo;
    QAction *actRedo;
    QAction *actQuickPrint;
    QAction *actPrintPreview;

    // File Menu actions
    QAction *actNew;
    QAction *actOpen;
    QAction *actSaveAsRtf;
    QAction *actSaveAsDocx;
    QAction *actSaveAsOdt;
    QAction *actSaveAsTxt;
    QAction *actSaveAsOther;
    QAction *actPrint;
    QAction *actPageSetup;
    QAction *actEmail;
    QAction *actAbout;
    QAction *actExit;

    // Home Tab - Clipboard
    QAction *actPaste;
    QAction *actPasteSpecial;
    QAction *actCut;
    QAction *actCopy;

    // Home Tab - Font
    QComboBox *cbFontFamily;
    QComboBox *cbFontSize;
    QAction *actGrowFont;
    QAction *actShrinkFont;
    QAction *actBold;
    QAction *actItalic;
    QAction *actUnderline;
    QAction *actStrike;
    QAction *actSubscript;
    QAction *actSuperscript;
    QAction *actFontColor;
    QAction *actHighlightColor;

    // Home Tab - Paragraph
    QAction *actDecreaseIndent;
    QAction *actIncreaseIndent;
    QAction *actListNone;
    QAction *actListBullet;
    QAction *actListNumber;
    QAction *actListAlphaLower;
    QAction *actListAlphaUpper;
    QAction *actListRomanLower;
    QAction *actListRomanUpper;
    QAction *actLineSpacing10;
    QAction *actLineSpacing115;
    QAction *actLineSpacing15;
    QAction *actLineSpacing20;
    QAction *actLineSpacingAdd10pt;
    QAction *actAlignLeft;
    QAction *actAlignCenter;
    QAction *actAlignRight;
    QAction *actAlignJustify;
    QAction *actParagraphDialog;

    // Home Tab - Insert
    QAction *actInsertPicture;
    QAction *actChangePicture;
    QAction *actResizePicture;
    QAction *actPaintDrawing;
    QAction *actDateTime;
    QAction *actInsertObject;

    // Home Tab - Editing
    QAction *actFind;
    QAction *actReplace;
    QAction *actSelectAll;

    // View Tab - Zoom
    QAction *actZoomIn;
    QAction *actZoomOut;
    QAction *actZoom100;

    // View Tab - Show/Hide
    QCheckBox *chkRuler;
    QCheckBox *chkStatusBar;

    // View Tab - Settings
    QComboBox *cbWordWrap;
    QComboBox *cbUnits;

    void updateFormattingState(const QTextCharFormat &charFmt, const QTextBlockFormat &blockFmt);
    void updateRecentFilesMenu(const QStringList &files);

signals:
    void recentFileTriggered(const QString &filePath);
    void fontColorSelected(const QColor &color);
    void highlightColorSelected(const QColor &color);

private slots:
    void onFileMenuClicked();
    void onFontColorClicked();
    void onHighlightColorClicked();

private:
    void setupQAT(QHBoxLayout *topBarLayout);
    void setupFileMenu();
    void setupHomeTab();
    void setupViewTab();
    QWidget *createRibbonGroup(const QString &title, QLayout *contentLayout);
    QToolButton *createLargeButton(QAction *action, const QString &iconPath, const QString &text);
    QToolButton *createSmallButton(QAction *action, const QString &iconPath, const QString &tooltip = QString());

    QTabWidget *m_tabWidget;
    QToolButton *m_btnFile;
    QMenu *m_fileMenu;
    QMenu *m_recentMenu;
    QWidget *m_homeTab;
    QWidget *m_viewTab;
};

} // namespace OpenWordPad
