#include "TranslationManager.h"
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace OpenWordPad {

TranslationManager &TranslationManager::instance() {
    static TranslationManager s_instance;
    return s_instance;
}

TranslationManager::TranslationManager()
    : m_translator(nullptr)
    , m_currentLang(QStringLiteral("en"))
{
}

TranslationManager::~TranslationManager() {
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QList<LanguageInfo> TranslationManager::availableLanguages() const {
    return {
        { QStringLiteral("en"), QStringLiteral("English"), QStringLiteral("English") },
        { QStringLiteral("it"), QStringLiteral("Italiano"), QStringLiteral("Italian") },
        { QStringLiteral("es"), QStringLiteral("Español"), QStringLiteral("Spanish") },
        { QStringLiteral("fr"), QStringLiteral("Français"), QStringLiteral("French") },
        { QStringLiteral("de"), QStringLiteral("Deutsch"), QStringLiteral("German") },
        { QStringLiteral("pt"), QStringLiteral("Português"), QStringLiteral("Portuguese") },
        { QStringLiteral("ru"), QStringLiteral("Русский"), QStringLiteral("Russian") },
        { QStringLiteral("zh_CN"), QStringLiteral("简体中文"), QStringLiteral("Chinese (Simplified)") },
        { QStringLiteral("zh_TW"), QStringLiteral("繁體中文"), QStringLiteral("Chinese (Traditional)") },
        { QStringLiteral("ja"), QStringLiteral("日本語"), QStringLiteral("Japanese") },
        { QStringLiteral("ko"), QStringLiteral("한국어"), QStringLiteral("Korean") },
        { QStringLiteral("ar"), QStringLiteral("العربية"), QStringLiteral("Arabic") },
        { QStringLiteral("hi"), QStringLiteral("हिन्दी"), QStringLiteral("Hindi") },
        { QStringLiteral("nl"), QStringLiteral("Nederlands"), QStringLiteral("Dutch") },
        { QStringLiteral("pl"), QStringLiteral("Polski"), QStringLiteral("Polish") },
        { QStringLiteral("tr"), QStringLiteral("Türkçe"), QStringLiteral("Turkish") },
        { QStringLiteral("sv"), QStringLiteral("Svenska"), QStringLiteral("Swedish") },
        { QStringLiteral("el"), QStringLiteral("Ελληνικά"), QStringLiteral("Greek") },
        { QStringLiteral("cs"), QStringLiteral("Čeština"), QStringLiteral("Czech") },
        { QStringLiteral("hu"), QStringLiteral("Magyar"), QStringLiteral("Hungarian") },
        { QStringLiteral("uk"), QStringLiteral("Українська"), QStringLiteral("Ukrainian") },
        { QStringLiteral("id"), QStringLiteral("Bahasa Indonesia"), QStringLiteral("Indonesian") }
    };
}

QString TranslationManager::currentLanguage() const {
    return m_currentLang;
}

void TranslationManager::initLanguage() {
    QSettings settings(QStringLiteral("LeonardoCorato"), QStringLiteral("OpenWordPad"));
    QString savedLang = settings.value(QStringLiteral("language")).toString();

    if (!savedLang.isEmpty()) {
        setLanguage(savedLang);
        return;
    }

    auto matchCode = [this](const QString &rawStr) -> QString {
        if (rawStr.isEmpty()) return QString();
        QString clean = rawStr;
        clean.replace('-', '_');

        // Check exact match (e.g. "zh_CN", "zh_TW")
        for (const auto &info : availableLanguages()) {
            if (clean.compare(info.code, Qt::CaseInsensitive) == 0) {
                return info.code;
            }
        }

        // Check 2-letter prefix match
        QString prefix = clean.left(2).toLower();
        for (const auto &info : availableLanguages()) {
            if (info.code.startsWith(prefix, Qt::CaseInsensitive)) {
                return info.code;
            }
        }
        return QString();
    };

    // 1. Check Windows API UI Language first on Windows (handles Windows Server language packs and session overrides)
#ifdef Q_OS_WIN
    LANGID uiLang = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(uiLang)) {
        case LANG_ITALIAN:    { setLanguage(QStringLiteral("it")); return; }
        case LANG_SPANISH:    { setLanguage(QStringLiteral("es")); return; }
        case LANG_FRENCH:     { setLanguage(QStringLiteral("fr")); return; }
        case LANG_GERMAN:     { setLanguage(QStringLiteral("de")); return; }
        case LANG_PORTUGUESE: { setLanguage(QStringLiteral("pt")); return; }
        case LANG_RUSSIAN:    { setLanguage(QStringLiteral("ru")); return; }
        case LANG_CHINESE:    {
            if (SUBLANGID(uiLang) == SUBLANG_CHINESE_TRADITIONAL || SUBLANGID(uiLang) == SUBLANG_CHINESE_HONGKONG)
                setLanguage(QStringLiteral("zh_TW"));
            else
                setLanguage(QStringLiteral("zh_CN"));
            return;
        }
        case LANG_JAPANESE:   { setLanguage(QStringLiteral("ja")); return; }
        case LANG_KOREAN:     { setLanguage(QStringLiteral("ko")); return; }
        case LANG_ARABIC:     { setLanguage(QStringLiteral("ar")); return; }
        case LANG_HINDI:      { setLanguage(QStringLiteral("hi")); return; }
        case LANG_DUTCH:      { setLanguage(QStringLiteral("nl")); return; }
        case LANG_POLISH:     { setLanguage(QStringLiteral("pl")); return; }
        case LANG_TURKISH:    { setLanguage(QStringLiteral("tr")); return; }
        case LANG_SWEDISH:    { setLanguage(QStringLiteral("sv")); return; }
        case LANG_GREEK:      { setLanguage(QStringLiteral("el")); return; }
        case LANG_CZECH:      { setLanguage(QStringLiteral("cs")); return; }
        case LANG_HUNGARIAN:  { setLanguage(QStringLiteral("hu")); return; }
        case LANG_UKRAINIAN:  { setLanguage(QStringLiteral("uk")); return; }
        case LANG_INDONESIAN: { setLanguage(QStringLiteral("id")); return; }
        default: break;
    }
#endif

    // 2. Check UI preferred languages list from Qt
    QStringList uiLangs = QLocale::system().uiLanguages();
    uiLangs.append(QLocale().uiLanguages());
    for (const QString &lang : uiLangs) {
        QString matched = matchCode(lang);
        if (!matched.isEmpty()) {
            setLanguage(matched);
            return;
        }
    }

    // 3. Check environment variables
    for (const char *var : { "LANGUAGE", "LC_ALL", "LC_MESSAGES", "LANG" }) {
        QByteArray val = qgetenv(var);
        if (!val.isEmpty()) {
            QString matched = matchCode(QString::fromLocal8Bit(val));
            if (!matched.isEmpty()) {
                setLanguage(matched);
                return;
            }
        }
    }

    // 4. Check QLocale::system().name() and QLocale().name()
    for (const QString &locName : { QLocale::system().name(), QLocale().name() }) {
        QString matched = matchCode(locName);
        if (!matched.isEmpty()) {
            setLanguage(matched);
            return;
        }
    }

    // Fallback to English
    setLanguage(QStringLiteral("en"));
}

bool TranslationManager::setLanguage(const QString &langCode) {
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_currentLang = langCode;

    if (langCode == QStringLiteral("en")) {
        QSettings settings(QStringLiteral("LeonardoCorato"), QStringLiteral("OpenWordPad"));
        settings.setValue(QStringLiteral("language"), langCode);
        return true;
    }

    m_translator = new QTranslator();
    QString qmPath = QStringLiteral(":/translations/openwordpad_%1.qm").arg(langCode);
    
    if (m_translator->load(qmPath)) {
        QCoreApplication::installTranslator(m_translator);
        QSettings settings(QStringLiteral("LeonardoCorato"), QStringLiteral("OpenWordPad"));
        settings.setValue(QStringLiteral("language"), langCode);
        return true;
    } else {
        qWarning() << "Failed to load translation:" << qmPath;
        delete m_translator;
        m_translator = nullptr;
        m_currentLang = QStringLiteral("en");
        return false;
    }
}

} // namespace OpenWordPad
