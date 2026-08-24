#include "RtfEngine.h"
#include "Units.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QTextList>
#include <QTextTableCell>
#include <QTextTable>
#include <QTextImageFormat>
#include <QBuffer>
#include <QImage>
#include <QRegularExpression>
#include <QStack>
#include <QDebug>

namespace OpenWordPad {

bool RtfEngine::isRtf(const QByteArray &data) {
    QByteArray trimmed = data.trimmed();
    return trimmed.startsWith("{\\rtf");
}

QString RtfEngine::exportToRtf(const QTextDocument *document) {
    if (!document) return QString();

    QStringList fontTable;
    QList<QColor> colorTable;
    colorTable.append(QColor()); // 0 is auto/default

    auto getFontIndex = [&](const QString &family) -> int {
        QString f = family.isEmpty() ? QStringLiteral("Calibri") : family;
        int idx = fontTable.indexOf(f);
        if (idx == -1) {
            fontTable.append(f);
            idx = fontTable.size() - 1;
        }
        return idx;
    };

    auto getColorIndex = [&](const QColor &color) -> int {
        if (!color.isValid() || color == Qt::transparent) return 0;
        for (int i = 1; i < colorTable.size(); ++i) {
            if (colorTable[i] == color) return i;
        }
        colorTable.append(color);
        return colorTable.size() - 1;
    };

    // First pass or building body stream
    QString body;

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat blockFmt = block.blockFormat();
        
        body += QStringLiteral("{\\pard");

        // Alignment
        Qt::Alignment align = blockFmt.alignment();
        if (align & Qt::AlignRight) body += QStringLiteral("\\qr");
        else if (align & Qt::AlignHCenter) body += QStringLiteral("\\qc");
        else if (align & Qt::AlignJustify) body += QStringLiteral("\\qj");
        else body += QStringLiteral("\\ql");

        // Indents (convert px to twips)
        if (blockFmt.leftMargin() > 0) {
            int twips = static_cast<int>(blockFmt.leftMargin() * (1440.0 / 96.0));
            body += QStringLiteral("\\li%1").arg(twips);
        }
        if (blockFmt.rightMargin() > 0) {
            int twips = static_cast<int>(blockFmt.rightMargin() * (1440.0 / 96.0));
            body += QStringLiteral("\\ri%1").arg(twips);
        }
        if (blockFmt.textIndent() != 0) {
            int twips = static_cast<int>(blockFmt.textIndent() * (1440.0 / 96.0));
            body += QStringLiteral("\\fi%1").arg(twips);
        }
        if (blockFmt.bottomMargin() > 0) {
            int twips = static_cast<int>(blockFmt.bottomMargin() * (1440.0 / 96.0));
            body += QStringLiteral("\\sa%1").arg(twips);
        }
        if (blockFmt.topMargin() > 0) {
            int twips = static_cast<int>(blockFmt.topMargin() * (1440.0 / 96.0));
            body += QStringLiteral("\\sb%1").arg(twips);
        }

        // Line spacing
        if (blockFmt.lineHeightType() == QTextBlockFormat::ProportionalHeight) {
            int sl = static_cast<int>(blockFmt.lineHeight() * 2.4); // 100% -> 240
            body += QStringLiteral("\\sl%1\\slmult1").arg(sl);
        }

        // Check if block is in list
        if (block.textList()) {
            body += QStringLiteral("{\\pntext\\bullet\\tab}{\\*\\pn\\pnlvlblt\\pnf1\\pnindent0{\\pntxtb\\'b7}}");
        }

        body += QStringLiteral(" ");

        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid()) continue;

            QTextCharFormat charFmt = fragment.charFormat();

