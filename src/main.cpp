#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStyleFactory>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("LeonardoCorato"));
    app.setOrganizationDomain(QStringLiteral("github.com/leocorato"));
    app.setApplicationName(QStringLiteral("OpenWordPad"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    // Modern clean style
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // Palette with classic clean Windows accents
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
    palette.setColor(QPalette::WindowText, QColor(0x20, 0x20, 0x20));
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(0xf7, 0xf7, 0xf7));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(0xf5, 0xf5, 0xf5));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0x00, 0x78, 0xd7));
    palette.setColor(QPalette::Highlight, QColor(0x00, 0x78, 0xd7));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(palette);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("OpenWordPad - Open Source Microsoft WordPad recreation in Qt6"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("file"), QStringLiteral("File to open."));
    parser.process(app);

    OpenWordPad::MainWindow window;
    window.show();

    const QStringList positionalArgs = parser.positionalArguments();
    if (!positionalArgs.isEmpty()) {
        window.openFile(positionalArgs.first());
    }

    return app.exec();
}
