#include "RibbonBar.h"
#include "../core/TranslationManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QPainter>
#include <QActionGroup>
#include <QFileInfo>
#include <QListView>
#include <QFontDatabase>

namespace OpenWordPad {

RibbonBar::RibbonBar(QWidget *parent)
    : QWidget(parent)
    , m_lblWordWrap(nullptr)
    , m_lblUnits(nullptr)
    , m_lblLanguage(nullptr)
    , m_lblGroupClipboard(nullptr)
    , m_lblGroupFont(nullptr)
    , m_lblGroupPara(nullptr)
    , m_lblGroupInsert(nullptr)
    , m_lblGroupEdit(nullptr)
    , m_lblGroupZoom(nullptr)
    , m_lblGroupShowHide(nullptr)
    , m_lblGroupSettings(nullptr)
    , m_btnPaste(nullptr)
    , m_btnCut(nullptr)
    , m_btnCopy(nullptr)
    , m_btnPic(nullptr)
    , m_btnPaint(nullptr)
    , m_btnDate(nullptr)
    , m_btnObj(nullptr)
    , m_btnFind(nullptr)
    , m_btnReplace(nullptr)
    , m_btnSelectAll(nullptr)
    , m_btnZoomIn(nullptr)
    , m_btnZoomOut(nullptr)
    , m_btnZoom100(nullptr)
{
    setFixedHeight(154);
    setStyleSheet(
        "QWidget { font-family: 'Segoe UI', 'DejaVu Sans', sans-serif; font-size: 9pt; }"
        "QTabWidget::pane { border-top: 1px solid #d4d4d4; background: #fdfdfd; }"
        "QTabBar::tab { background: #f0f0f0; border: 1px solid #d4d4d4; border-bottom: none; padding: 5px 18px; margin-right: 2px; border-top-left-radius: 3px; border-top-right-radius: 3px; color: #333; }"
        "QTabBar::tab:selected { background: #fdfdfd; border-color: #d4d4d4; border-bottom: 1px solid #fdfdfd; font-weight: bold; color: #0078d7; }"
        "QTabBar::tab:hover:!selected { background: #e5f1fb; color: #000; }"
        "QToolButton { border: 1px solid transparent; border-radius: 2px; background: transparent; padding: 2px; color: #000000; }"
        "QToolButton:hover { background: #e5f1fb; border: 1px solid #70c0e7; }"
        "QToolButton:pressed { background: #cce4f7; border: 1px solid #005499; }"
        "QToolButton:checked { background: #cce4f7; border: 1px solid #005499; }"
        "QToolButton::menu-button { border: none; border-left: 1px solid rgba(0, 0, 0, 0.15); background: transparent; width: 14px; }"
        "QToolButton::menu-button:hover { background: rgba(0, 120, 215, 0.15); }"
        "QToolButton::menu-arrow { image: url(:/icons/arrow_down.svg); width: 8px; height: 8px; }"
        "QComboBox { border: 1px solid #c0c0c0; border-radius: 2px; padding: 1px 4px; background: #ffffff; color: #000000; font-size: 9pt; height: 22px; }"
        "QComboBox:hover { border: 1px solid #70c0e7; }"
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 16px; border-left: 1px solid #e0e0e0; background: #f6f6f6; }"
        "QComboBox::drop-down:hover { background: #e5f1fb; }"
        "QComboBox::down-arrow { image: url(:/icons/arrow_down.svg); width: 8px; height: 8px; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #000000; selection-background-color: #0078d7; selection-color: #ffffff; border: 1px solid #70c0e7; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 22px; padding: 2px 6px; color: #000000; background-color: #ffffff; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #0078d7; color: #ffffff; }"
        "QComboBox QAbstractItemView::item:selected { background-color: #0078d7; color: #ffffff; }"
    );

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Top Row: Quick Access Toolbar & File Button
    auto topBar = new QWidget(this);
    topBar->setFixedHeight(32);
    topBar->setStyleSheet("background: #f0f0f0; border-bottom: 1px solid #d4d4d4;");
    auto topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(4, 2, 4, 2);
    topBarLayout->setSpacing(4);

    // File Application Menu Button (Blue styling)
    m_btnFile = new QToolButton(topBar);
    m_btnFile->setText(QStringLiteral("  ") + tr("File") + QStringLiteral("  "));
    m_btnFile->setPopupMode(QToolButton::InstantPopup);
    m_btnFile->setStyleSheet(
        "QToolButton { background: #1976d2; color: white; font-weight: bold; border-radius: 2px; padding: 4px 14px; }"
        "QToolButton:hover { background: #1565c0; }"
        "QToolButton:pressed { background: #0d47a1; }"
    );
    topBarLayout->addWidget(m_btnFile);

    setupQAT(topBarLayout);
    topBarLayout->addStretch();
    mainLayout->addWidget(topBar);

    // Ribbon Tabs
    m_tabWidget = new QTabWidget(this);
    m_homeTab = new QWidget(m_tabWidget);
    m_viewTab = new QWidget(m_tabWidget);

    m_tabWidget->addTab(m_homeTab, tr("Home"));
    m_tabWidget->addTab(m_viewTab, tr("View"));

    setupFileMenu();
    setupHomeTab();
    setupViewTab();

    mainLayout->addWidget(m_tabWidget);
}

void RibbonBar::setupQAT(QHBoxLayout *topBarLayout) {
    actSave = new QAction(QIcon(QStringLiteral(":/icons/save.svg")), tr("Save"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setToolTip(tr("Save") + QStringLiteral(" (Ctrl+S)"));

    actUndo = new QAction(QIcon(QStringLiteral(":/icons/undo.svg")), QStringLiteral("Undo"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setToolTip(QStringLiteral("Undo (Ctrl+Z)"));

    actRedo = new QAction(QIcon(QStringLiteral(":/icons/redo.svg")), QStringLiteral("Redo"), this);
    actRedo->setShortcut(QKeySequence::Redo);
    actRedo->setToolTip(QStringLiteral("Redo (Ctrl+Y)"));

    actQuickPrint = new QAction(QIcon(QStringLiteral(":/icons/quick_print.svg")), tr("Quick Print"), this);
    actQuickPrint->setToolTip(tr("Quick Print"));

    actPrintPreview = new QAction(QIcon(QStringLiteral(":/icons/print_preview.svg")), tr("Print Preview"), this);
    actPrintPreview->setToolTip(tr("Print Preview"));

    topBarLayout->addWidget(createSmallButton(actSave, QStringLiteral(":/icons/save.svg"), tr("Save") + QStringLiteral(" (Ctrl+S)")));
    topBarLayout->addWidget(createSmallButton(actUndo, QStringLiteral(":/icons/undo.svg"), QStringLiteral("Undo (Ctrl+Z)")));
    topBarLayout->addWidget(createSmallButton(actRedo, QStringLiteral(":/icons/redo.svg"), QStringLiteral("Redo (Ctrl+Y)")));
}

void RibbonBar::setupFileMenu() {
    m_fileMenu = new QMenu(this);

    actNew = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/new.svg")), tr("New"));
    actNew->setShortcut(QKeySequence::New);

    actOpen = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/open.svg")), tr("Open..."));
    actOpen->setShortcut(QKeySequence::Open);

    m_fileMenu->addAction(actSave);

    // Save As Submenu
    QMenu *saveAsMenu = m_fileMenu->addMenu(QIcon(QStringLiteral(":/icons/save_as.svg")), tr("Save As..."));
    actSaveAsRtf = saveAsMenu->addAction(QStringLiteral("Rich Text Document (*.rtf)"));
    actSaveAsDocx = saveAsMenu->addAction(QStringLiteral("Office Open XML Document (*.docx)"));
    actSaveAsOdt = saveAsMenu->addAction(QStringLiteral("OpenDocument Text (*.odt)"));
    actSaveAsTxt = saveAsMenu->addAction(QStringLiteral("Plain Text Document (*.txt)"));
    actSaveAsOther = saveAsMenu->addAction(QStringLiteral("Other formats (HTML, PDF)..."));

    // Print Submenu
    QMenu *printMenu = m_fileMenu->addMenu(QIcon(QStringLiteral(":/icons/print.svg")), tr("Print..."));
    actPrint = printMenu->addAction(QIcon(QStringLiteral(":/icons/print.svg")), tr("Print..."));
    actPrint->setShortcut(QKeySequence::Print);
    printMenu->addAction(actQuickPrint);
    printMenu->addAction(actPrintPreview);

    actPageSetup = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/page_setup.svg")), tr("Page Setup..."));
    actEmail = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/email.svg")), tr("Send in email"));
    
    m_fileMenu->addSeparator();
    m_recentMenu = m_fileMenu->addMenu(QStringLiteral("Recent documents"));
    
    m_fileMenu->addSeparator();
    actAbout = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/about.svg")), tr("About WordPad"));
    actExit = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/exit.svg")), tr("Exit"));

    m_btnFile->setMenu(m_fileMenu);
}

void RibbonBar::setupHomeTab() {
    auto homeLayout = new QHBoxLayout(m_homeTab);
    homeLayout->setContentsMargins(6, 3, 6, 3);
    homeLayout->setSpacing(6);

    // 1. Clipboard Group
    actPaste = new QAction(QIcon(QStringLiteral(":/icons/paste.svg")), tr("Paste"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    actPasteSpecial = new QAction(QIcon(QStringLiteral(":/icons/paste.svg")), tr("Paste Special..."), this);
    actCut = new QAction(QIcon(QStringLiteral(":/icons/cut.svg")), tr("Cut"), this);
    actCut->setShortcut(QKeySequence::Cut);
    actCopy = new QAction(QIcon(QStringLiteral(":/icons/copy.svg")), tr("Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);

    auto clipLayout = new QHBoxLayout();
    clipLayout->setContentsMargins(0, 0, 0, 0);
    clipLayout->setSpacing(4);
    
    m_btnPaste = createLargeButton(actPaste, QStringLiteral(":/icons/paste.svg"), tr("Paste"));
    m_btnPaste->setFixedSize(68, 66);
    QMenu *pasteMenu = new QMenu(this);
    pasteMenu->addAction(actPaste);
    pasteMenu->addAction(actPasteSpecial);
    m_btnPaste->setMenu(pasteMenu);
    m_btnPaste->setPopupMode(QToolButton::MenuButtonPopup);
    clipLayout->addWidget(m_btnPaste);

    auto clipSmallLayout = new QVBoxLayout();
    clipSmallLayout->setContentsMargins(0, 2, 0, 2);
    clipSmallLayout->setSpacing(3);

    m_btnCut = new QToolButton(this);
    m_btnCut->setDefaultAction(actCut);
    m_btnCut->setIcon(QIcon(QStringLiteral(":/icons/cut.svg")));
    m_btnCut->setText(QStringLiteral(" ") + tr("Cut"));
    m_btnCut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_btnCut->setFixedHeight(22);
    m_btnCut->setMinimumWidth(64);
    m_btnCut->setStyleSheet("QToolButton { font-size: 8pt; padding: 2px; }");
    clipSmallLayout->addWidget(m_btnCut);

    m_btnCopy = new QToolButton(this);
    m_btnCopy->setDefaultAction(actCopy);
    m_btnCopy->setIcon(QIcon(QStringLiteral(":/icons/copy.svg")));
    m_btnCopy->setText(QStringLiteral(" ") + tr("Copy"));
    m_btnCopy->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_btnCopy->setFixedHeight(22);
    m_btnCopy->setMinimumWidth(64);
    m_btnCopy->setStyleSheet("QToolButton { font-size: 8pt; padding: 2px; }");
    clipSmallLayout->addWidget(m_btnCopy);

    clipLayout->addLayout(clipSmallLayout);
    homeLayout->addWidget(createRibbonGroup(tr("Clipboard"), clipLayout, &m_lblGroupClipboard));

    // 2. Font Group
    auto fontMainLayout = new QVBoxLayout();
    fontMainLayout->setContentsMargins(0, 0, 0, 0);
    fontMainLayout->setSpacing(3);

    // Row 1: Font Family, Font Size, Grow, Shrink
    auto fontRow1 = new QHBoxLayout();
    fontRow1->setContentsMargins(0, 0, 0, 0);
    fontRow1->setSpacing(3);
    cbFontFamily = new QComboBox(this);
    cbFontFamily->setView(new QListView(cbFontFamily));
    cbFontFamily->setEditable(true);
    cbFontFamily->setFixedWidth(145);
    QStringList fontList = { QStringLiteral("Calibri"), QStringLiteral("Arial"), QStringLiteral("Segoe UI"), QStringLiteral("Times New Roman"), QStringLiteral("Courier New"), QStringLiteral("Verdana"), QStringLiteral("Tahoma"), QStringLiteral("Georgia"), QStringLiteral("Comic Sans MS") };
    fontList.append(QFontDatabase::families());
    fontList.removeDuplicates();
    cbFontFamily->addItems(fontList);
    cbFontFamily->setCurrentText(QStringLiteral("Calibri"));

    cbFontSize = new QComboBox(this);
    cbFontSize->setView(new QListView(cbFontSize));
    cbFontSize->setEditable(true);
    cbFontSize->setFixedWidth(52);
    QList<int> sizes = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72 };
    for (int s : sizes) cbFontSize->addItem(QString::number(s));
    cbFontSize->setCurrentText(QStringLiteral("11"));

    actGrowFont = new QAction(QIcon(QStringLiteral(":/icons/font_grow.svg")), tr("Grow font"), this);
    actShrinkFont = new QAction(QIcon(QStringLiteral(":/icons/font_shrink.svg")), tr("Shrink font"), this);

    fontRow1->addWidget(cbFontFamily);
    fontRow1->addWidget(cbFontSize);
    fontRow1->addWidget(createSmallButton(actGrowFont, QStringLiteral(":/icons/font_grow.svg"), tr("Grow font") + QStringLiteral(" (Ctrl+Shift+>)")));
    fontRow1->addWidget(createSmallButton(actShrinkFont, QStringLiteral(":/icons/font_shrink.svg"), tr("Shrink font") + QStringLiteral(" (Ctrl+Shift+<)")));
    fontRow1->addStretch();
    fontMainLayout->addLayout(fontRow1);

    // Row 2: Bold, Italic, Underline, Strikethrough, Subscript, Superscript, Font Color, Highlight Color
    auto fontRow2 = new QHBoxLayout();
    fontRow2->setContentsMargins(0, 0, 0, 0);
    fontRow2->setSpacing(2);

    actBold = new QAction(QIcon(QStringLiteral(":/icons/bold.svg")), tr("Bold"), this);
    actBold->setCheckable(true);
    actBold->setShortcut(QKeySequence::Bold);
    actItalic = new QAction(QIcon(QStringLiteral(":/icons/italic.svg")), tr("Italic"), this);
    actItalic->setCheckable(true);
    actItalic->setShortcut(QKeySequence::Italic);
    actUnderline = new QAction(QIcon(QStringLiteral(":/icons/underline.svg")), tr("Underline"), this);
    actUnderline->setCheckable(true);
    actUnderline->setShortcut(QKeySequence::Underline);
    actStrike = new QAction(QIcon(QStringLiteral(":/icons/strikethrough.svg")), tr("Strikethrough"), this);
    actStrike->setCheckable(true);
    actSubscript = new QAction(QIcon(QStringLiteral(":/icons/subscript.svg")), tr("Subscript"), this);
    actSubscript->setCheckable(true);
    actSuperscript = new QAction(QIcon(QStringLiteral(":/icons/superscript.svg")), tr("Superscript"), this);
    actSuperscript->setCheckable(true);
    
    actFontColor = new QAction(QIcon(QStringLiteral(":/icons/font_color.svg")), tr("Text color"), this);
    actHighlightColor = new QAction(QIcon(QStringLiteral(":/icons/highlight_color.svg")), tr("Text highlight color"), this);

    connect(actFontColor, &QAction::triggered, this, &RibbonBar::onFontColorClicked);
    connect(actHighlightColor, &QAction::triggered, this, &RibbonBar::onHighlightColorClicked);

    fontRow2->addWidget(createSmallButton(actBold, QStringLiteral(":/icons/bold.svg"), tr("Bold") + QStringLiteral(" (Ctrl+B)")));
    fontRow2->addWidget(createSmallButton(actItalic, QStringLiteral(":/icons/italic.svg"), tr("Italic") + QStringLiteral(" (Ctrl+I)")));
    fontRow2->addWidget(createSmallButton(actUnderline, QStringLiteral(":/icons/underline.svg"), tr("Underline") + QStringLiteral(" (Ctrl+U)")));
    fontRow2->addWidget(createSmallButton(actStrike, QStringLiteral(":/icons/strikethrough.svg"), tr("Strikethrough")));
    fontRow2->addWidget(createSmallButton(actSubscript, QStringLiteral(":/icons/subscript.svg"), tr("Subscript")));
    fontRow2->addWidget(createSmallButton(actSuperscript, QStringLiteral(":/icons/superscript.svg"), tr("Superscript")));
    fontRow2->addSpacing(3);
    fontRow2->addWidget(createSmallButton(actFontColor, QStringLiteral(":/icons/font_color.svg"), tr("Text color")));
    fontRow2->addWidget(createSmallButton(actHighlightColor, QStringLiteral(":/icons/highlight_color.svg"), tr("Text highlight color")));
    fontRow2->addStretch();
    fontMainLayout->addLayout(fontRow2);

    homeLayout->addWidget(createRibbonGroup(tr("Font"), fontMainLayout, &m_lblGroupFont));

    // 3. Paragraph Group
    auto paraMainLayout = new QVBoxLayout();
    paraMainLayout->setContentsMargins(0, 0, 0, 0);
    paraMainLayout->setSpacing(3);

    // Row 1: Indents, Lists, Line Spacing
    auto paraRow1 = new QHBoxLayout();
    paraRow1->setContentsMargins(0, 0, 0, 0);
    paraRow1->setSpacing(2);

    actDecreaseIndent = new QAction(QIcon(QStringLiteral(":/icons/indent_dec.svg")), tr("Decrease indent"), this);
    actIncreaseIndent = new QAction(QIcon(QStringLiteral(":/icons/indent_inc.svg")), tr("Increase indent"), this);

    // List Style Menu
    auto btnList = new QToolButton(this);
    btnList->setIcon(QIcon(QStringLiteral(":/icons/list_bullets.svg")));
    btnList->setPopupMode(QToolButton::InstantPopup);
    btnList->setToolTip(tr("Start a list"));
    btnList->setFixedSize(22, 22);
    QMenu *listMenu = new QMenu(this);
    actListNone = listMenu->addAction(QStringLiteral("None"));
    actListBullet = listMenu->addAction(QStringLiteral("• Bulleted list"));
    actListNumber = listMenu->addAction(QStringLiteral("1, 2, 3 Numbered"));
    actListAlphaLower = listMenu->addAction(QStringLiteral("a, b, c Lowercase"));
    actListAlphaUpper = listMenu->addAction(QStringLiteral("A, B, C Uppercase"));
    actListRomanLower = listMenu->addAction(QStringLiteral("i, ii, iii Roman lowercase"));
    actListRomanUpper = listMenu->addAction(QStringLiteral("I, II, III Roman uppercase"));
    btnList->setMenu(listMenu);

    // Line Spacing Menu
    auto btnLineSpacing = new QToolButton(this);
    btnLineSpacing->setIcon(QIcon(QStringLiteral(":/icons/line_spacing.svg")));
    btnLineSpacing->setPopupMode(QToolButton::InstantPopup);
    btnLineSpacing->setToolTip(tr("Line spacing"));
    btnLineSpacing->setFixedSize(22, 22);
    QMenu *lsMenu = new QMenu(this);
    actLineSpacing10 = lsMenu->addAction(QStringLiteral("1.0"));
    actLineSpacing115 = lsMenu->addAction(QStringLiteral("1.15"));
    actLineSpacing15 = lsMenu->addAction(QStringLiteral("1.5"));
    actLineSpacing20 = lsMenu->addAction(QStringLiteral("2.0"));
    lsMenu->addSeparator();
    actLineSpacingAdd10pt = lsMenu->addAction(QStringLiteral("Add 10pt space after paragraphs"));
    actLineSpacingAdd10pt->setCheckable(true);
    actLineSpacingAdd10pt->setChecked(true);
    btnLineSpacing->setMenu(lsMenu);

    actParagraphDialog = new QAction(QIcon(QStringLiteral(":/icons/paragraph.svg")), tr("Paragraph"), this);

    paraRow1->addWidget(createSmallButton(actDecreaseIndent, QStringLiteral(":/icons/indent_dec.svg"), tr("Decrease indent")));
    paraRow1->addWidget(createSmallButton(actIncreaseIndent, QStringLiteral(":/icons/indent_inc.svg"), tr("Increase indent")));
    paraRow1->addWidget(btnList);
    paraRow1->addWidget(btnLineSpacing);
    paraRow1->addStretch();
    paraMainLayout->addLayout(paraRow1);

    // Row 2: Alignments & Dialog launcher
    auto paraRow2 = new QHBoxLayout();
    paraRow2->setContentsMargins(0, 0, 0, 0);
    paraRow2->setSpacing(2);

    actAlignLeft = new QAction(QIcon(QStringLiteral(":/icons/align_left.svg")), tr("Align text left"), this);
    actAlignLeft->setCheckable(true);
    actAlignLeft->setChecked(true);
    actAlignLeft->setShortcut(QKeySequence(QStringLiteral("Ctrl+L")));

    actAlignCenter = new QAction(QIcon(QStringLiteral(":/icons/align_center.svg")), tr("Center"), this);
    actAlignCenter->setCheckable(true);
    actAlignCenter->setShortcut(QKeySequence(QStringLiteral("Ctrl+E")));

    actAlignRight = new QAction(QIcon(QStringLiteral(":/icons/align_right.svg")), tr("Align text right"), this);
    actAlignRight->setCheckable(true);
    actAlignRight->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));

    actAlignJustify = new QAction(QIcon(QStringLiteral(":/icons/align_justify.svg")), tr("Justify"), this);
    actAlignJustify->setCheckable(true);
    actAlignJustify->setShortcut(QKeySequence(QStringLiteral("Ctrl+J")));

    auto alignGroup = new QActionGroup(this);
    alignGroup->addAction(actAlignLeft);
    alignGroup->addAction(actAlignCenter);
    alignGroup->addAction(actAlignRight);
    alignGroup->addAction(actAlignJustify);

    paraRow2->addWidget(createSmallButton(actAlignLeft, QStringLiteral(":/icons/align_left.svg"), tr("Align text left") + QStringLiteral(" (Ctrl+L)")));
    paraRow2->addWidget(createSmallButton(actAlignCenter, QStringLiteral(":/icons/align_center.svg"), tr("Center") + QStringLiteral(" (Ctrl+E)")));
    paraRow2->addWidget(createSmallButton(actAlignRight, QStringLiteral(":/icons/align_right.svg"), tr("Align text right") + QStringLiteral(" (Ctrl+R)")));
    paraRow2->addWidget(createSmallButton(actAlignJustify, QStringLiteral(":/icons/align_justify.svg"), tr("Justify") + QStringLiteral(" (Ctrl+J)")));
    paraRow2->addWidget(createSmallButton(actParagraphDialog, QStringLiteral(":/icons/paragraph.svg"), tr("Paragraph")));
    paraRow2->addStretch();
    paraMainLayout->addLayout(paraRow2);

    homeLayout->addWidget(createRibbonGroup(tr("Paragraph"), paraMainLayout, &m_lblGroupPara));

    // 4. Insert Group
    auto insertLayout = new QHBoxLayout();
    insertLayout->setContentsMargins(0, 0, 0, 0);
    insertLayout->setSpacing(4);

    actInsertPicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), tr("Picture"), this);
    actChangePicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), tr("Change picture"), this);
    actResizePicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), tr("Resize picture"), this);

