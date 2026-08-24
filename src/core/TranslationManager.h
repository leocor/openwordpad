#pragma once

#include <QString>
#include <QStringList>
#include <QPair>
#include <QTranslator>
#include <QCoreApplication>
#include <QLocale>
#include <QSettings>

namespace OpenWordPad {

struct LanguageInfo {
    QString code;
    QString nativeName;
    QString englishName;
};

class TranslationManager {
public:
    static TranslationManager &instance();

    QList<LanguageInfo> availableLanguages() const;
    QString currentLanguage() const;
    bool setLanguage(const QString &langCode);
    void initLanguage();

private:
    TranslationManager();
    ~TranslationManager();

    QTranslator *m_translator;
    QString m_currentLang;
};

} // namespace OpenWordPad
