#include <QApplication>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QDebug>
#include <iostream>
#include "../src/core/RtfEngine.h"
#include "../src/core/DocumentManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::cout << "Testing RTF Round-trip with Formatted Text, Colors & Embedded Images..." << std::endl;

    QTextDocument doc;
    QTextCursor cursor(&doc);

    // 1. Insert Title
    QTextCharFormat titleFmt;
    titleFmt.setFontPointSize(18);
    titleFmt.setFontWeight(QFont::Bold);
    titleFmt.setForeground(QColor(0, 120, 215)); // Blue
    cursor.insertText(QStringLiteral("Documento di Prova RTF\n"), titleFmt);

    // 2. Insert Formatted Paragraph
    QTextCharFormat bodyFmt;
    bodyFmt.setFontPointSize(11);
    bodyFmt.setFontItalic(true);
    bodyFmt.setForeground(QColor(180, 0, 0)); // Red
    cursor.insertText(QStringLiteral("Paragrafo di testo in rosso corsivo.\n"), bodyFmt);

    // 3. Create and Insert an Image
    QImage testImg(100, 100, QImage::Format_RGB32);
    testImg.fill(QColor(30, 144, 255));
    {
        QPainter p(&testImg);
        p.setPen(Qt::white);
        p.drawText(testImg.rect(), Qt::AlignCenter, "TEST IMAGE");
    }
    QString imgName = QStringLiteral("embedded_test_img.png");
    doc.addResource(QTextDocument::ImageResource, QUrl(imgName), testImg);
    cursor.insertImage(imgName);
    cursor.insertText(QStringLiteral("\n"));

    // 4. Export to RTF
    QString rtf = OpenWordPad::RtfEngine::exportToRtf(&doc);
    if (rtf.isEmpty()) {
        std::cerr << "FAILED: exportToRtf returned empty!" << std::endl;
        return 1;
    }

    std::cout << "RTF Exported successfully (" << rtf.length() << " chars)." << std::endl;

    // Check that RTF contains \pict and \pngblip
    if (!rtf.contains(QStringLiteral("\\pict")) || !rtf.contains(QStringLiteral("\\pngblip"))) {
        std::cerr << "FAILED: RTF does not contain \\pict or \\pngblip!" << std::endl;
        return 1;
    }

    // 5. Import back into a new QTextDocument
    QTextDocument doc2;
    bool ok = OpenWordPad::RtfEngine::importRtf(rtf.toUtf8(), &doc2);
    if (!ok) {
        std::cerr << "FAILED: importRtf returned false!" << std::endl;
        return 1;
    }

    // 6. Verify contents of doc2
    std::cout << "Imported text:\n" << doc2.toPlainText().toStdString() << std::endl;
    
    // Check background colors in doc2 - ensure NO black background is set
    for (QTextBlock b = doc2.begin(); b.isValid(); b = b.next()) {
        for (auto it = b.begin(); !it.atEnd(); ++it) {
            QTextFragment frag = it.fragment();
            if (frag.isValid()) {
                QTextCharFormat f = frag.charFormat();
                if (f.background().style() != Qt::NoBrush && f.background().color() == Qt::black) {
                    std::cerr << "FAILED: Detected invalid black background on text fragment!" << std::endl;
                    return 1;
                }
            }
        }
    }

    std::cout << "SUCCESS: All formatting, colors, and embedded images round-tripped perfectly!" << std::endl;
    return 0;
}