    m_btnPic = createLargeButton(actInsertPicture, QStringLiteral(":/icons/picture.svg"), tr("Picture"));
    m_btnPic->setFixedSize(82, 66);
    QMenu *picMenu = new QMenu(this);
    picMenu->addAction(actInsertPicture);
    picMenu->addAction(actChangePicture);
    picMenu->addAction(actResizePicture);
    m_btnPic->setMenu(picMenu);
    m_btnPic->setPopupMode(QToolButton::MenuButtonPopup);
    insertLayout->addWidget(m_btnPic);

    actPaintDrawing = new QAction(QIcon(QStringLiteral(":/icons/paint.svg")), tr("Paint drawing"), this);
    m_btnPaint = createLargeButton(actPaintDrawing, QStringLiteral(":/icons/paint.svg"), tr("Paint drawing"));
    m_btnPaint->setFixedSize(76, 66);
    insertLayout->addWidget(m_btnPaint);

    actDateTime = new QAction(QIcon(QStringLiteral(":/icons/datetime.svg")), tr("Date and time"), this);
    m_btnDate = createLargeButton(actDateTime, QStringLiteral(":/icons/datetime.svg"), tr("Date and time"));
    m_btnDate->setFixedSize(68, 66);
    insertLayout->addWidget(m_btnDate);

