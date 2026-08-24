#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextDocument>
#include <QPageSize>
#include <QPageLayout>
#include <QMarginsF>
#include "Units.h"

namespace OpenWordPad {

enum class DocumentFormat {
    Rtf,
    Docx,
    Odt,
    PlainText,
    Html,
    Pdf,
    Unknown
};

struct PageSettings {
    QPageSize::PageSizeId pageSizeId = QPageSize::A4;
    QPageLayout::Orientation orientation = QPageLayout::Portrait;
    double leftMargin = 1.0;   // inches
    double rightMargin = 1.0;  // inches
    double topMargin = 1.0;    // inches
    double bottomMargin = 1.0; // inches
    bool printPageNumbers = true;
};

class DocumentManager : public QObject {
    Q_OBJECT

public:
    explicit DocumentManager(QTextDocument *document, QObject *parent = nullptr);

    bool newDocument();
    bool openFile(const QString &filePath);
    bool saveFile();
    bool saveFileAs(const QString &filePath, DocumentFormat format);
    bool exportToPdf(const QString &filePath);

    QString currentFilePath() const { return m_currentFilePath; }
    QString documentTitle() const;
    bool isModified() const;
    void setModified(bool modified);

    DocumentFormat currentFormat() const { return m_currentFormat; }
    static DocumentFormat formatFromExtension(const QString &filePath);
    static QString filterForFormat(DocumentFormat format);
    static QString allSupportedFilters();

    const PageSettings &pageSettings() const { return m_pageSettings; }
    void setPageSettings(const PageSettings &settings);

    QStringList recentFiles() const { return m_recentFiles; }
    void addRecentFile(const QString &filePath);
    void clearRecentFiles();

signals:
    void documentModifiedChanged(bool modified);
    void documentPathChanged(const QString &path);
    void pageSettingsChanged();
    void recentFilesChanged();
    void errorOccurred(const QString &title, const QString &message);

private:
    void loadRecentFiles();
    void saveRecentFiles();

    QTextDocument *m_document;
    QString m_currentFilePath;
    DocumentFormat m_currentFormat = DocumentFormat::Rtf;
    PageSettings m_pageSettings;
    QStringList m_recentFiles;
    static constexpr int MAX_RECENT_FILES = 10;
};

} // namespace OpenWordPad
