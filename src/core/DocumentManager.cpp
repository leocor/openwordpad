#include "DocumentManager.h"
#include "RtfEngine.h"
#include "DocxEngine.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QSettings>
#include <QDebug>

namespace OpenWordPad {

DocumentManager::DocumentManager(QTextDocument *document, QObject *parent)
    : QObject(parent), m_document(document)
{
    loadRecentFiles();
    connect(m_document, &QTextDocument::modificationChanged, this, &DocumentManager::documentModifiedChanged);
}

QString DocumentManager::documentTitle() const {
    if (m_currentFilePath.isEmpty()) {
        return QStringLiteral("Documento");
    }
    return QFileInfo(m_currentFilePath).fileName();
}

bool DocumentManager::isModified() const {
    return m_document ? m_document->isModified() : false;
}

void DocumentManager::setModified(bool modified) {
    if (m_document) {
        m_document->setModified(modified);
    }
}

DocumentFormat DocumentManager::formatFromExtension(const QString &filePath) {
    QString ext = QFileInfo(filePath).suffix().toLower();
    if (ext == QStringLiteral("rtf")) return DocumentFormat::Rtf;
    if (ext == QStringLiteral("docx")) return DocumentFormat::Docx;
    if (ext == QStringLiteral("odt")) return DocumentFormat::Odt;
    if (ext == QStringLiteral("txt") || ext == QStringLiteral("text") || ext == QStringLiteral("log")) return DocumentFormat::PlainText;
    if (ext == QStringLiteral("htm") || ext == QStringLiteral("html")) return DocumentFormat::Html;
    if (ext == QStringLiteral("pdf")) return DocumentFormat::Pdf;
    return DocumentFormat::Unknown;
}

QString DocumentManager::filterForFormat(DocumentFormat format) {
    switch (format) {
        case DocumentFormat::Rtf:
            return QStringLiteral("Rich Text Document (*.rtf)");
        case DocumentFormat::Docx:
            return QStringLiteral("Office Open XML Document (*.docx)");
        case DocumentFormat::Odt:
            return QStringLiteral("OpenDocument Text (*.odt)");
        case DocumentFormat::PlainText:
            return QStringLiteral("Plain Text Document (*.txt)");
        case DocumentFormat::Html:
            return QStringLiteral("HTML Document (*.html *.htm)");
        case DocumentFormat::Pdf:
            return QStringLiteral("PDF Document (*.pdf)");
        default:
            return QStringLiteral("All Files (*.*)");
    }
}

QString DocumentManager::allSupportedFilters() {
    return QStringLiteral("All Supported Files (*.rtf *.docx *.odt *.txt *.html *.htm);;"
                          "Rich Text Format (*.rtf);;"
                          "Office Open XML Document (*.docx);;"
                          "OpenDocument Text (*.odt);;"
                          "Plain Text Document (*.txt);;"
                          "HTML Document (*.html *.htm);;"
                          "All Files (*.*)");
}

bool DocumentManager::newDocument() {
    m_document->clear();
    m_currentFilePath.clear();
    m_currentFormat = DocumentFormat::Rtf;
    setModified(false);
    emit documentPathChanged(m_currentFilePath);
    return true;
}

bool DocumentManager::openFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QStringLiteral("Open Failed"), QStringLiteral("Could not open file: ") + file.errorString());
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    DocumentFormat format = formatFromExtension(filePath);
    bool success = false;

    if (format == DocumentFormat::Rtf || RtfEngine::isRtf(data)) {
        success = RtfEngine::importRtf(data, m_document);
        m_currentFormat = DocumentFormat::Rtf;
    } else if (format == DocumentFormat::Docx) {
        success = DocxEngine::loadDocx(filePath, m_document);
        m_currentFormat = DocumentFormat::Docx;
    } else if (format == DocumentFormat::Odt) {
        m_document->setBaseUrl(QUrl::fromLocalFile(filePath));
        QTextDocumentWriter writer;
        // Qt's QTextDocument does not have direct readOdf built into QTextDocument, so we load using standard QTextDocument ODF reader or plaintext
        // For ODT, QTextDocument can load ODF via QTextCodec/QTextStream or standard HTML conversion
        m_document->setPlainText(QString::fromUtf8(data)); // Fallback
        success = true;
        m_currentFormat = DocumentFormat::Odt;
    } else if (format == DocumentFormat::Html) {
        m_document->setHtml(QString::fromUtf8(data));
        success = true;
        m_currentFormat = DocumentFormat::Html;
    } else {
        // Plain text with UTF-8 / Latin1 fallback
        QString text = QString::fromUtf8(data);
        if (text.contains(QChar::ReplacementCharacter)) {
            text = QString::fromLatin1(data);
        }
        m_document->setPlainText(text);
        success = true;
        m_currentFormat = DocumentFormat::PlainText;
    }

    if (success) {
        m_currentFilePath = filePath;
        setModified(false);
        addRecentFile(filePath);
        emit documentPathChanged(m_currentFilePath);
        return true;
    } else {
        emit errorOccurred(QStringLiteral("Open Error"), QStringLiteral("Failed to parse document content."));
        return false;
    }
}

