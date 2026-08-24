#include <QCoreApplication>
#include <QTranslator>
#include <QFile>
#include <QTest>
#include "../src/core/TranslationManager.h"

class TestTranslations : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    void testAllLanguagesLoad() {
        auto &mgr = OpenWordPad::TranslationManager::instance();
        const auto langs = mgr.availableLanguages();
        QCOMPARE(langs.size(), 22);

        for (const auto &info : langs) {
            bool ok = mgr.setLanguage(info.code);
            QVERIFY2(ok, qPrintable(QString("Failed to set language: %1 (%2)").arg(info.code, info.nativeName)));
            QCOMPARE(mgr.currentLanguage(), info.code);
            
            if (info.code != "en") {
                QString qmResource = QString(":/translations/openwordpad_%1.qm").arg(info.code);
                QVERIFY2(QFile::exists(qmResource), qPrintable(QString("Resource missing: %1").arg(qmResource)));
            }
        }
    }
};

QTEST_MAIN(TestTranslations)
#include "test_translations.moc"