    actInsertObject = new QAction(QIcon(QStringLiteral(":/icons/object.svg")), tr("Insert object"), this);
    m_btnObj = createLargeButton(actInsertObject, QStringLiteral(":/icons/object.svg"), tr("Insert object"));
    m_btnObj->setFixedSize(78, 66);
    insertLayout->addWidget(m_btnObj);

    homeLayout->addWidget(createRibbonGroup(tr("Insert"), insertLayout, &m_lblGroupInsert));

    // 5. Editing Group
    auto editLayout = new QVBoxLayout();
    editLayout->setContentsMargins(0, 2, 0, 2);
    editLayout->setSpacing(2);

    actFind = new QAction(QIcon(QStringLiteral(":/icons/find.svg")), tr("Find"), this);
    actFind->setShortcut(QKeySequence::Find);
    actReplace = new QAction(QIcon(QStringLiteral(":/icons/replace.svg")), tr("Replace"), this);
    actReplace->setShortcut(QKeySequence(QStringLiteral("Ctrl+H")));
    actSelectAll = new QAction(QIcon(QStringLiteral(":/icons/select_all.svg")), tr("Select all"), this);
    actSelectAll->setShortcut(QKeySequence::SelectAll);

    auto makeEditBtn = [&](QAction *act, const QString &icon, const QString &text) {
        auto btn = new QToolButton(this);
        btn->setDefaultAction(act);
        btn->setIcon(QIcon(icon));
        btn->setText(text);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btn->setFixedHeight(20);
        btn->setStyleSheet("QToolButton { font-size: 8pt; padding: 1px 3px; }");
        return btn;
    };

