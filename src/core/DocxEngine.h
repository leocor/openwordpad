#pragma once

#include <QString>
#include <QTextDocument>

namespace OpenWordPad {

class DocxEngine {
public:
    static bool isDocx(const QString &fileName);
    static bool loadDocx(const QString &fileName, QTextDocument *document);
    static bool saveDocx(const QString &fileName, const QTextDocument *document);
};

} // namespace OpenWordPad