bool DocumentManager::saveFile() {
    if (m_currentFilePath.isEmpty()) {
        return false;
    }
    return saveFileAs(m_currentFilePath, m_currentFormat);
}

bool DocumentManager::saveFileAs(const QString &filePath, DocumentFormat format) {
    if (filePath.isEmpty()) return false;

    if (format == DocumentFormat::Unknown) {
        format = formatFromExtension(filePath);
    }
    if (format == DocumentFormat::Unknown) {
        format = DocumentFormat::Rtf;
    }

    bool success = false;

    if (format == DocumentFormat::Rtf) {
        QString rtf = RtfEngine::exportToRtf(m_document);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(rtf.toUtf8());
            file.close();
            success = true;
        }
    } else if (format == DocumentFormat::Docx) {
        success = DocxEngine::saveDocx(filePath, m_document);
    } else if (format == DocumentFormat::Odt) {
        QTextDocumentWriter writer(filePath, "ODF");
        success = writer.write(m_document);
    } else if (format == DocumentFormat::Html) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream out(&file);
            out << m_document->toHtml();
            file.close();
            success = true;
        }
    } else if (format == DocumentFormat::Pdf) {
        success = exportToPdf(filePath);
    } else {
        // Plain text
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream out(&file);
            out << m_document->toPlainText();
            file.close();
            success = true;
        }
    }

    if (success) {
        m_currentFilePath = filePath;
        m_currentFormat = format;
        setModified(false);
        addRecentFile(filePath);
        emit documentPathChanged(m_currentFilePath);
        return true;
    } else {
        emit errorOccurred(QStringLiteral("Save Error"), QStringLiteral("Failed to write to: ") + filePath);
        return false;
    }
}

bool DocumentManager::exportToPdf(const QString &filePath) {
    if (filePath.isEmpty()) return false;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    
    QPageSize pageSize(m_pageSettings.pageSizeId);
    QPageLayout layout(pageSize, m_pageSettings.orientation,
                       QMarginsF(m_pageSettings.leftMargin * 25.4, m_pageSettings.topMargin * 25.4,
                                 m_pageSettings.rightMargin * 25.4, m_pageSettings.bottomMargin * 25.4),
                       QPageLayout::Millimeter);
    printer.setPageLayout(layout);

    m_document->print(&printer);
    return true;
}

void DocumentManager::setPageSettings(const PageSettings &settings) {
    m_pageSettings = settings;
    emit pageSettingsChanged();
}

void DocumentManager::loadRecentFiles() {
    QSettings settings(QStringLiteral("LeonardoCorato"), QStringLiteral("OpenWordPad"));
    m_recentFiles = settings.value(QStringLiteral("RecentFiles")).toStringList();
}

void DocumentManager::saveRecentFiles() {
    QSettings settings(QStringLiteral("LeonardoCorato"), QStringLiteral("OpenWordPad"));
    settings.setValue(QStringLiteral("RecentFiles"), m_recentFiles);
}

void DocumentManager::addRecentFile(const QString &filePath) {
    if (filePath.isEmpty()) return;
    m_recentFiles.removeAll(filePath);
    m_recentFiles.prepend(filePath);
    while (m_recentFiles.size() > MAX_RECENT_FILES) {
        m_recentFiles.removeLast();
    }
    saveRecentFiles();
    emit recentFilesChanged();
}

void DocumentManager::clearRecentFiles() {
    m_recentFiles.clear();
    saveRecentFiles();
    emit recentFilesChanged();
}

} // namespace OpenWordPad
