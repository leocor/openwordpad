#include "DocxEngine.h"
#include "miniz.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextCursor>
#include <QTextBlock>
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <QDebug>

namespace OpenWordPad {

bool DocxEngine::isDocx(const QString &fileName) {
    return fileName.endsWith(QStringLiteral(".docx"), Qt::CaseInsensitive);
}

bool DocxEngine::loadDocx(const QString &fileName, QTextDocument *document) {
    if (!document || fileName.isEmpty()) return false;

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (!mz_zip_reader_init_file(&zip, fileName.toUtf8().constData(), 0)) {
        return false;
    }

    int fileIdx = mz_zip_reader_locate_file(&zip, "word/document.xml", nullptr, 0);
    if (fileIdx < 0) {
        mz_zip_reader_end(&zip);
        return false;
    }

    size_t uncompSize = 0;
    void *pData = mz_zip_reader_extract_to_heap(&zip, fileIdx, &uncompSize, 0);
    mz_zip_reader_end(&zip);

    if (!pData) return false;

    QByteArray xmlData(static_cast<const char *>(pData), static_cast<int>(uncompSize));
    mz_free(pData);

    document->clear();
    QTextCursor cursor(document);

    QXmlStreamReader xml(xmlData);

    struct RunFormat {
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strike = false;
        QString fontFamily = QStringLiteral("Calibri");
        double fontSize = 11.0;
        QColor color = Qt::black;
        QColor highlight = Qt::transparent;
        QTextCharFormat::VerticalAlignment vertAlign = QTextCharFormat::AlignNormal;
    };

    RunFormat currentRunFmt;
    Qt::Alignment currentAlign = Qt::AlignLeft;
    double leftMargin = 0.0;
    double rightMargin = 0.0;
    double textIndent = 0.0;
    double spaceAfter = 0.0;
    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();

            if (name == QStringLiteral("p")) {
                currentAlign = Qt::AlignLeft;
                leftMargin = 0.0;
                rightMargin = 0.0;
                textIndent = 0.0;
                spaceAfter = 0.0;
                currentRunFmt = RunFormat();
            } else if (name == QStringLiteral("jc")) {
                QString val = xml.attributes().value(QStringLiteral("w:val")).toString();
                if (val == QStringLiteral("center")) currentAlign = Qt::AlignHCenter;
                else if (val == QStringLiteral("right")) currentAlign = Qt::AlignRight;
                else if (val == QStringLiteral("both")) currentAlign = Qt::AlignJustify;
                else currentAlign = Qt::AlignLeft;
            } else if (name == QStringLiteral("ind")) {
                if (xml.attributes().hasAttribute(QStringLiteral("w:left"))) {
                    leftMargin = xml.attributes().value(QStringLiteral("w:left")).toInt() * (96.0 / 1440.0);
                }
                if (xml.attributes().hasAttribute(QStringLiteral("w:right"))) {
                    rightMargin = xml.attributes().value(QStringLiteral("w:right")).toInt() * (96.0 / 1440.0);
                }
                if (xml.attributes().hasAttribute(QStringLiteral("w:firstLine"))) {
                    textIndent = xml.attributes().value(QStringLiteral("w:firstLine")).toInt() * (96.0 / 1440.0);
                }
            } else if (name == QStringLiteral("spacing")) {
                if (xml.attributes().hasAttribute(QStringLiteral("w:after"))) {
                    spaceAfter = xml.attributes().value(QStringLiteral("w:after")).toInt() * (96.0 / 1440.0);
                }
            } else if (name == QStringLiteral("rPr")) {
                currentRunFmt = RunFormat();
            } else if (name == QStringLiteral("b")) {
                currentRunFmt.bold = true;
            } else if (name == QStringLiteral("i")) {
                currentRunFmt.italic = true;
            } else if (name == QStringLiteral("u")) {
                currentRunFmt.underline = true;
            } else if (name == QStringLiteral("strike")) {
                currentRunFmt.strike = true;
            } else if (name == QStringLiteral("rFonts")) {
                if (xml.attributes().hasAttribute(QStringLiteral("w:ascii"))) {
                    currentRunFmt.fontFamily = xml.attributes().value(QStringLiteral("w:ascii")).toString();
                }
            } else if (name == QStringLiteral("sz")) {
                currentRunFmt.fontSize = xml.attributes().value(QStringLiteral("w:val")).toDouble() / 2.0;
            } else if (name == QStringLiteral("color")) {
                QString hex = xml.attributes().value(QStringLiteral("w:val")).toString();
                if (!hex.isEmpty() && hex.toLower() != QStringLiteral("auto")) {
                    currentRunFmt.color = QColor(QStringLiteral("#") + hex);
                }
            } else if (name == QStringLiteral("vertAlign")) {
                QString val = xml.attributes().value(QStringLiteral("w:val")).toString();
                if (val == QStringLiteral("subscript")) currentRunFmt.vertAlign = QTextCharFormat::AlignSubScript;
                else if (val == QStringLiteral("superscript")) currentRunFmt.vertAlign = QTextCharFormat::AlignSuperScript;
            } else if (name == QStringLiteral("t")) {
                QTextCharFormat cf;
                cf.setFontFamilies({currentRunFmt.fontFamily});
                cf.setFontPointSize(currentRunFmt.fontSize);
                cf.setFontWeight(currentRunFmt.bold ? QFont::Bold : QFont::Normal);
                cf.setFontItalic(currentRunFmt.italic);
                cf.setFontUnderline(currentRunFmt.underline);
                cf.setFontStrikeOut(currentRunFmt.strike);
                cf.setVerticalAlignment(currentRunFmt.vertAlign);
                cf.setForeground(currentRunFmt.color);

                QString text = xml.readElementText();
                cursor.insertText(text, cf);
            }
        } else if (xml.isEndElement()) {
            if (xml.name() == QStringLiteral("p")) {
                QTextBlockFormat bf;
                bf.setAlignment(currentAlign);
                if (leftMargin > 0) bf.setLeftMargin(leftMargin);
                if (rightMargin > 0) bf.setRightMargin(rightMargin);
                if (textIndent != 0) bf.setTextIndent(textIndent);
                if (spaceAfter > 0) bf.setBottomMargin(spaceAfter);
                cursor.insertBlock(bf);
            }
        }
    }

    mz_zip_archive zipMedia;
    memset(&zipMedia, 0, sizeof(zipMedia));
    if (mz_zip_reader_init_file(&zipMedia, fileName.toUtf8().constData(), 0)) {
        int numFiles = static_cast<int>(mz_zip_reader_get_num_files(&zipMedia));
        for (int i = 0; i < numFiles; ++i) {
            mz_zip_archive_file_stat stat;
            if (mz_zip_reader_file_stat(&zipMedia, i, &stat)) {
                QString entryName = QString::fromUtf8(stat.m_filename);
                if (entryName.startsWith(QStringLiteral("word/media/"))) {
                    size_t imgSize = 0;
                    void *imgPtr = mz_zip_reader_extract_to_heap(&zipMedia, i, &imgSize, 0);
                    if (imgPtr) {
                        QByteArray ba(static_cast<const char *>(imgPtr), static_cast<int>(imgSize));
                        mz_free(imgPtr);
                        QImage img = QImage::fromData(ba);
                        if (!img.isNull()) {
                            QString imgName = QFileInfo(entryName).fileName();
                            document->addResource(QTextDocument::ImageResource, QUrl(imgName), img);
                            cursor.insertImage(imgName);
                        }
                    }
                }
            }
        }
        mz_zip_reader_end(&zipMedia);
    }

    return true;
}