            if (charFmt.isImageFormat()) {
                QTextImageFormat imgFmt = charFmt.toImageFormat();
                QString imgName = imgFmt.name();
                QImage img;

                QVariant resource = document->resource(QTextDocument::ImageResource, QUrl(imgName));
                if (resource.isValid()) {
                    if (resource.canConvert<QImage>()) {
                        img = resource.value<QImage>();
                    } else if (resource.canConvert<QPixmap>()) {
                        img = resource.value<QPixmap>().toImage();
                    }
                }

                if (img.isNull()) {
                    resource = document->resource(QTextDocument::ImageResource, QUrl::fromLocalFile(imgName));
                    if (resource.isValid() && resource.canConvert<QImage>()) {
                        img = resource.value<QImage>();
                    }
                }

                if (img.isNull()) {
                    resource = document->resource(QTextDocument::ImageResource, QUrl::fromUserInput(imgName));
                    if (resource.isValid() && resource.canConvert<QImage>()) {
                        img = resource.value<QImage>();
                    }
                }

                if (img.isNull()) {
                    img.load(imgName);
                }

                if (!img.isNull()) {
                    QByteArray ba;
                    QBuffer buf(&ba);
                    buf.open(QIODevice::WriteOnly);
                    img.save(&buf, "PNG");
                    buf.close();

                    int wGoal = imgFmt.width() > 0 ? static_cast<int>(imgFmt.width() * (1440.0 / 96.0)) : static_cast<int>(img.width() * (1440.0 / 96.0));
                    int hGoal = imgFmt.height() > 0 ? static_cast<int>(imgFmt.height() * (1440.0 / 96.0)) : static_cast<int>(img.height() * (1440.0 / 96.0));

                    body += QStringLiteral("{\\pict\\pngblip\\picw%1\\pich%2\\picwgoal%3\\pichgoal%4\n")
                                .arg(img.width())
                                .arg(img.height())
                                .arg(wGoal)
                                .arg(hGoal);
                    body += ba.toHex();
                    body += QStringLiteral("}");
                }
                continue;
            }

            body += QStringLiteral("{");

            QString family = charFmt.fontFamilies().toStringList().value(0, QStringLiteral("Calibri"));
            int fontIdx = getFontIndex(family);
            body += QStringLiteral("\\f%1").arg(fontIdx);

            if (charFmt.fontPointSize() > 0) {
                int halfPoints = static_cast<int>(charFmt.fontPointSize() * 2.0 + 0.5);
                body += QStringLiteral("\\fs%1").arg(halfPoints);
            }

            if (charFmt.fontWeight() >= QFont::Bold) body += QStringLiteral("\\b");
            if (charFmt.fontItalic()) body += QStringLiteral("\\i");
            if (charFmt.fontUnderline()) body += QStringLiteral("\\ul");
            if (charFmt.fontStrikeOut()) body += QStringLiteral("\\strike");
            if (charFmt.verticalAlignment() == QTextCharFormat::AlignSubScript) body += QStringLiteral("\\sub");
            else if (charFmt.verticalAlignment() == QTextCharFormat::AlignSuperScript) body += QStringLiteral("\\super");

            // Foreground text color (only if explicitly set and not default black)
            if (charFmt.foreground().style() != Qt::NoBrush && charFmt.foreground().color().isValid()) {
                QColor c = charFmt.foreground().color();
                if (c.alpha() > 0 && c != Qt::black && (c.red() > 0 || c.green() > 0 || c.blue() > 0)) {
                    int cIdx = getColorIndex(c);
                    if (cIdx > 0) body += QStringLiteral("\\cf%1").arg(cIdx);
                }
            }

            // Background highlight (only if explicitly set and not transparent/NoBrush)
            if (charFmt.background().style() != Qt::NoBrush && charFmt.background().color().isValid()) {
                QColor bg = charFmt.background().color();
                if (bg.alpha() > 0 && bg != Qt::transparent && bg != Qt::white) {
                    int bgIdx = getColorIndex(bg);
                    if (bgIdx > 0) body += QStringLiteral("\\highlight%1").arg(bgIdx);
                }
            }

            body += QStringLiteral(" ");

