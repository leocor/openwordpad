#include "DateTimeDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QLocale>

namespace OpenWordPad {

DateTimeDialog::DateTimeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Date and Time"));
    setFixedSize(360, 300);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);

    mainLayout->addWidget(new QLabel(QStringLiteral("Available formats:"), this));

    m_listFormats = new QListWidget(this);
    
    QDateTime now = QDateTime::currentDateTime();
    QLocale loc = QLocale::system();

    QStringList formats = {
        now.toString(QStringLiteral("M/d/yyyy")),
        now.toString(QStringLiteral("MM/dd/yyyy")),
        now.toString(QStringLiteral("yyyy-MM-dd")),
        now.toString(QStringLiteral("dd/MM/yyyy")),
        now.toString(QStringLiteral("d MMMM yyyy")),
        now.toString(QStringLiteral("dddd, MMMM d, yyyy")),
        now.toString(QStringLiteral("dddd, d MMMM yyyy")),
        now.toString(QStringLiteral("h:mm:ss AP")),
        now.toString(QStringLiteral("h:mm AP")),
        now.toString(QStringLiteral("HH:mm:ss")),
        now.toString(QStringLiteral("HH:mm")),
        now.toString(QStringLiteral("M/d/yyyy h:mm AP")),
        now.toString(QStringLiteral("dd/MM/yyyy HH:mm:ss"))
    };

    for (const QString &fmt : formats) {
        m_listFormats->addItem(fmt);
    }
    if (m_listFormats->count() > 0) {
        m_listFormats->setCurrentRow(0);
    }

    mainLayout->addWidget(m_listFormats);

    connect(m_listFormats, &QListWidget::itemDoubleClicked, this, &QDialog::accept);

    // Buttons
    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto btnOk = new QPushButton(QStringLiteral("OK"), this);
    btnOk->setDefault(true);
    btnOk->setFixedWidth(80);
    auto btnCancel = new QPushButton(QStringLiteral("Cancel"), this);
    btnCancel->setFixedWidth(80);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);
}

QString DateTimeDialog::selectedDateTimeString() const {
    auto item = m_listFormats->currentItem();
    return item ? item->text() : QString();
}

} // namespace OpenWordPad
