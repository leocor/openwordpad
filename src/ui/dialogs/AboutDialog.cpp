#include "AboutDialog.h"
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSysInfo>
#include <QIcon>
#include <QGroupBox>

namespace OpenWordPad {

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("About WordPad"));
    setFixedSize(460, 360);
    setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.svg")));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 16);

    // Top banner with icon and title
    auto topLayout = new QHBoxLayout();
    auto iconLabel = new QLabel(this);
    iconLabel->setPixmap(QIcon(QStringLiteral(":/icons/app_icon.svg")).pixmap(64, 64));
    topLayout->addWidget(iconLabel);

    auto titleLayout = new QVBoxLayout();
    auto appTitle = new QLabel(QStringLiteral("<b><font size='+2'>OpenWordPad</font></b>"), this);
    auto verLabel = new QLabel(QStringLiteral("Version %1 (Qt6 C++ Build)").arg(QStringLiteral(OPENWORDPAD_VERSION)), this);
    auto authorLabel = new QLabel(QStringLiteral("Author: <b>Leonardo Corato</b>"), this);
    
    titleLayout->addWidget(appTitle);
    titleLayout->addWidget(verLabel);
    titleLayout->addWidget(authorLabel);
    topLayout->addLayout(titleLayout);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    // Separator line
    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Information details
    auto infoText = new QLabel(this);
    infoText->setWordWrap(true);
    infoText->setText(
        QStringLiteral("<p>A faithful, high-performance, clean-room recreation of Microsoft WordPad built with modern C++20 and Qt6.</p>"
                       "<p><b>License:</b> GNU General Public License v2.0 (GPLv2)<br>"
                       "<b>Author:</b> Leonardo Corato</p>"
                       "<p>This software is free and open source. You may copy, modify, and distribute it under the terms of the GNU GPLv2.</p>"
                       "<p><small style='color: #666;'><b>Disclaimer:</b> Microsoft, WordPad, and Windows are registered trademarks of Microsoft Corporation. OpenWordPad is an independent open-source project and is not affiliated with, endorsed by, or sponsored by Microsoft Corporation.</small></p>")
    );
    mainLayout->addWidget(infoText);

    mainLayout->addStretch();

    // OK Button
    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto btnOk = new QPushButton(QStringLiteral("OK"), this);
    btnOk->setDefault(true);
    btnOk->setFixedWidth(85);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(btnOk);
    mainLayout->addLayout(btnLayout);
}

} // namespace OpenWordPad
