#include "RibbonBar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QPainter>
#include <QActionGroup>
#include <QFileInfo>
#include <QListView>

namespace OpenWordPad {

RibbonBar::RibbonBar(QWidget *parent)
    : QWidget(parent)
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
        "QToolButton::menu-arrow { image: none; width: 0px; height: 0px; border-left: 3px solid transparent; border-right: 3px solid transparent; border-top: 4px solid #333333; }"
        "QComboBox { border: 1px solid #c0c0c0; border-radius: 2px; padding: 1px 4px; background: #ffffff; color: #000000; font-size: 9pt; height: 22px; }"
        "QComboBox:hover { border: 1px solid #70c0e7; }"
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 16px; border-left: 1px solid #e0e0e0; background: #f6f6f6; }"
        "QComboBox::drop-down:hover { background: #e5f1fb; }"
        "QComboBox::down-arrow { image: none; width: 0px; height: 0px; border-left: 3px solid transparent; border-right: 3px solid transparent; border-top: 4px solid #333333; }"
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
    m_btnFile->setText(QStringLiteral("  File  "));
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

    m_tabWidget->addTab(m_homeTab, QStringLiteral("Home"));
    m_tabWidget->addTab(m_viewTab, QStringLiteral("Visualizza"));

    setupFileMenu();
    setupHomeTab();
    setupViewTab();

    mainLayout->addWidget(m_tabWidget);
}