    m_btnFind = makeEditBtn(actFind, QStringLiteral(":/icons/find.svg"), QStringLiteral(" ") + tr("Find"));
    m_btnReplace = makeEditBtn(actReplace, QStringLiteral(":/icons/replace.svg"), QStringLiteral(" ") + tr("Replace"));
    m_btnSelectAll = makeEditBtn(actSelectAll, QStringLiteral(":/icons/select_all.svg"), QStringLiteral(" ") + tr("Select all"));

    editLayout->addWidget(m_btnFind);
    editLayout->addWidget(m_btnReplace);
    editLayout->addWidget(m_btnSelectAll);

    homeLayout->addWidget(createRibbonGroup(tr("Editing"), editLayout, &m_lblGroupEdit));
    homeLayout->addStretch();
}

void RibbonBar::setupViewTab() {
    auto viewLayout = new QHBoxLayout(m_viewTab);
    viewLayout->setContentsMargins(6, 3, 6, 3);
    viewLayout->setSpacing(6);

    // 1. Zoom Group
    auto zoomLayout = new QHBoxLayout();
    zoomLayout->setContentsMargins(0, 0, 0, 0);
    zoomLayout->setSpacing(4);
    actZoomIn = new QAction(QIcon(QStringLiteral(":/icons/zoom_in.svg")), tr("Zoom in"), this);
    actZoomOut = new QAction(QIcon(QStringLiteral(":/icons/zoom_out.svg")), tr("Zoom out"), this);
    actZoom100 = new QAction(QIcon(QStringLiteral(":/icons/zoom_100.svg")), tr("100%"), this);

    m_btnZoomIn = createLargeButton(actZoomIn, QStringLiteral(":/icons/zoom_in.svg"), tr("Zoom in"));
    m_btnZoomIn->setFixedSize(68, 66);
    m_btnZoomOut = createLargeButton(actZoomOut, QStringLiteral(":/icons/zoom_out.svg"), tr("Zoom out"));
    m_btnZoomOut->setFixedSize(68, 66);
    m_btnZoom100 = createLargeButton(actZoom100, QStringLiteral(":/icons/zoom_100.svg"), tr("100%"));
    m_btnZoom100->setFixedSize(68, 66);

    zoomLayout->addWidget(m_btnZoomIn);
    zoomLayout->addWidget(m_btnZoomOut);
    zoomLayout->addWidget(m_btnZoom100);
    viewLayout->addWidget(createRibbonGroup(tr("Zoom"), zoomLayout, &m_lblGroupZoom));

    // 2. Show or Hide Group
    auto showHideLayout = new QVBoxLayout();
    showHideLayout->setContentsMargins(0, 0, 0, 0);
    showHideLayout->setSpacing(2);
    chkRuler = new QCheckBox(tr("Ruler"), this);
    chkRuler->setChecked(true);
    chkStatusBar = new QCheckBox(tr("Status bar"), this);
    chkStatusBar->setChecked(true);
    showHideLayout->addWidget(chkRuler);
    showHideLayout->addWidget(chkStatusBar);
    showHideLayout->addStretch();
    viewLayout->addWidget(createRibbonGroup(tr("Show or hide"), showHideLayout, &m_lblGroupShowHide));

    // 3. Settings Group
    auto settingsLayout = new QVBoxLayout();
    settingsLayout->setContentsMargins(0, 0, 0, 0);
    settingsLayout->setSpacing(2);

    auto wrapLayout = new QHBoxLayout();
    wrapLayout->setContentsMargins(0, 0, 0, 0);
    m_lblWordWrap = new QLabel(tr("Word wrap:"), this);
    wrapLayout->addWidget(m_lblWordWrap);
    cbWordWrap = new QComboBox(this);
    cbWordWrap->setView(new QListView(cbWordWrap));
    cbWordWrap->addItem(tr("No wrap"), static_cast<int>(WrapMode::NoWrap));
    cbWordWrap->addItem(tr("Wrap to window"), static_cast<int>(WrapMode::WrapToWindow));
    cbWordWrap->addItem(tr("Wrap to ruler"), static_cast<int>(WrapMode::WrapToRuler));
    cbWordWrap->setCurrentIndex(2); // Wrap to ruler default
    wrapLayout->addWidget(cbWordWrap);
    settingsLayout->addLayout(wrapLayout);

    auto unitsLayout = new QHBoxLayout();
    unitsLayout->setContentsMargins(0, 0, 0, 0);
    m_lblUnits = new QLabel(tr("Measurement units:"), this);
    unitsLayout->addWidget(m_lblUnits);
    cbUnits = new QComboBox(this);
    cbUnits->setView(new QListView(cbUnits));
    cbUnits->addItem(tr("Inches"), static_cast<int>(UnitType::Inches));
    cbUnits->addItem(tr("Centimeters"), static_cast<int>(UnitType::Centimeters));
    cbUnits->addItem(tr("Points"), static_cast<int>(UnitType::Points));
    cbUnits->addItem(tr("Picas"), static_cast<int>(UnitType::Picas));
    cbUnits->setCurrentIndex(1); // Centimeters default
    unitsLayout->addWidget(cbUnits);
    settingsLayout->addLayout(unitsLayout);

    auto langLayout = new QHBoxLayout();
    langLayout->setContentsMargins(0, 0, 0, 0);
    m_lblLanguage = new QLabel(tr("Language:"), this);
    langLayout->addWidget(m_lblLanguage);
    cbLanguage = new QComboBox(this);
    cbLanguage->setView(new QListView(cbLanguage));
    
    for (const auto &info : TranslationManager::instance().availableLanguages()) {
        cbLanguage->addItem(info.nativeName, info.code);
    }
    QString curLang = TranslationManager::instance().currentLanguage();
    for (int i = 0; i < cbLanguage->count(); ++i) {
        if (cbLanguage->itemData(i).toString() == curLang) {
            cbLanguage->setCurrentIndex(i);
            break;
        }
    }
    connect(cbLanguage, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        QString langCode = cbLanguage->itemData(idx).toString();
        emit languageChanged(langCode);
    });
    langLayout->addWidget(cbLanguage);
    settingsLayout->addLayout(langLayout);

    viewLayout->addWidget(createRibbonGroup(tr("Settings"), settingsLayout, &m_lblGroupSettings));
    viewLayout->addStretch();
}

