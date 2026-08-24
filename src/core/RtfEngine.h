#pragma once

#include <QString>
#include <QByteArray>
#include <QTextDocument>
#include <QColor>
#include <QFont>
#include <QVector>
#include <QMap>

namespace OpenWordPad {

class RtfEngine {
public:
    static QString exportToRtf(const QTextDocument *document);
    static bool importRtf(const QByteArray &rtfData, QTextDocument *document);
    static bool isRtf(const QByteArray &data);

private:
    // RTF Parser helper structures and methods
    struct RtfState {
        QString fontName = QStringLiteral("Calibri");
        int fontSize = 22; // half-points (11pt)
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strike = false;
        bool subscript = false;
        bool superscript = false;
        QColor textColor = Qt::black;
        QColor bgColor = Qt::transparent;
        Qt::Alignment alignment = Qt::AlignLeft;
        int leftIndent = 0;
        int rightIndent = 0;
        int firstLineIndent = 0;
        int lineSpacing = 100; // percent
        int spaceAfter = 0;
        bool inList = false;
    };
};

} // namespace OpenWordPad