void RibbonBar::setupQAT(QHBoxLayout *topBarLayout) {
    actSave = new QAction(QIcon(QStringLiteral(":/icons/save.svg")), QStringLiteral("Salva"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setToolTip(QStringLiteral("Salva (Ctrl+S)"));

    actUndo = new QAction(QIcon(QStringLiteral(":/icons/undo.svg")), QStringLiteral("Annulla"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setToolTip(QStringLiteral("Annulla (Ctrl+Z)"));

    actRedo = new QAction(QIcon(QStringLiteral(":/icons/redo.svg")), QStringLiteral("Ripristina"), this);
    actRedo->setShortcut(QKeySequence::Redo);
    actRedo->setToolTip(QStringLiteral("Ripristina (Ctrl+Y)"));

    actQuickPrint = new QAction(QIcon(QStringLiteral(":/icons/quick_print.svg")), QStringLiteral("Stampa rapida"), this);
    actQuickPrint->setToolTip(QStringLiteral("Stampa rapida"));

    actPrintPreview = new QAction(QIcon(QStringLiteral(":/icons/print_preview.svg")), QStringLiteral("Anteprima di stampa"), this);
    actPrintPreview->setToolTip(QStringLiteral("Anteprima di stampa"));

    topBarLayout->addWidget(createSmallButton(actSave, QStringLiteral(":/icons/save.svg"), QStringLiteral("Salva (Ctrl+S)")));
    topBarLayout->addWidget(createSmallButton(actUndo, QStringLiteral(":/icons/undo.svg"), QStringLiteral("Annulla (Ctrl+Z)")));
    topBarLayout->addWidget(createSmallButton(actRedo, QStringLiteral(":/icons/redo.svg"), QStringLiteral("Ripristina (Ctrl+Y)")));
}

void RibbonBar::setupFileMenu() {
    m_fileMenu = new QMenu(this);

    actNew = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/new.svg")), QStringLiteral("Nuovo"));
    actNew->setShortcut(QKeySequence::New);

    actOpen = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/open.svg")), QStringLiteral("Apri..."));
    actOpen->setShortcut(QKeySequence::Open);

    m_fileMenu->addAction(actSave);

    // Save As Submenu
    QMenu *saveAsMenu = m_fileMenu->addMenu(QIcon(QStringLiteral(":/icons/save_as.svg")), QStringLiteral("Salva con nome"));
    actSaveAsRtf = saveAsMenu->addAction(QStringLiteral("Documento RTF (*.rtf)"));
    actSaveAsDocx = saveAsMenu->addAction(QStringLiteral("Documento Office Open XML (*.docx)"));
    actSaveAsOdt = saveAsMenu->addAction(QStringLiteral("Documento OpenDocument (*.odt)"));
    actSaveAsTxt = saveAsMenu->addAction(QStringLiteral("Documento di testo normale (*.txt)"));
    actSaveAsOther = saveAsMenu->addAction(QStringLiteral("Altri formati (HTML, PDF)..."));

    // Print Submenu
    QMenu *printMenu = m_fileMenu->addMenu(QIcon(QStringLiteral(":/icons/print.svg")), QStringLiteral("Stampa"));
    actPrint = printMenu->addAction(QIcon(QStringLiteral(":/icons/print.svg")), QStringLiteral("Stampa..."));
    actPrint->setShortcut(QKeySequence::Print);
    printMenu->addAction(actQuickPrint);
    printMenu->addAction(actPrintPreview);

    actPageSetup = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/page_setup.svg")), QStringLiteral("Imposta pagina"));
    actEmail = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/email.svg")), QStringLiteral("Invia come messaggio di posta elettronica"));
    
    m_fileMenu->addSeparator();
    m_recentMenu = m_fileMenu->addMenu(QStringLiteral("Documenti recenti"));
    
    m_fileMenu->addSeparator();
    actAbout = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/about.svg")), QStringLiteral("Informazioni su WordPad"));
    actExit = m_fileMenu->addAction(QIcon(QStringLiteral(":/icons/exit.svg")), QStringLiteral("Esci"));

    m_btnFile->setMenu(m_fileMenu);
}

void RibbonBar::setupHomeTab() {
    auto homeLayout = new QHBoxLayout(m_homeTab);
    homeLayout->setContentsMargins(4, 3, 4, 3);
    homeLayout->setSpacing(4);

    // 1. Clipboard Group (Appunti)
    actPaste = new QAction(QIcon(QStringLiteral(":/icons/paste.svg")), QStringLiteral("Incolla"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    actPasteSpecial = new QAction(QIcon(QStringLiteral(":/icons/paste.svg")), QStringLiteral("Incolla speciale..."), this);
    actCut = new QAction(QIcon(QStringLiteral(":/icons/cut.svg")), QStringLiteral("Taglia"), this);
    actCut->setShortcut(QKeySequence::Cut);
    actCopy = new QAction(QIcon(QStringLiteral(":/icons/copy.svg")), QStringLiteral("Copia"), this);
    actCopy->setShortcut(QKeySequence::Copy);

    auto clipLayout = new QHBoxLayout();
    clipLayout->setContentsMargins(0, 0, 0, 0);
    clipLayout->setSpacing(3);
    
    auto btnPaste = createLargeButton(actPaste, QStringLiteral(":/icons/paste.svg"), QStringLiteral("Incolla"));
    btnPaste->setFixedSize(58, 66);
    QMenu *pasteMenu = new QMenu(this);
    pasteMenu->addAction(actPaste);
    pasteMenu->addAction(actPasteSpecial);
    btnPaste->setMenu(pasteMenu);
    btnPaste->setPopupMode(QToolButton::MenuButtonPopup);
    clipLayout->addWidget(btnPaste);

    auto clipSmallLayout = new QVBoxLayout();
    clipSmallLayout->setContentsMargins(0, 2, 0, 2);
    clipSmallLayout->setSpacing(3);

    auto btnCut = new QToolButton(this);
    btnCut->setDefaultAction(actCut);
    btnCut->setIcon(QIcon(QStringLiteral(":/icons/cut.svg")));
    btnCut->setText(QStringLiteral(" Taglia"));
    btnCut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnCut->setFixedHeight(22);
    btnCut->setStyleSheet("QToolButton { font-size: 8pt; padding: 2px; }");
    clipSmallLayout->addWidget(btnCut);

    auto btnCopy = new QToolButton(this);
    btnCopy->setDefaultAction(actCopy);
    btnCopy->setIcon(QIcon(QStringLiteral(":/icons/copy.svg")));
    btnCopy->setText(QStringLiteral(" Copia"));
    btnCopy->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnCopy->setFixedHeight(22);
    btnCopy->setStyleSheet("QToolButton { font-size: 8pt; padding: 2px; }");
    clipSmallLayout->addWidget(btnCopy);

    clipLayout->addLayout(clipSmallLayout);
    homeLayout->addWidget(createRibbonGroup(QStringLiteral("Appunti"), clipLayout));

    // 2. Font Group (Carattere)
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

    actGrowFont = new QAction(QIcon(QStringLiteral(":/icons/font_grow.svg")), QStringLiteral("Ingrandisci carattere"), this);
    actShrinkFont = new QAction(QIcon(QStringLiteral(":/icons/font_shrink.svg")), QStringLiteral("Riduci carattere"), this);

    fontRow1->addWidget(cbFontFamily);
    fontRow1->addWidget(cbFontSize);
    fontRow1->addWidget(createSmallButton(actGrowFont, QStringLiteral(":/icons/font_grow.svg"), QStringLiteral("Ingrandisci carattere (Ctrl+Shift+>)")));
    fontRow1->addWidget(createSmallButton(actShrinkFont, QStringLiteral(":/icons/font_shrink.svg"), QStringLiteral("Riduci carattere (Ctrl+Shift+<)")));
    fontRow1->addStretch();
    fontMainLayout->addLayout(fontRow1);

    // Row 2: Bold (G), Italic (C), Underline (S), Strikethrough (abc), Subscript, Superscript, Font Color, Highlight Color
    auto fontRow2 = new QHBoxLayout();
    fontRow2->setContentsMargins(0, 0, 0, 0);
    fontRow2->setSpacing(2);

    actBold = new QAction(QIcon(QStringLiteral(":/icons/bold.svg")), QStringLiteral("Grassetto"), this);
    actBold->setCheckable(true);
    actBold->setShortcut(QKeySequence::Bold);
    actItalic = new QAction(QIcon(QStringLiteral(":/icons/italic.svg")), QStringLiteral("Corsivo"), this);
    actItalic->setCheckable(true);
    actItalic->setShortcut(QKeySequence::Italic);
    actUnderline = new QAction(QIcon(QStringLiteral(":/icons/underline.svg")), QStringLiteral("Sottolineato"), this);
    actUnderline->setCheckable(true);
    actUnderline->setShortcut(QKeySequence::Underline);
    actStrike = new QAction(QIcon(QStringLiteral(":/icons/strikethrough.svg")), QStringLiteral("Barrato"), this);
    actStrike->setCheckable(true);
    actSubscript = new QAction(QIcon(QStringLiteral(":/icons/subscript.svg")), QStringLiteral("Pedice"), this);
    actSubscript->setCheckable(true);
    actSuperscript = new QAction(QIcon(QStringLiteral(":/icons/superscript.svg")), QStringLiteral("Apice"), this);
    actSuperscript->setCheckable(true);
    
    actFontColor = new QAction(QIcon(QStringLiteral(":/icons/font_color.svg")), QStringLiteral("Colore testo"), this);
    actHighlightColor = new QAction(QIcon(QStringLiteral(":/icons/highlight_color.svg")), QStringLiteral("Colore evidenziatore"), this);

    connect(actFontColor, &QAction::triggered, this, &RibbonBar::onFontColorClicked);
    connect(actHighlightColor, &QAction::triggered, this, &RibbonBar::onHighlightColorClicked);

    fontRow2->addWidget(createSmallButton(actBold, QStringLiteral(":/icons/bold.svg"), QStringLiteral("Grassetto (Ctrl+B)")));
    fontRow2->addWidget(createSmallButton(actItalic, QStringLiteral(":/icons/italic.svg"), QStringLiteral("Corsivo (Ctrl+I)")));
    fontRow2->addWidget(createSmallButton(actUnderline, QStringLiteral(":/icons/underline.svg"), QStringLiteral("Sottolineato (Ctrl+U)")));
    fontRow2->addWidget(createSmallButton(actStrike, QStringLiteral(":/icons/strikethrough.svg"), QStringLiteral("Barrato")));
    fontRow2->addWidget(createSmallButton(actSubscript, QStringLiteral(":/icons/subscript.svg"), QStringLiteral("Pedice")));
    fontRow2->addWidget(createSmallButton(actSuperscript, QStringLiteral(":/icons/superscript.svg"), QStringLiteral("Apice")));
    fontRow2->addSpacing(3);
    fontRow2->addWidget(createSmallButton(actFontColor, QStringLiteral(":/icons/font_color.svg"), QStringLiteral("Colore testo")));
    fontRow2->addWidget(createSmallButton(actHighlightColor, QStringLiteral(":/icons/highlight_color.svg"), QStringLiteral("Colore evidenziatore")));
    fontRow2->addStretch();
    fontMainLayout->addLayout(fontRow2);

    homeLayout->addWidget(createRibbonGroup(QStringLiteral("Carattere"), fontMainLayout));

    // 3. Paragraph Group (Paragrafo)
    auto paraMainLayout = new QVBoxLayout();
    paraMainLayout->setContentsMargins(0, 0, 0, 0);
    paraMainLayout->setSpacing(3);

    // Row 1: Indents, Lists, Line Spacing
    auto paraRow1 = new QHBoxLayout();
    paraRow1->setContentsMargins(0, 0, 0, 0);
    paraRow1->setSpacing(2);

    actDecreaseIndent = new QAction(QIcon(QStringLiteral(":/icons/indent_dec.svg")), QStringLiteral("Riduci rientro"), this);
    actIncreaseIndent = new QAction(QIcon(QStringLiteral(":/icons/indent_inc.svg")), QStringLiteral("Aumenta rientro"), this);

    // List Style Menu
    auto btnList = new QToolButton(this);
    btnList->setIcon(QIcon(QStringLiteral(":/icons/list_bullets.svg")));
    btnList->setPopupMode(QToolButton::InstantPopup);
    btnList->setToolTip(QStringLiteral("Inizia un elenco"));
    btnList->setFixedSize(22, 22);
    QMenu *listMenu = new QMenu(this);
    actListNone = listMenu->addAction(QStringLiteral("Nessuno"));
    actListBullet = listMenu->addAction(QStringLiteral("• Elenco puntato"));
    actListNumber = listMenu->addAction(QStringLiteral("1, 2, 3 Numerato"));
    actListAlphaLower = listMenu->addAction(QStringLiteral("a, b, c Lettere minuscole"));
    actListAlphaUpper = listMenu->addAction(QStringLiteral("A, B, C Lettere maiuscole"));
    actListRomanLower = listMenu->addAction(QStringLiteral("i, ii, iii Numeri romani minuscoli"));
    actListRomanUpper = listMenu->addAction(QStringLiteral("I, II, III Numeri romani maiuscoli"));
    btnList->setMenu(listMenu);

    // Line Spacing Menu
    auto btnLineSpacing = new QToolButton(this);
    btnLineSpacing->setIcon(QIcon(QStringLiteral(":/icons/line_spacing.svg")));
    btnLineSpacing->setPopupMode(QToolButton::InstantPopup);
    btnLineSpacing->setToolTip(QStringLiteral("Interlinea"));
    btnLineSpacing->setFixedSize(22, 22);
    QMenu *lsMenu = new QMenu(this);
    actLineSpacing10 = lsMenu->addAction(QStringLiteral("1.0"));
    actLineSpacing115 = lsMenu->addAction(QStringLiteral("1.15"));
    actLineSpacing15 = lsMenu->addAction(QStringLiteral("1.5"));
    actLineSpacing20 = lsMenu->addAction(QStringLiteral("2.0"));
    lsMenu->addSeparator();
    actLineSpacingAdd10pt = lsMenu->addAction(QStringLiteral("Aggiungi spazio di 10 pt dopo i paragrafi"));
    actLineSpacingAdd10pt->setCheckable(true);
    actLineSpacingAdd10pt->setChecked(true);
    btnLineSpacing->setMenu(lsMenu);

    actParagraphDialog = new QAction(QIcon(QStringLiteral(":/icons/paragraph.svg")), QStringLiteral("Paragrafo"), this);

    paraRow1->addWidget(createSmallButton(actDecreaseIndent, QStringLiteral(":/icons/indent_dec.svg"), QStringLiteral("Riduci rientro")));
    paraRow1->addWidget(createSmallButton(actIncreaseIndent, QStringLiteral(":/icons/indent_inc.svg"), QStringLiteral("Aumenta rientro")));
    paraRow1->addWidget(btnList);
    paraRow1->addWidget(btnLineSpacing);
    paraRow1->addStretch();
    paraMainLayout->addLayout(paraRow1);

    // Row 2: Alignments & Dialog launcher
    auto paraRow2 = new QHBoxLayout();
    paraRow2->setContentsMargins(0, 0, 0, 0);
    paraRow2->setSpacing(2);

    actAlignLeft = new QAction(QIcon(QStringLiteral(":/icons/align_left.svg")), QStringLiteral("Allinea a sinistra"), this);
    actAlignLeft->setCheckable(true);
    actAlignLeft->setChecked(true);
    actAlignLeft->setShortcut(QKeySequence(QStringLiteral("Ctrl+L")));

    actAlignCenter = new QAction(QIcon(QStringLiteral(":/icons/align_center.svg")), QStringLiteral("Allinea al centro"), this);
    actAlignCenter->setCheckable(true);
    actAlignCenter->setShortcut(QKeySequence(QStringLiteral("Ctrl+E")));

    actAlignRight = new QAction(QIcon(QStringLiteral(":/icons/align_right.svg")), QStringLiteral("Allinea a destra"), this);
    actAlignRight->setCheckable(true);
    actAlignRight->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));

    actAlignJustify = new QAction(QIcon(QStringLiteral(":/icons/align_justify.svg")), QStringLiteral("Giustifica"), this);
    actAlignJustify->setCheckable(true);
    actAlignJustify->setShortcut(QKeySequence(QStringLiteral("Ctrl+J")));

    auto alignGroup = new QActionGroup(this);
    alignGroup->addAction(actAlignLeft);
    alignGroup->addAction(actAlignCenter);
    alignGroup->addAction(actAlignRight);
    alignGroup->addAction(actAlignJustify);

    paraRow2->addWidget(createSmallButton(actAlignLeft, QStringLiteral(":/icons/align_left.svg"), QStringLiteral("Allinea a sinistra (Ctrl+L)")));
    paraRow2->addWidget(createSmallButton(actAlignCenter, QStringLiteral(":/icons/align_center.svg"), QStringLiteral("Allinea al centro (Ctrl+E)")));
    paraRow2->addWidget(createSmallButton(actAlignRight, QStringLiteral(":/icons/align_right.svg"), QStringLiteral("Allinea a destra (Ctrl+R)")));
    paraRow2->addWidget(createSmallButton(actAlignJustify, QStringLiteral(":/icons/align_justify.svg"), QStringLiteral("Giustifica (Ctrl+J)")));
    paraRow2->addWidget(createSmallButton(actParagraphDialog, QStringLiteral(":/icons/paragraph.svg"), QStringLiteral("Proprietà paragrafo")));
    paraRow2->addStretch();
    paraMainLayout->addLayout(paraRow2);

    homeLayout->addWidget(createRibbonGroup(QStringLiteral("Paragrafo"), paraMainLayout));

    // 4. Insert Group (Inserisci)
    auto insertLayout = new QHBoxLayout();
    insertLayout->setContentsMargins(0, 0, 0, 0);
    insertLayout->setSpacing(3);

    actInsertPicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), QStringLiteral("Immagine"), this);
    actChangePicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), QStringLiteral("Cambia immagine"), this);
    actResizePicture = new QAction(QIcon(QStringLiteral(":/icons/picture.svg")), QStringLiteral("Ridimensiona immagine"), this);

    auto btnPic = createLargeButton(actInsertPicture, QStringLiteral(":/icons/picture.svg"), QStringLiteral("Immagine"));
    btnPic->setFixedSize(68, 66);
    QMenu *picMenu = new QMenu(this);
    picMenu->addAction(actInsertPicture);
    picMenu->addAction(actChangePicture);
    picMenu->addAction(actResizePicture);
    btnPic->setMenu(picMenu);
    btnPic->setPopupMode(QToolButton::MenuButtonPopup);
    insertLayout->addWidget(btnPic);

    actPaintDrawing = new QAction(QIcon(QStringLiteral(":/icons/paint.svg")), QStringLiteral("Disegno\ndi Paint"), this);
    auto btnPaint = createLargeButton(actPaintDrawing, QStringLiteral(":/icons/paint.svg"), QStringLiteral("Disegno\ndi Paint"));
    btnPaint->setFixedSize(68, 66);
    insertLayout->addWidget(btnPaint);

    actDateTime = new QAction(QIcon(QStringLiteral(":/icons/datetime.svg")), QStringLiteral("Data\ne ora"), this);
    auto btnDate = createLargeButton(actDateTime, QStringLiteral(":/icons/datetime.svg"), QStringLiteral("Data\ne ora"));
    btnDate->setFixedSize(66, 66);
    insertLayout->addWidget(btnDate);

    actInsertObject = new QAction(QIcon(QStringLiteral(":/icons/object.svg")), QStringLiteral("Inserisci\noggetto"), this);
    auto btnObj = createLargeButton(actInsertObject, QStringLiteral(":/icons/object.svg"), QStringLiteral("Inserisci\noggetto"));
    btnObj->setFixedSize(68, 66);
    insertLayout->addWidget(btnObj);

    homeLayout->addWidget(createRibbonGroup(QStringLiteral("Inserisci"), insertLayout));

    // 5. Editing Group (Modifica)
    auto editLayout = new QVBoxLayout();
    editLayout->setContentsMargins(0, 2, 0, 2);
    editLayout->setSpacing(2);

    actFind = new QAction(QIcon(QStringLiteral(":/icons/find.svg")), QStringLiteral("Trova"), this);
    actFind->setShortcut(QKeySequence::Find);
    actReplace = new QAction(QIcon(QStringLiteral(":/icons/replace.svg")), QStringLiteral("Sostituisci"), this);
    actReplace->setShortcut(QKeySequence(QStringLiteral("Ctrl+H")));
    actSelectAll = new QAction(QIcon(QStringLiteral(":/icons/select_all.svg")), QStringLiteral("Seleziona tutto"), this);
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

    editLayout->addWidget(makeEditBtn(actFind, QStringLiteral(":/icons/find.svg"), QStringLiteral(" Trova")));
    editLayout->addWidget(makeEditBtn(actReplace, QStringLiteral(":/icons/replace.svg"), QStringLiteral(" Sostituisci")));
    editLayout->addWidget(makeEditBtn(actSelectAll, QStringLiteral(":/icons/select_all.svg"), QStringLiteral(" Seleziona tutto")));

    homeLayout->addWidget(createRibbonGroup(QStringLiteral("Modifica"), editLayout));
    homeLayout->addStretch();
}