QWidget *RibbonBar::createRibbonGroup(const QString &title, QLayout *contentLayout, QLabel **outLabel) {
    auto groupWidget = new QWidget(this);
    auto vLayout = new QVBoxLayout(groupWidget);
    vLayout->setContentsMargins(6, 2, 6, 2);
    vLayout->setSpacing(2);

    auto contentWidget = new QWidget(groupWidget);
    contentWidget->setLayout(contentLayout);
    vLayout->addWidget(contentWidget, 1);

    auto lblTitle = new QLabel(title, groupWidget);
    lblTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lblTitle->setFixedHeight(15);
    lblTitle->setStyleSheet("color: #717171; font-size: 8pt; font-family: 'Segoe UI', 'DejaVu Sans', sans-serif; font-weight: normal; margin: 0; padding: 0; border: none;");
    vLayout->addWidget(lblTitle, 0);

    if (outLabel) {
        *outLabel = lblTitle;
    }

    groupWidget->setStyleSheet("QWidget { border-right: 1px solid #e0e0e0; }");
    return groupWidget;
}

QToolButton *RibbonBar::createLargeButton(QAction *action, const QString &iconPath, const QString &text) {
    auto btn = new QToolButton(this);
    btn->setDefaultAction(action);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(32, 32));
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMinimumWidth(66);
    btn->setFixedHeight(66);
    btn->setStyleSheet("QToolButton { font-size: 8pt; padding: 2px 2px; }");
    return btn;
}