            // Text with RTF escape & Unicode support
            QString text = fragment.text();
            for (int i = 0; i < text.length(); ++i) {
                QChar ch = text.at(i);
                ushort ucode = ch.unicode();
                if (ucode == 0xFFFC) continue; // Skip Qt object replacement character
                if (ucode == '\\') body += QStringLiteral("\\\\");
                else if (ucode == '{') body += QStringLiteral("\\{");
                else if (ucode == '}') body += QStringLiteral("\\}");
                else if (ucode == '\n' || ucode == 0x2028) body += QStringLiteral("\\line ");
                else if (ucode < 128 && ucode >= 32) body += ch;
                else if (ucode == '\t') body += QStringLiteral("\\tab ");
                else {
                    body += QStringLiteral("\\u%1?").arg(static_cast<short>(ucode));
                }
            }

            body += QStringLiteral("}");
        }

        body += QStringLiteral("\\par}\n");
    }

    // Build Header
    QString header = QStringLiteral("{\\rtf1\\ansi\\ansicpg1252\\deff0\\nouicompat\\deflang1033");

    // Font table
    if (fontTable.isEmpty()) fontTable.append(QStringLiteral("Calibri"));
    header += QStringLiteral("{\\fonttbl");
    for (int i = 0; i < fontTable.size(); ++i) {
        header += QStringLiteral("{\\f%1\\fnil\\fcharset0 %2;}").arg(i).arg(fontTable[i]);
    }
    header += QStringLiteral("}\n");

    // Color table
    if (colorTable.size() > 1) {
        header += QStringLiteral("{\\colortbl ;");
        for (int i = 1; i < colorTable.size(); ++i) {
            const QColor &c = colorTable[i];
            header += QStringLiteral("\\red%1\\green%2\\blue%3;").arg(c.red()).arg(c.green()).arg(c.blue());
        }
        header += QStringLiteral("}\n");
    }

    // Page margins (1 inch default = 1440 twips)
    header += QStringLiteral("\\viewkind4\\uc1\\pard\\margl1440\\margr1440\\margt1440\\margb1440\n");

    return header + body + QStringLiteral("}\n");
}