bool DocxEngine::saveDocx(const QString &fileName, const QTextDocument *document) {
    if (!document || fileName.isEmpty()) return false;

    struct SavedImage {
        QString rId;
        QString fileName;
        QByteArray data;
    };
    QList<SavedImage> savedImages;
    int imgCounter = 1;

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid()) continue;
            QTextCharFormat cf = fragment.charFormat();
            if (cf.isImageFormat()) {
                QTextImageFormat imgFmt = cf.toImageFormat();
                QString imgName = imgFmt.name();
                QImage img;
                QVariant res = document->resource(QTextDocument::ImageResource, QUrl(imgName));
                if (res.isValid() && res.canConvert<QImage>()) img = res.value<QImage>();
                if (img.isNull()) img.load(imgName);
                if (!img.isNull()) {
                    QByteArray ba;
                    QBuffer buf(&ba);
                    buf.open(QIODevice::WriteOnly);
                    img.save(&buf, "PNG");
                    buf.close();

                    SavedImage si;
                    si.rId = QStringLiteral("rIdImg%1").arg(imgCounter);
                    si.fileName = QStringLiteral("image%1.png").arg(imgCounter);
                    si.data = ba;
                    savedImages.append(si);
                    imgCounter++;
                }
            }
        }
    }

    QString docXml;
    QXmlStreamWriter xml(&docXml);
    xml.setAutoFormatting(true);
    xml.writeStartDocument(QStringLiteral("1.0"), true);
    xml.writeStartElement(QStringLiteral("w:document"));
    xml.writeAttribute(QStringLiteral("xmlns:w"), QStringLiteral("http://schemas.openxmlformats.org/wordprocessingml/2006/main"));
    xml.writeAttribute(QStringLiteral("xmlns:r"), QStringLiteral("http://schemas.openxmlformats.org/officeDocument/2006/relationships"));
    xml.writeStartElement(QStringLiteral("w:body"));

    int currentImgIdx = 0;
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        xml.writeStartElement(QStringLiteral("w:p"));

        xml.writeStartElement(QStringLiteral("w:pPr"));
        QTextBlockFormat bf = block.blockFormat();
        QString alignVal = QStringLiteral("left");
        if (bf.alignment() & Qt::AlignRight) alignVal = QStringLiteral("right");
        else if (bf.alignment() & Qt::AlignHCenter) alignVal = QStringLiteral("center");
        else if (bf.alignment() & Qt::AlignJustify) alignVal = QStringLiteral("both");

        xml.writeStartElement(QStringLiteral("w:jc"));
        xml.writeAttribute(QStringLiteral("w:val"), alignVal);
        xml.writeEndElement();

        if (bf.leftMargin() > 0 || bf.rightMargin() > 0 || bf.textIndent() != 0) {
            xml.writeStartElement(QStringLiteral("w:ind"));
            if (bf.leftMargin() > 0) xml.writeAttribute(QStringLiteral("w:left"), QString::number(static_cast<int>(bf.leftMargin() * (1440.0 / 96.0))));
            if (bf.rightMargin() > 0) xml.writeAttribute(QStringLiteral("w:right"), QString::number(static_cast<int>(bf.rightMargin() * (1440.0 / 96.0))));
            if (bf.textIndent() != 0) xml.writeAttribute(QStringLiteral("w:firstLine"), QString::number(static_cast<int>(bf.textIndent() * (1440.0 / 96.0))));
            xml.writeEndElement();
        }
        if (bf.bottomMargin() > 0) {
            xml.writeStartElement(QStringLiteral("w:spacing"));
            xml.writeAttribute(QStringLiteral("w:after"), QString::number(static_cast<int>(bf.bottomMargin() * (1440.0 / 96.0))));
            xml.writeEndElement();
        }
        xml.writeEndElement();

        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid()) continue;

            QTextCharFormat cf = fragment.charFormat();
            if (cf.isImageFormat() && currentImgIdx < savedImages.size()) {
                const auto &si = savedImages[currentImgIdx++];
                xml.writeStartElement(QStringLiteral("w:r"));
                xml.writeStartElement(QStringLiteral("w:drawing"));
                xml.writeStartElement(QStringLiteral("wp:inline"));
                xml.writeAttribute(QStringLiteral("xmlns:wp"), QStringLiteral("http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing"));
                xml.writeStartElement(QStringLiteral("a:graphic"));
                xml.writeAttribute(QStringLiteral("xmlns:a"), QStringLiteral("http://schemas.openxmlformats.org/drawingml/2006/main"));
                xml.writeStartElement(QStringLiteral("a:graphicData"));
                xml.writeAttribute(QStringLiteral("uri"), QStringLiteral("http://schemas.openxmlformats.org/drawingml/2006/picture"));
                xml.writeStartElement(QStringLiteral("pic:pic"));
                xml.writeAttribute(QStringLiteral("xmlns:pic"), QStringLiteral("http://schemas.openxmlformats.org/drawingml/2006/picture"));
                xml.writeStartElement(QStringLiteral("pic:blipFill"));
                xml.writeEmptyElement(QStringLiteral("a:blip"));
                xml.writeAttribute(QStringLiteral("r:embed"), si.rId);
                xml.writeEndElement();
                xml.writeEndElement();
                xml.writeEndElement();
                xml.writeEndElement();
                xml.writeEndElement();
                xml.writeEndElement();
                xml.writeEndElement();
                continue;
            }

            xml.writeStartElement(QStringLiteral("w:r"));
            xml.writeStartElement(QStringLiteral("w:rPr"));
            QStringList fams = cf.fontFamilies().toStringList();
            QString family = fams.isEmpty() ? QStringLiteral("Calibri") : fams.first();
            xml.writeStartElement(QStringLiteral("w:rFonts"));
            xml.writeAttribute(QStringLiteral("w:ascii"), family);
            xml.writeAttribute(QStringLiteral("w:hAnsi"), family);
            xml.writeEndElement();
            if (cf.fontWeight() >= QFont::Bold) xml.writeEmptyElement(QStringLiteral("w:b"));
            if (cf.fontItalic()) xml.writeEmptyElement(QStringLiteral("w:i"));
            if (cf.fontUnderline()) { xml.writeStartElement(QStringLiteral("w:u")); xml.writeAttribute(QStringLiteral("w:val"), QStringLiteral("single")); xml.writeEndElement(); }
            if (cf.fontStrikeOut()) xml.writeEmptyElement(QStringLiteral("w:strike"));
            if (cf.verticalAlignment() == QTextCharFormat::AlignSubScript) { xml.writeStartElement(QStringLiteral("w:vertAlign")); xml.writeAttribute(QStringLiteral("w:val"), QStringLiteral("subscript")); xml.writeEndElement(); }
            else if (cf.verticalAlignment() == QTextCharFormat::AlignSuperScript) { xml.writeStartElement(QStringLiteral("w:vertAlign")); xml.writeAttribute(QStringLiteral("w:val"), QStringLiteral("superscript")); xml.writeEndElement(); }
            if (cf.fontPointSize() > 0) { xml.writeStartElement(QStringLiteral("w:sz")); xml.writeAttribute(QStringLiteral("w:val"), QString::number(static_cast<int>(cf.fontPointSize() * 2))); xml.writeEndElement(); }
            if (cf.foreground().color().isValid() && cf.foreground().color() != Qt::black) { xml.writeStartElement(QStringLiteral("w:color")); xml.writeAttribute(QStringLiteral("w:val"), cf.foreground().color().name().mid(1).toUpper()); xml.writeEndElement(); }
            xml.writeEndElement();
            xml.writeStartElement(QStringLiteral("w:t"));
            xml.writeAttribute(QStringLiteral("xml:space"), QStringLiteral("preserve"));
            xml.writeCharacters(fragment.text());
            xml.writeEndElement();
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();

    QByteArray contentTypes =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
        "  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
        "  <Default Extension=\"xml\" ContentType=\"application/xml\"/>\n"
        "  <Default Extension=\"png\" ContentType=\"image/png\"/>\n"
        "  <Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>\n"
        "</Types>";

    QByteArray dotRels =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
        "  <Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>\n"
        "</Relationships>";

    QString docRelsXml = QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n");
    for (const auto &si : savedImages) {
        docRelsXml += QStringLiteral("  <Relationship Id=\"%1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/image\" Target=\"media/%2\"/>\n").arg(si.rId).arg(si.fileName);
    }
    docRelsXml += QStringLiteral("</Relationships>");

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_writer_init_file(&zip, fileName.toUtf8().constData(), 0)) return false;

    mz_zip_writer_add_mem(&zip, "[Content_Types].xml", contentTypes.constData(), contentTypes.size(), MZ_BEST_COMPRESSION);
    mz_zip_writer_add_mem(&zip, "_rels/.rels", dotRels.constData(), dotRels.size(), MZ_BEST_COMPRESSION);
    QByteArray docRelsBytes = docRelsXml.toUtf8();
    mz_zip_writer_add_mem(&zip, "word/_rels/document.xml.rels", docRelsBytes.constData(), docRelsBytes.size(), MZ_BEST_COMPRESSION);
    for (const auto &si : savedImages) {
        QString mediaPath = QStringLiteral("word/media/") + si.fileName;
        mz_zip_writer_add_mem(&zip, mediaPath.toUtf8().constData(), si.data.constData(), si.data.size(), MZ_BEST_COMPRESSION);
    }
    QByteArray docXmlBytes = docXml.toUtf8();
    mz_zip_writer_add_mem(&zip, "word/document.xml", docXmlBytes.constData(), docXmlBytes.size(), MZ_BEST_COMPRESSION);
    mz_zip_writer_finalize_archive(&zip);
    mz_zip_writer_end(&zip);

    return true;
}

} // namespace OpenWordPad
