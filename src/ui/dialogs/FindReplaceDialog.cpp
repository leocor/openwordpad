#include "FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

namespace OpenWordPad {

FindReplaceDialog::FindReplaceDialog(bool isReplaceMode, QWidget *parent)
    : QDialog(parent), m_isReplaceMode(isReplaceMode)
{
    setWindowTitle(isReplaceMode ? QStringLiteral("Replace") : QStringLiteral("Find"));
    setFixedWidth(400);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(12);

    // Left Column: Inputs & Options
    auto leftLayout = new QVBoxLayout();

    auto grid = new QGridLayout();
    grid->addWidget(new QLabel(QStringLiteral("Find what:"), this), 0, 0);
    m_editFind = new QLineEdit(this);
    grid->addWidget(m_editFind, 0, 1);

    m_replaceWidget = new QWidget(this);
    auto replLayout = new QHBoxLayout(m_replaceWidget);
    replLayout->setContentsMargins(0, 0, 0, 0);
    replLayout->addWidget(new QLabel(QStringLiteral("Replace with:"), m_replaceWidget));
    m_editReplace = new QLineEdit(m_replaceWidget);
    replLayout->addWidget(m_editReplace);
    grid->addWidget(m_replaceWidget, 1, 0, 1, 2);

    leftLayout->addLayout(grid);

    m_chkWholeWord = new QCheckBox(QStringLiteral("Match whole word only"), this);
    m_chkMatchCase = new QCheckBox(QStringLiteral("Match case"), this);
    leftLayout->addWidget(m_chkWholeWord);
    leftLayout->addWidget(m_chkMatchCase);

    // Direction (only relevant in Find mode)
    auto dirGroup = new QGroupBox(QStringLiteral("Direction"), this);
    auto dirLayout = new QHBoxLayout(dirGroup);
    m_rbUp = new QRadioButton(QStringLiteral("Up"), this);
    m_rbDown = new QRadioButton(QStringLiteral("Down"), this);
    m_rbDown->setChecked(true);
    dirLayout->addWidget(m_rbUp);
    dirLayout->addWidget(m_rbDown);
    leftLayout->addWidget(dirGroup);

    mainLayout->addLayout(leftLayout);

    // Right Column: Action Buttons
    auto btnLayout = new QVBoxLayout();
    m_btnFindNext = new QPushButton(QStringLiteral("Find Next"), this);
    m_btnFindNext->setDefault(true);
    m_btnReplace = new QPushButton(QStringLiteral("Replace"), this);
    m_btnReplaceAll = new QPushButton(QStringLiteral("Replace All"), this);
    auto btnCancel = new QPushButton(QStringLiteral("Cancel"), this);

    btnLayout->addWidget(m_btnFindNext);
    btnLayout->addWidget(m_btnReplace);
    btnLayout->addWidget(m_btnReplaceAll);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    connect(m_btnFindNext, &QPushButton::clicked, this, &FindReplaceDialog::onFindClicked);
    connect(m_btnReplace, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceClicked);
    connect(m_btnReplaceAll, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAllClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    setReplaceMode(isReplaceMode);
}

void FindReplaceDialog::setReplaceMode(bool replace) {
    m_isReplaceMode = replace;
    setWindowTitle(replace ? QStringLiteral("Replace") : QStringLiteral("Find"));
    m_replaceWidget->setVisible(replace);
    m_btnReplace->setVisible(replace);
    m_btnReplaceAll->setVisible(replace);
}

QString FindReplaceDialog::findText() const {
    return m_editFind->text();
}

QString FindReplaceDialog::replaceText() const {
    return m_editReplace->text();
}

QTextDocument::FindFlags FindReplaceDialog::findFlags() const {
    QTextDocument::FindFlags flags;
    if (m_chkWholeWord->isChecked()) flags |= QTextDocument::FindWholeWords;
    if (m_chkMatchCase->isChecked()) flags |= QTextDocument::FindCaseSensitively;
    if (!m_isReplaceMode && m_rbUp->isChecked()) flags |= QTextDocument::FindBackward;
    return flags;
}

void FindReplaceDialog::onFindClicked() {
    emit findNextRequested(findText(), findFlags());
}

void FindReplaceDialog::onReplaceClicked() {
    emit replaceRequested(findText(), replaceText(), findFlags());
}

void FindReplaceDialog::onReplaceAllClicked() {
    emit replaceAllRequested(findText(), replaceText(), findFlags());
}

} // namespace OpenWordPad