bool RtfEngine::importRtf(const QByteArray &rtfData, QTextDocument *document) {
    if (!document || rtfData.isEmpty()) return false;

    document->clear();
    QTextCursor cursor(document);

    QList<QString> fontTable;
    QList<QColor> colorTable;
    colorTable.append(QColor()); // 0 index is default

    QStack<RtfState> stateStack;
    RtfState currentState;
    stateStack.push(currentState);

    int pos = 0;
    int len = rtfData.length();
    static int s_imageCounter = 1;

    auto skipGroup = [&](int &p) {
        int depth = 1;
        while (p < len && depth > 0) {
            char c = rtfData[p++];
            if (c == '\\') {
                if (p < len) p++; // skip escaped char
            } else if (c == '{') {
                depth++;
            } else if (c == '}') {
                depth--;
            }
        }
    };

    auto parsePictHex = [&](int hexStart, int hexEnd) {
        int p = hexStart;
        // Skip RTF control words (\pngblip, \picwgoalN, etc.) and leading whitespace
        while (p < hexEnd) {
            if (rtfData[p] == '\\') {
                p++;
                while (p < hexEnd && ((rtfData[p] >= 'a' && rtfData[p] <= 'z') || 
                                      (rtfData[p] >= 'A' && rtfData[p] <= 'Z') || 
                                      (rtfData[p] >= '0' && rtfData[p] <= '9') || 
                                      rtfData[p] == '-')) {
                    p++;
                }
                if (p < hexEnd && rtfData[p] == ' ') p++;
            } else if (rtfData[p] == ' ' || rtfData[p] == '\r' || rtfData[p] == '\n' || rtfData[p] == '\t') {
                p++;
            } else {
                break;
            }
        }

        QByteArray hex;
        while (p < hexEnd) {
            char c = rtfData[p++];
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                hex.append(c);
            }
        }

        if (!hex.isEmpty()) {
            QByteArray imgData = QByteArray::fromHex(hex);
            QImage img = QImage::fromData(imgData);
            if (!img.isNull()) {
                QString imgName = QStringLiteral("rtf_img_%1.png").arg(s_imageCounter++);
                document->addResource(QTextDocument::ImageResource, QUrl(imgName), img);
                cursor.insertImage(imgName);
            }
        }
    };

    // Parse Font Table
    int fontTblPos = rtfData.indexOf("{\\fonttbl");
    if (fontTblPos != -1) {
        int p = fontTblPos + 9;
        while (p < len && rtfData[p] != '}') {
            if (rtfData[p] == '{') {
                p++;
                // Find \fN
                int fIdx = -1;
                if (rtfData.mid(p, 2) == "\\f") {
                    p += 2;
                    int numStart = p;
                    while (p < len && rtfData[p] >= '0' && rtfData[p] <= '9') p++;
                    fIdx = rtfData.mid(numStart, p - numStart).toInt();
                }
                // Skip until space or family name
                while (p < len && rtfData[p] != ';' && rtfData[p] != '}') {
                    if (rtfData[p] == ' ') {
                        p++;
                        int nameStart = p;
                        while (p < len && rtfData[p] != ';' && rtfData[p] != '}') p++;
                        QString fontName = QString::fromUtf8(rtfData.mid(nameStart, p - nameStart)).trimmed();
                        while (fontTable.size() <= fIdx) fontTable.append(QStringLiteral("Calibri"));
                        if (fIdx >= 0 && fIdx < 100) fontTable[fIdx] = fontName;
                        break;
                    }
                    p++;
                }
                while (p < len && rtfData[p] != '}') p++;
                if (p < len && rtfData[p] == '}') p++;
            } else {
                p++;
            }
        }
    }

    // Parse Color Table
    int colTblPos = rtfData.indexOf("{\\colortbl");
    if (colTblPos != -1) {
        int p = colTblPos + 10;
        int r = 0, g = 0, b = 0;
        while (p < len && rtfData[p] != '}') {
            if (rtfData.mid(p, 4) == "\\red") {
                p += 4;
                int start = p;
                while (p < len && rtfData[p] >= '0' && rtfData[p] <= '9') p++;
                r = rtfData.mid(start, p - start).toInt();
            } else if (rtfData.mid(p, 6) == "\\green") {
                p += 6;
                int start = p;
                while (p < len && rtfData[p] >= '0' && rtfData[p] <= '9') p++;
                g = rtfData.mid(start, p - start).toInt();
            } else if (rtfData.mid(p, 5) == "\\blue") {
                p += 5;
                int start = p;
                while (p < len && rtfData[p] >= '0' && rtfData[p] <= '9') p++;
                b = rtfData.mid(start, p - start).toInt();
            } else if (rtfData[p] == ';') {
                colorTable.append(QColor(r, g, b));
                r = g = b = 0;
                p++;
            } else {
                p++;
            }
        }
    }

    // Main body parser
    pos = 0;
    while (pos < len) {
        char ch = rtfData[pos++];

        if (ch == '{') {
            // Check for special groups to ignore like stylesheet, generator, info
            if (pos + 1 < len && rtfData[pos] == '\\') {
                if (rtfData.mid(pos, 11) == "\\stylesheet" ||
                    rtfData.mid(pos, 10) == "\\generator" ||
                    rtfData.mid(pos, 5) == "\\info" ||
                    rtfData.mid(pos, 8) == "\\fonttbl" ||
                    rtfData.mid(pos, 9) == "\\colortbl") {
                    skipGroup(pos);
                    continue;
                }
                // Check for pict
                if (rtfData.mid(pos, 5) == "\\pict") {
                    pos += 5;
                    int hexStart = pos;
                    int hexEnd = rtfData.indexOf('}', hexStart);
                    if (hexEnd != -1) {
                        parsePictHex(hexStart, hexEnd);
                        pos = hexEnd + 1;
                        continue;
                    }
                }
            }
            stateStack.push(currentState);
        } else if (ch == '}') {
            if (!stateStack.isEmpty()) {
                currentState = stateStack.pop();
            }
        } else if (ch == '\\') {
            // Control word or symbol
            if (pos >= len) break;
            char nextChar = rtfData[pos];

            if (nextChar == '\\' || nextChar == '{' || nextChar == '}') {
                pos++;
                QTextCharFormat fmt;
                fmt.setFontFamilies({currentState.fontName});
                fmt.setFontPointSize(currentState.fontSize / 2.0);
                fmt.setFontWeight(currentState.bold ? QFont::Bold : QFont::Normal);
                fmt.setFontItalic(currentState.italic);
                fmt.setFontUnderline(currentState.underline);
                fmt.setFontStrikeOut(currentState.strike);
                if (currentState.subscript) fmt.setVerticalAlignment(QTextCharFormat::AlignSubScript);
                else if (currentState.superscript) fmt.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
                fmt.setForeground(currentState.textColor);
                if (currentState.bgColor != Qt::transparent) fmt.setBackground(currentState.bgColor);
                cursor.insertText(QString(nextChar), fmt);
                continue;
            }

            if (nextChar == '\'') {
                // Hex character
                pos++;
                if (pos + 2 <= len) {
                    QByteArray hex = rtfData.mid(pos, 2);
                    pos += 2;
                    bool ok = false;
                    int code = hex.toInt(&ok, 16);
                    if (ok) {
                        QTextCharFormat fmt;
                        fmt.setFontFamilies({currentState.fontName});
                        fmt.setFontPointSize(currentState.fontSize / 2.0);
                        fmt.setFontWeight(currentState.bold ? QFont::Bold : QFont::Normal);
                        fmt.setFontItalic(currentState.italic);
                        fmt.setFontUnderline(currentState.underline);
                        fmt.setForeground(currentState.textColor);
                        cursor.insertText(QString(QChar(code)), fmt);
                    }
                }
                continue;
            }

            // Read control word
            int kwStart = pos;
            while (pos < len && ((rtfData[pos] >= 'a' && rtfData[pos] <= 'z') || (rtfData[pos] >= 'A' && rtfData[pos] <= 'Z') || rtfData[pos] == '*')) {
                pos++;
            }
            QString kw = QString::fromLatin1(rtfData.mid(kwStart, pos - kwStart));

            // Read optional parameter
            int paramStart = pos;
            bool hasParam = false;
            bool isNeg = false;
            if (pos < len && rtfData[pos] == '-') {
                isNeg = true;
                pos++;
            }
            while (pos < len && rtfData[pos] >= '0' && rtfData[pos] <= '9') {
                hasParam = true;
                pos++;
            }
            int param = 0;
            if (hasParam) {
                param = rtfData.mid(paramStart, pos - paramStart).toInt();
                if (isNeg) param = -param;
            }

            // Skip single space after control word
            if (pos < len && rtfData[pos] == ' ') {
                pos++;
            }

            // Process control words
            if (kw == QStringLiteral("b")) {
                currentState.bold = !hasParam || param != 0;
            } else if (kw == QStringLiteral("i")) {
                currentState.italic = !hasParam || param != 0;
            } else if (kw == QStringLiteral("ul") || kw == QStringLiteral("ulnone")) {
                currentState.underline = (kw == QStringLiteral("ul")) && (!hasParam || param != 0);
            } else if (kw == QStringLiteral("strike")) {
                currentState.strike = !hasParam || param != 0;
            } else if (kw == QStringLiteral("sub")) {
                currentState.subscript = true;
                currentState.superscript = false;
            } else if (kw == QStringLiteral("super")) {
                currentState.superscript = true;
                currentState.subscript = false;
            } else if (kw == QStringLiteral("nosupersub")) {
                currentState.subscript = false;
                currentState.superscript = false;
            } else if (kw == QStringLiteral("f") && hasParam) {
                if (param >= 0 && param < fontTable.size()) {
                    currentState.fontName = fontTable[param];
                }
            } else if (kw == QStringLiteral("fs") && hasParam) {
                currentState.fontSize = param;
            } else if (kw == QStringLiteral("cf") && hasParam) {
                if (param > 0 && param < colorTable.size()) {
                    currentState.textColor = colorTable[param];
                } else {
                    currentState.textColor = Qt::black;
                }
            } else if (kw == QStringLiteral("highlight") && hasParam) {
                if (param > 0 && param < colorTable.size()) {
                    currentState.bgColor = colorTable[param];
                } else {
                    currentState.bgColor = Qt::transparent;
                }
            } else if (kw == QStringLiteral("ql")) {
                currentState.alignment = Qt::AlignLeft;
            } else if (kw == QStringLiteral("qc")) {
                currentState.alignment = Qt::AlignHCenter;
            } else if (kw == QStringLiteral("qr")) {
                currentState.alignment = Qt::AlignRight;
            } else if (kw == QStringLiteral("qj")) {
                currentState.alignment = Qt::AlignJustify;
            } else if (kw == QStringLiteral("li") && hasParam) {
                currentState.leftIndent = param;
            } else if (kw == QStringLiteral("ri") && hasParam) {
                currentState.rightIndent = param;
            } else if (kw == QStringLiteral("fi") && hasParam) {
                currentState.firstLineIndent = param;
            } else if (kw == QStringLiteral("sa") && hasParam) {
                currentState.spaceAfter = param;
            } else if (kw == QStringLiteral("par")) {
                QTextBlockFormat bf;
                bf.setAlignment(currentState.alignment);
                if (currentState.leftIndent > 0) bf.setLeftMargin(currentState.leftIndent * (96.0 / 1440.0));
                if (currentState.rightIndent > 0) bf.setRightMargin(currentState.rightIndent * (96.0 / 1440.0));
                if (currentState.firstLineIndent != 0) bf.setTextIndent(currentState.firstLineIndent * (96.0 / 1440.0));
                if (currentState.spaceAfter > 0) bf.setBottomMargin(currentState.spaceAfter * (96.0 / 1440.0));
                cursor.insertBlock(bf);
            } else if (kw == QStringLiteral("line")) {
                cursor.insertText(QStringLiteral("\n"));
            } else if (kw == QStringLiteral("tab")) {
                cursor.insertText(QStringLiteral("\t"));
            } else if (kw == QStringLiteral("pict")) {
                int hexStart = pos;
                int hexEnd = rtfData.indexOf('}', hexStart);
                if (hexEnd != -1) {
                    parsePictHex(hexStart, hexEnd);
                    pos = hexEnd;
                }
            } else if (kw == QStringLiteral("u") && hasParam) {
                ushort ucode = static_cast<ushort>(param);
                QTextCharFormat fmt;
                fmt.setFontFamilies({currentState.fontName});
                fmt.setFontPointSize(currentState.fontSize / 2.0);
                fmt.setFontWeight(currentState.bold ? QFont::Bold : QFont::Normal);
                fmt.setFontItalic(currentState.italic);
                fmt.setFontUnderline(currentState.underline);
                fmt.setForeground(currentState.textColor);
                cursor.insertText(QString(QChar(ucode)), fmt);
                // Skip fallback character
                if (pos < len && rtfData[pos] == '?') pos++;
            }
        } else if (ch == '\r' || ch == '\n') {
            // RTF ignorable whitespace
            continue;
        } else {
            // Plain text character
            QTextCharFormat fmt;
            fmt.setFontFamilies({currentState.fontName});
            fmt.setFontPointSize(currentState.fontSize / 2.0);
            fmt.setFontWeight(currentState.bold ? QFont::Bold : QFont::Normal);
            fmt.setFontItalic(currentState.italic);
            fmt.setFontUnderline(currentState.underline);
            fmt.setFontStrikeOut(currentState.strike);
            if (currentState.subscript) fmt.setVerticalAlignment(QTextCharFormat::AlignSubScript);
            else if (currentState.superscript) fmt.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
            fmt.setForeground(currentState.textColor);
            if (currentState.bgColor != Qt::transparent) fmt.setBackground(currentState.bgColor);

            cursor.insertText(QString(ch), fmt);
        }
    }

    return true;
}

} // namespace OpenWordPad
