#include "TranslationManager.h"
#include <QDebug>

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

    // Detect system locale
    QString sysLocale = QLocale::system().name(); // e.g. "it_IT", "zh_CN", "es_ES"
    
    // Check exact match first
    for (const auto &info : availableLanguages()) {
        if (sysLocale.startsWith(info.code, Qt::CaseInsensitive) || (info.code.contains('_') && sysLocale.compare(info.code, Qt::CaseInsensitive) == 0)) {
            setLanguage(info.code);
            return;
        }
    }

    // Check 2-letter prefix match
    QString prefix = sysLocale.left(2).toLower();
    for (const auto &info : availableLanguages()) {
        if (info.code.startsWith(prefix, Qt::CaseInsensitive)) {
            setLanguage(info.code);
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