QToolButton *RibbonBar::createSmallButton(QAction *action, const QString &iconPath, const QString &tooltip) {
    auto btn = new QToolButton(this);
    btn->setDefaultAction(action);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(16, 16));
    if (!tooltip.isEmpty()) btn->setToolTip(tooltip);
    btn->setFixedSize(22, 22);
    return btn;
}

void RibbonBar::retranslateUi() {
    m_tabWidget->setTabText(0, tr("Home"));
    m_tabWidget->setTabText(1, tr("View"));

    m_btnFile->setText(QStringLiteral("  ") + tr("File") + QStringLiteral("  "));

    // Actions
    actNew->setText(tr("New"));
    actOpen->setText(tr("Open..."));
    actSave->setText(tr("Save"));
    actSaveAsRtf->setText(tr("Save As..."));
    actPrint->setText(tr("Print..."));
    actQuickPrint->setText(tr("Quick Print"));
    actPrintPreview->setText(tr("Print Preview"));
    actPageSetup->setText(tr("Page Setup..."));
    actEmail->setText(tr("Send in email"));
    actAbout->setText(tr("About WordPad"));
    actExit->setText(tr("Exit"));

    actPaste->setText(tr("Paste"));
    actPasteSpecial->setText(tr("Paste Special..."));
    actCut->setText(tr("Cut"));
    actCopy->setText(tr("Copy"));

    actGrowFont->setText(tr("Grow font"));
    actShrinkFont->setText(tr("Shrink font"));
    actBold->setText(tr("Bold"));
    actItalic->setText(tr("Italic"));
    actUnderline->setText(tr("Underline"));
    actStrike->setText(tr("Strikethrough"));
    actSubscript->setText(tr("Subscript"));
    actSuperscript->setText(tr("Superscript"));
    actFontColor->setText(tr("Text color"));
    actHighlightColor->setText(tr("Text highlight color"));

    actDecreaseIndent->setText(tr("Decrease indent"));
    actIncreaseIndent->setText(tr("Increase indent"));
    actAlignLeft->setText(tr("Align text left"));
    actAlignCenter->setText(tr("Center"));
    actAlignRight->setText(tr("Align text right"));
    actAlignJustify->setText(tr("Justify"));
    actParagraphDialog->setText(tr("Paragraph"));

    actInsertPicture->setText(tr("Picture"));
    actChangePicture->setText(tr("Change picture"));
    actResizePicture->setText(tr("Resize picture"));
    actPaintDrawing->setText(tr("Paint drawing"));
    actDateTime->setText(tr("Date and time"));
    actInsertObject->setText(tr("Insert object"));

    actFind->setText(tr("Find"));
    actReplace->setText(tr("Replace"));
    actSelectAll->setText(tr("Select all"));

    actZoomIn->setText(tr("Zoom in"));
    actZoomOut->setText(tr("Zoom out"));
    actZoom100->setText(tr("100%"));

    // Group labels
    if (m_lblGroupClipboard) m_lblGroupClipboard->setText(tr("Clipboard"));
    if (m_lblGroupFont) m_lblGroupFont->setText(tr("Font"));
    if (m_lblGroupPara) m_lblGroupPara->setText(tr("Paragraph"));
    if (m_lblGroupInsert) m_lblGroupInsert->setText(tr("Insert"));
    if (m_lblGroupEdit) m_lblGroupEdit->setText(tr("Editing"));
    if (m_lblGroupZoom) m_lblGroupZoom->setText(tr("Zoom"));
    if (m_lblGroupShowHide) m_lblGroupShowHide->setText(tr("Show or hide"));
    if (m_lblGroupSettings) m_lblGroupSettings->setText(tr("Settings"));

    // Buttons
    if (m_btnPaste) m_btnPaste->setText(tr("Paste"));
    if (m_btnCut) m_btnCut->setText(QStringLiteral(" ") + tr("Cut"));
    if (m_btnCopy) m_btnCopy->setText(QStringLiteral(" ") + tr("Copy"));
    if (m_btnPic) m_btnPic->setText(tr("Picture"));
    if (m_btnPaint) m_btnPaint->setText(tr("Paint drawing"));
    if (m_btnDate) m_btnDate->setText(tr("Date and time"));
    if (m_btnObj) m_btnObj->setText(tr("Insert object"));
    if (m_btnFind) m_btnFind->setText(QStringLiteral(" ") + tr("Find"));
    if (m_btnReplace) m_btnReplace->setText(QStringLiteral(" ") + tr("Replace"));
    if (m_btnSelectAll) m_btnSelectAll->setText(QStringLiteral(" ") + tr("Select all"));
    if (m_btnZoomIn) m_btnZoomIn->setText(tr("Zoom in"));
    if (m_btnZoomOut) m_btnZoomOut->setText(tr("Zoom out"));
    if (m_btnZoom100) m_btnZoom100->setText(tr("100%"));

    // Checkboxes
    if (chkRuler) chkRuler->setText(tr("Ruler"));
    if (chkStatusBar) chkStatusBar->setText(tr("Status bar"));

    // Settings
    if (m_lblWordWrap) m_lblWordWrap->setText(tr("Word wrap:"));
    if (m_lblUnits) m_lblUnits->setText(tr("Measurement units:"));
    if (m_lblLanguage) m_lblLanguage->setText(tr("Language:"));

    if (cbWordWrap) {
        int wrapIdx = cbWordWrap->currentIndex();
        cbWordWrap->blockSignals(true);
        cbWordWrap->clear();
        cbWordWrap->addItem(tr("No wrap"), static_cast<int>(WrapMode::NoWrap));
        cbWordWrap->addItem(tr("Wrap to window"), static_cast<int>(WrapMode::WrapToWindow));
        cbWordWrap->addItem(tr("Wrap to ruler"), static_cast<int>(WrapMode::WrapToRuler));
        cbWordWrap->setCurrentIndex(wrapIdx >= 0 ? wrapIdx : 2);
        cbWordWrap->blockSignals(false);
    }

    if (cbUnits) {
        int unitIdx = cbUnits->currentIndex();
        cbUnits->blockSignals(true);
        cbUnits->clear();
        cbUnits->addItem(tr("Inches"), static_cast<int>(UnitType::Inches));
        cbUnits->addItem(tr("Centimeters"), static_cast<int>(UnitType::Centimeters));
        cbUnits->addItem(tr("Points"), static_cast<int>(UnitType::Points));
        cbUnits->addItem(tr("Picas"), static_cast<int>(UnitType::Picas));
        cbUnits->setCurrentIndex(unitIdx >= 0 ? unitIdx : 1);
        cbUnits->blockSignals(false);
    }

    if (cbLanguage) {
        QString curLang = TranslationManager::instance().currentLanguage();
        for (int i = 0; i < cbLanguage->count(); ++i) {
            if (cbLanguage->itemData(i).toString() == curLang) {
                cbLanguage->blockSignals(true);
                cbLanguage->setCurrentIndex(i);
                cbLanguage->blockSignals(false);
                break;
            }
        }
    }
}

