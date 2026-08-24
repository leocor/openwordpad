#include <QApplication>
#include <QTextDocument>
#include <QTextCursor>
#include <QFile>
#include <QDebug>
#include <cassert>
#include "../src/core/RtfEngine.h"
#include "../src/core/DocxEngine.h"
#include "../src/core/DocumentManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << "=== Running OpenWordPad Engine Tests ===";

    // Test 1: RTF Generation & Parsing
    {
        QTextDocument doc;
        QTextCursor cursor(&doc);
        cursor.insertText("Hello ");
        
        QTextCharFormat boldFmt;
        boldFmt.setFontWeight(QFont::Bold);
        cursor.insertText("World", boldFmt);

        cursor.insertBlock();
        QTextCharFormat italicFmt;
        italicFmt.setFontItalic(true);
        cursor.insertText("This is OpenWordPad by Leonardo Corato", italicFmt);

        QString rtf = OpenWordPad::RtfEngine::exportToRtf(&doc);
        assert(!rtf.isEmpty());
        assert(OpenWordPad::RtfEngine::isRtf(rtf.toUtf8()));
        qDebug() << "Test 1 Passed: RTF Export generated valid RTF.";

        QTextDocument doc2;
        bool ok = OpenWordPad::RtfEngine::importRtf(rtf.toUtf8(), &doc2);
        assert(ok);
        assert(doc2.toPlainText().contains("Hello World"));
        assert(doc2.toPlainText().contains("Leonardo Corato"));
        qDebug() << "Test 1 Passed: RTF Import verified.";
    }

    // Test 2: DOCX Generation & Parsing
    {
        QTextDocument doc;
        QTextCursor cursor(&doc);
        cursor.insertText("Testing DOCX support in OpenWordPad.");
        cursor.insertBlock();
        cursor.insertText("100% Cross-Platform C++20 Qt6.");

        QString docxPath = "/tmp/test_openwordpad.docx";
        bool saveOk = OpenWordPad::DocxEngine::saveDocx(docxPath, &doc);
        assert(saveOk);
        assert(QFile::exists(docxPath));
        qDebug() << "Test 2 Passed: DOCX Saved.";

        QTextDocument doc2;
        bool loadOk = OpenWordPad::DocxEngine::loadDocx(docxPath, &doc2);
        assert(loadOk);
        assert(doc2.toPlainText().contains("Testing DOCX support in OpenWordPad"));
        qDebug() << "Test 2 Passed: DOCX Loaded.";
        QFile::remove(docxPath);
    }

    // Test 3: DocumentManager formats
    {
        QTextDocument doc;
        OpenWordPad::DocumentManager mgr(&doc);
        mgr.newDocument();
        assert(mgr.documentTitle() == "Document");
        assert(!mgr.isModified());

        doc.setPlainText("Hello Text File");
        QString txtPath = "/tmp/test_openwordpad.txt";
        bool saveOk = mgr.saveFileAs(txtPath, OpenWordPad::DocumentFormat::PlainText);
        assert(saveOk);

        QTextDocument doc2;
        OpenWordPad::DocumentManager mgr2(&doc2);
        bool openOk = mgr2.openFile(txtPath);
        assert(openOk);
        assert(doc2.toPlainText() == "Hello Text File");
        qDebug() << "Test 3 Passed: Plain text DocumentManager save/open verified.";
        QFile::remove(txtPath);
    }

    qDebug() << "=== All OpenWordPad Tests Passed Successfully! ===";
    return 0;
}