void RibbonBar::setupViewTab() {
    auto viewLayout = new QHBoxLayout(m_viewTab);
    viewLayout->setContentsMargins(4, 2, 4, 2);
    viewLayout->setSpacing(4);

    // 1. Zoom Group
    auto zoomLayout = new QHBoxLayout();
    zoomLayout->setContentsMargins(0, 0, 0, 0);
    zoomLayout->setSpacing(2);
    actZoomIn = new QAction(QIcon(QStringLiteral(":/icons/zoom_in.svg")), QStringLiteral("Ingrandisci"), this);
    actZoomOut = new QAction(QIcon(QStringLiteral(":/icons/zoom_out.svg")), QStringLiteral("Riduci"), this);
    actZoom100 = new QAction(QIcon(QStringLiteral(":/icons/zoom_100.svg")), QStringLiteral("100%"), this);

    zoomLayout->addWidget(createLargeButton(actZoomIn, QStringLiteral(":/icons/zoom_in.svg"), QStringLiteral("Ingrandisci")));
    zoomLayout->addWidget(createLargeButton(actZoomOut, QStringLiteral(":/icons/zoom_out.svg"), QStringLiteral("Riduci")));
    zoomLayout->addWidget(createLargeButton(actZoom100, QStringLiteral(":/icons/zoom_100.svg"), QStringLiteral("100%")));
    viewLayout->addWidget(createRibbonGroup(QStringLiteral("Zoom"), zoomLayout));

    // 2. Show or Hide Group (Mostra o nascondi)
    auto showHideLayout = new QVBoxLayout();
    showHideLayout->setContentsMargins(0, 0, 0, 0);
    showHideLayout->setSpacing(2);
    chkRuler = new QCheckBox(QStringLiteral("Righello"), this);
    chkRuler->setChecked(true);
    chkStatusBar = new QCheckBox(QStringLiteral("Barra di stato"), this);
    chkStatusBar->setChecked(true);
    showHideLayout->addWidget(chkRuler);
    showHideLayout->addWidget(chkStatusBar);
    showHideLayout->addStretch();
    viewLayout->addWidget(createRibbonGroup(QStringLiteral("Mostra o nascondi"), showHideLayout));

    // 3. Settings Group (Impostazioni)
    auto settingsLayout = new QVBoxLayout();
    settingsLayout->setContentsMargins(0, 0, 0, 0);
    settingsLayout->setSpacing(2);

    auto wrapLayout = new QHBoxLayout();
    wrapLayout->setContentsMargins(0, 0, 0, 0);
    wrapLayout->addWidget(new QLabel(QStringLiteral("A capo automatico:"), this));
    cbWordWrap = new QComboBox(this);
    cbWordWrap->setView(new QListView(cbWordWrap));
    cbWordWrap->addItem(QStringLiteral("Nessun a capo"), static_cast<int>(WrapMode::NoWrap));
    cbWordWrap->addItem(QStringLiteral("Adatta alla finestra"), static_cast<int>(WrapMode::WrapToWindow));
    cbWordWrap->addItem(QStringLiteral("Adatta al righello"), static_cast<int>(WrapMode::WrapToRuler));
    cbWordWrap->setCurrentIndex(2); // Wrap to ruler default
    wrapLayout->addWidget(cbWordWrap);
    settingsLayout->addLayout(wrapLayout);

    auto unitsLayout = new QHBoxLayout();
    unitsLayout->setContentsMargins(0, 0, 0, 0);
    unitsLayout->addWidget(new QLabel(QStringLiteral("Unità di misura:"), this));
    cbUnits = new QComboBox(this);
    cbUnits->setView(new QListView(cbUnits));
    cbUnits->addItem(QStringLiteral("Pollici"), static_cast<int>(UnitType::Inches));
    cbUnits->addItem(QStringLiteral("Centimetri"), static_cast<int>(UnitType::Centimeters));
    cbUnits->addItem(QStringLiteral("Punti"), static_cast<int>(UnitType::Points));
    cbUnits->addItem(QStringLiteral("Pica"), static_cast<int>(UnitType::Picas));
    cbUnits->setCurrentIndex(1); // Centimeters default
    unitsLayout->addWidget(cbUnits);
    settingsLayout->addLayout(unitsLayout);

    viewLayout->addWidget(createRibbonGroup(QStringLiteral("Impostazioni"), settingsLayout));
    viewLayout->addStretch();
}