void RibbonBar::onFileMenuClicked() {
    if (m_fileMenu) {
        m_fileMenu->exec(m_btnFile->mapToGlobal(QPoint(0, m_btnFile->height())));
    }
}

void RibbonBar::onFontColorClicked() {
    QColor color = QColorDialog::getColor(Qt::black, this, tr("Text color"));
    if (color.isValid()) {
        emit fontColorSelected(color);
    }
}

void RibbonBar::onHighlightColorClicked() {
    QColor color = QColorDialog::getColor(Qt::yellow, this, tr("Text highlight color"));
    if (color.isValid()) {
        emit highlightColorSelected(color);
    }
}

void RibbonBar::updateFormattingState(const QTextCharFormat &charFmt, const QTextBlockFormat &blockFmt) {
    QFont font = charFmt.font();
    cbFontFamily->blockSignals(true);
    cbFontFamily->setCurrentText(font.family());
    cbFontFamily->blockSignals(false);

    cbFontSize->blockSignals(true);
    int ptSize = qRound(charFmt.fontPointSize());
    if (ptSize > 0) {
        cbFontSize->setCurrentText(QString::number(ptSize));
    }
    cbFontSize->blockSignals(false);

    actBold->blockSignals(true);
    actBold->setChecked(charFmt.fontWeight() >= QFont::Bold);
    actBold->blockSignals(false);

    actItalic->blockSignals(true);
    actItalic->setChecked(charFmt.fontItalic());
    actItalic->blockSignals(false);

    actUnderline->blockSignals(true);
    actUnderline->setChecked(charFmt.fontUnderline());
    actUnderline->blockSignals(false);

    actStrike->blockSignals(true);
    actStrike->setChecked(charFmt.fontStrikeOut());
    actStrike->blockSignals(false);

    actSubscript->blockSignals(true);
    actSubscript->setChecked(charFmt.verticalAlignment() == QTextCharFormat::AlignSubScript);
    actSubscript->blockSignals(false);

    actSuperscript->blockSignals(true);
    actSuperscript->setChecked(charFmt.verticalAlignment() == QTextCharFormat::AlignSuperScript);
    actSuperscript->blockSignals(false);

    Qt::Alignment align = blockFmt.alignment();
    actAlignLeft->blockSignals(true);
    actAlignCenter->blockSignals(true);
    actAlignRight->blockSignals(true);
    actAlignJustify->blockSignals(true);

    actAlignLeft->setChecked(align & Qt::AlignLeft);
    actAlignCenter->setChecked(align & Qt::AlignHCenter);
    actAlignRight->setChecked(align & Qt::AlignRight);
    actAlignJustify->setChecked(align & Qt::AlignJustify);

    actAlignLeft->blockSignals(false);
    actAlignCenter->blockSignals(false);
    actAlignRight->blockSignals(false);
    actAlignJustify->blockSignals(false);
}

void RibbonBar::updateRecentFilesMenu(const QStringList &files) {
    if (!m_recentMenu) return;
    m_recentMenu->clear();
    for (int i = 0; i < files.size(); ++i) {
        const QString &file = files.at(i);
        QFileInfo fi(file);
        QString label = QStringLiteral("&%1 %2").arg(i + 1).arg(fi.fileName());
        QAction *act = m_recentMenu->addAction(label);
        act->setData(file);
        connect(act, &QAction::triggered, this, [this, file]() {
            emit recentFileTriggered(file);
        });
    }
}

} // namespace OpenWordPad
