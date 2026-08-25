#include <QApplication>
#include <QPainter>
#include <QDir>
#include <QTabWidget>
#include <QTextCursor>
#include <QTextTable>
#include <QScrollBar>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextListFormat>
#include "MainWindow.h"
#include "DocumentEditor.h"
#include "RibbonBar.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/PageSetupDialog.h"
#include "dialogs/ParagraphDialog.h"
#include "TranslationManager.h"

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    app.setApplicationName("OpenWordPad");

    QDir().mkpath("docs/screenshots");

    // Initialize Translations
    OpenWordPad::TranslationManager::instance().setLanguage("en");

    // 1. Capture Main Editor Window
    OpenWordPad::MainWindow window;
    window.resize(1180, 800);
    window.show();

    // Find DocumentEditor inside window
    auto *editor = window.findChild<OpenWordPad::DocumentEditor*>();
    if (editor) {
        editor->document()->clear();
        QTextCursor cursor(editor->document());

        // Document Title
        QTextBlockFormat titleBlock;
        titleBlock.setAlignment(Qt::AlignCenter);
        titleBlock.setBottomMargin(8);
        cursor.setBlockFormat(titleBlock);

        QTextCharFormat titleFormat;
        titleFormat.setFontPointSize(20);
        titleFormat.setFontWeight(QFont::Bold);
        titleFormat.setForeground(QColor("#0066cc"));
        cursor.insertText("Welcome to OpenWordPad\n", titleFormat);

        // Subtitle
        QTextBlockFormat subBlock;
        subBlock.setAlignment(Qt::AlignCenter);
        subBlock.setBottomMargin(16);
        cursor.setBlockFormat(subBlock);

        QTextCharFormat subFormat;
        subFormat.setFontPointSize(11);
        subFormat.setFontItalic(true);
        subFormat.setForeground(QColor("#666666"));
        cursor.insertText("The Modern, Open-Source Recreation of Classic WordPad\n\n", subFormat);

        // Body Paragraph
        QTextBlockFormat bodyBlock;
        bodyBlock.setAlignment(Qt::AlignLeft);
        bodyBlock.setLineHeight(130, QTextBlockFormat::ProportionalHeight);
        bodyBlock.setBottomMargin(10);
        cursor.setBlockFormat(bodyBlock);

        QTextCharFormat normalFormat;
        normalFormat.setFontPointSize(11);
        normalFormat.setForeground(QColor("#222222"));
        cursor.insertText("OpenWordPad is a fast, native, multiplatform word processor written in ", normalFormat);

        QTextCharFormat boldFormat = normalFormat;
        boldFormat.setFontWeight(QFont::Bold);
        boldFormat.setForeground(QColor("#0078d4"));
        cursor.insertText("C++20 & Qt 6", boldFormat);

        cursor.insertText(". It delivers rich formatting, seamless file interoperability with Microsoft Word (.docx) and Rich Text Format (.rtf), and full support for over 20 languages.\n\n", normalFormat);

        // Feature Highlights Heading
        QTextBlockFormat h2Block;
        h2Block.setBottomMargin(8);
        cursor.setBlockFormat(h2Block);

        QTextCharFormat h2Format;
        h2Format.setFontPointSize(12);
        h2Format.setFontWeight(QFont::Bold);
        h2Format.setForeground(QColor("#107c41"));
        cursor.insertText("Key Highlights & Features:\n", h2Format);

        // Bullet list
        QTextListFormat listFormat;
        listFormat.setStyle(QTextListFormat::ListDisc);
        listFormat.setIndent(1);
        cursor.insertList(listFormat);

        auto insertBullet = [&](const QString &boldTxt, const QString &restTxt) {
            cursor.insertText(boldTxt, boldFormat);
            cursor.insertText(restTxt + "\n", normalFormat);
        };

        insertBullet("Full Interoperability: ", "Open and save .docx, .rtf, .odt, and plain text .txt files.");
        insertBullet("Classic Ribbon Interface: ", "Intuitive formatting, font effects, colors, paragraph indentation, and alignment.");
        insertBullet("Multiplatform Release: ", "Native binaries for Windows (Setup.exe & MSI), Linux (AppImage), and macOS (Apple Silicon DMG).");
        insertBullet("Multilingual Support: ", "Complete localization in 20+ languages with automatic session detection.");

        // Scroll back to top
        QTextCursor topCursor = editor->textCursor();
        topCursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(topCursor);
        if (editor->verticalScrollBar()) {
            editor->verticalScrollBar()->setValue(0);
        }
    }

    // Process events to ensure layout and rendering are fully updated
    app.processEvents();
    app.processEvents();

    // Render Main Window
    QPixmap pixmap(window.size());
    pixmap.fill(Qt::white);
    window.render(&pixmap);
    pixmap.save("docs/screenshots/screenshot_main.png");

    // 2. Switch to View Tab & Render
    auto *ribbon = window.findChild<OpenWordPad::RibbonBar*>();
    if (ribbon) {
        auto *tabWidget = ribbon->findChild<QTabWidget*>();
        if (tabWidget && tabWidget->count() > 1) {
            tabWidget->setCurrentIndex(1); // Switch to View tab
            app.processEvents();
            app.processEvents();
            QPixmap viewPixmap(window.size());
            viewPixmap.fill(Qt::white);
            window.render(&viewPixmap);
            viewPixmap.save("docs/screenshots/screenshot_view.png");
            tabWidget->setCurrentIndex(0); // Return to Home
        }
    }

    // 3. Render About Dialog
    OpenWordPad::AboutDialog aboutDialog(&window);
    aboutDialog.resize(460, 420);
    aboutDialog.show();
    app.processEvents();
    QPixmap aboutPixmap(aboutDialog.size());
    aboutPixmap.fill(Qt::white);
    aboutDialog.render(&aboutPixmap);
    aboutPixmap.save("docs/screenshots/screenshot_about.png");

    // 4. Render Page Setup Dialog
    OpenWordPad::PageSettings pageSettings;
    OpenWordPad::PageSetupDialog pageSetupDialog(pageSettings, OpenWordPad::UnitType::Centimeters, &window);
    pageSetupDialog.resize(480, 360);
    pageSetupDialog.show();
    app.processEvents();
    QPixmap pagePixmap(pageSetupDialog.size());
    pagePixmap.fill(Qt::white);
    pageSetupDialog.render(&pagePixmap);
    pagePixmap.save("docs/screenshots/screenshot_page_setup.png");

    // 5. Render Paragraph Dialog
    OpenWordPad::ParagraphDialog paraDialog(0.0, 0.0, 0.0, 1.15, true, Qt::AlignLeft, OpenWordPad::UnitType::Centimeters, &window);
    paraDialog.resize(460, 380);
    paraDialog.show();
    app.processEvents();
    QPixmap paraPixmap(paraDialog.size());
    paraPixmap.fill(Qt::white);
    paraDialog.render(&paraPixmap);
    paraPixmap.save("docs/screenshots/screenshot_paragraph.png");

    return 0;
}