QWidget *RibbonBar::createRibbonGroup(const QString &title, QLayout *contentLayout) {
    auto groupWidget = new QWidget(this);
    auto vLayout = new QVBoxLayout(groupWidget);
    vLayout->setContentsMargins(4, 2, 4, 1);
    vLayout->setSpacing(1);

    auto contentWidget = new QWidget(groupWidget);
    contentWidget->setLayout(contentLayout);
    vLayout->addWidget(contentWidget, 1);

    auto lblTitle = new QLabel(title, groupWidget);
    lblTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lblTitle->setFixedHeight(14);
    lblTitle->setStyleSheet("color: #717171; font-size: 8pt; font-family: 'Segoe UI', 'DejaVu Sans', sans-serif; font-weight: normal; margin: 0; padding: 0; border: none;");
    vLayout->addWidget(lblTitle, 0);

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
    btn->setMinimumWidth(64);
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

void RibbonBar::updateFormattingState(const QTextCharFormat &charFmt, const QTextBlockFormat &blockFmt) {
    actBold->blockSignals(true);
    actBold->setChecked(charFmt.fontWeight() > QFont::Normal);
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

    cbFontFamily->blockSignals(true);
    QStringList fams = charFmt.fontFamilies().toStringList();
    if (!fams.isEmpty()) {
        cbFontFamily->setCurrentText(fams.first());
    }
    cbFontFamily->blockSignals(false);

    cbFontSize->blockSignals(true);
    if (charFmt.fontPointSize() > 0) {
        cbFontSize->setCurrentText(QString::number(static_cast<int>(charFmt.fontPointSize() + 0.5)));
    }
    cbFontSize->blockSignals(false);

    // Alignment state
    Qt::Alignment align = blockFmt.alignment();
    actAlignLeft->blockSignals(true);
    actAlignCenter->blockSignals(true);
    actAlignRight->blockSignals(true);
    actAlignJustify->blockSignals(true);

    if (align & Qt::AlignRight) actAlignRight->setChecked(true);
    else if (align & Qt::AlignHCenter) actAlignCenter->setChecked(true);
    else if (align & Qt::AlignJustify) actAlignJustify->setChecked(true);
    else actAlignLeft->setChecked(true);

    actAlignLeft->blockSignals(false);
    actAlignCenter->blockSignals(false);
    actAlignRight->blockSignals(false);
    actAlignJustify->blockSignals(false);
}

void RibbonBar::updateRecentFilesMenu(const QStringList &files) {
    m_recentMenu->clear();
    if (files.isEmpty()) {
        QAction *emptyAct = m_recentMenu->addAction(QStringLiteral("(Nessun documento recente)"));
        emptyAct->setEnabled(false);
        return;
    }

    for (int i = 0; i < files.size(); ++i) {
        QString f = files[i];
        QFileInfo fi(f);
        QString label = QStringLiteral("&%1 %2").arg(i + 1).arg(fi.fileName());
        QAction *act = m_recentMenu->addAction(label);
        act->setToolTip(f);
        connect(act, &QAction::triggered, this, [this, f]() {
            emit recentFileTriggered(f);
        });
    }
}

void RibbonBar::onFileMenuClicked() {
    m_fileMenu->exec(m_btnFile->mapToGlobal(QPoint(0, m_btnFile->height())));
}

void RibbonBar::onFontColorClicked() {
    QColor c = QColorDialog::getColor(Qt::black, this, QStringLiteral("Select Text Color"));
    if (c.isValid()) {
        emit fontColorSelected(c);
    }
}

void RibbonBar::onHighlightColorClicked() {
    QColor c = QColorDialog::getColor(Qt::yellow, this, QStringLiteral("Select Highlight Color"));
    if (c.isValid()) {
        emit highlightColorSelected(c);
    }
}

} // namespace OpenWordPad
