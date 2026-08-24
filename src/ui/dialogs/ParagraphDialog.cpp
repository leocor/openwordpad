#include "ParagraphDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

namespace OpenWordPad {

ParagraphDialog::ParagraphDialog(double leftIndent, double rightIndent, double firstLineIndent,
                                 double lineSpacing, bool spaceAfter, Qt::Alignment align,
                                 UnitType unit, QWidget *parent)
    : QDialog(parent), m_unit(unit)
{
    setWindowTitle(QStringLiteral("Paragraph"));
    setFixedSize(360, 320);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    QString suffix = QStringLiteral(" ") + Units::unitSuffix(m_unit);
    double factor = (m_unit == UnitType::Centimeters) ? 2.54 : 1.0;

    // Indentation Group
    auto indentGroup = new QGroupBox(QStringLiteral("Indentation"), this);
    auto indentGrid = new QGridLayout(indentGroup);

    auto makeSpin = [&](double valInches) {
        auto spin = new QDoubleSpinBox(this);
        spin->setRange(-5.0, 20.0);
        spin->setSingleStep(0.1);
        spin->setSuffix(suffix);
        spin->setValue(valInches * factor);
        return spin;
    };

    indentGrid->addWidget(new QLabel(QStringLiteral("Left:"), this), 0, 0);
    m_spinLeft = makeSpin(leftIndent);
    indentGrid->addWidget(m_spinLeft, 0, 1);

    indentGrid->addWidget(new QLabel(QStringLiteral("Right:"), this), 1, 0);
    m_spinRight = makeSpin(rightIndent);
    indentGrid->addWidget(m_spinRight, 1, 1);

    indentGrid->addWidget(new QLabel(QStringLiteral("First line:"), this), 2, 0);
    m_spinFirstLine = makeSpin(firstLineIndent);
    indentGrid->addWidget(m_spinFirstLine, 2, 1);

    mainLayout->addWidget(indentGroup);

    // Spacing & Alignment Group
    auto spacingGroup = new QGroupBox(QStringLiteral("Spacing"), this);
    auto spacingGrid = new QGridLayout(spacingGroup);

    spacingGrid->addWidget(new QLabel(QStringLiteral("Line spacing:"), this), 0, 0);
    m_cbLineSpacing = new QComboBox(this);
    m_cbLineSpacing->addItem(QStringLiteral("1.0"), 1.0);
    m_cbLineSpacing->addItem(QStringLiteral("1.15"), 1.15);
    m_cbLineSpacing->addItem(QStringLiteral("1.5"), 1.5);
    m_cbLineSpacing->addItem(QStringLiteral("2.0"), 2.0);
    int lsIdx = m_cbLineSpacing->findData(lineSpacing);
    if (lsIdx >= 0) m_cbLineSpacing->setCurrentIndex(lsIdx);
    spacingGrid->addWidget(m_cbLineSpacing, 0, 1);

    m_chkSpaceAfter = new QCheckBox(QStringLiteral("Add 10 pt space after paragraphs"), this);
    m_chkSpaceAfter->setChecked(spaceAfter);
    spacingGrid->addWidget(m_chkSpaceAfter, 1, 0, 1, 2);

    spacingGrid->addWidget(new QLabel(QStringLiteral("Alignment:"), this), 2, 0);
    m_cbAlignment = new QComboBox(this);
    m_cbAlignment->addItem(QStringLiteral("Left"), static_cast<int>(Qt::AlignLeft));
    m_cbAlignment->addItem(QStringLiteral("Center"), static_cast<int>(Qt::AlignHCenter));
    m_cbAlignment->addItem(QStringLiteral("Right"), static_cast<int>(Qt::AlignRight));
    m_cbAlignment->addItem(QStringLiteral("Justified"), static_cast<int>(Qt::AlignJustify));

    int alignIdx = m_cbAlignment->findData(static_cast<int>(align));
    if (alignIdx >= 0) m_cbAlignment->setCurrentIndex(alignIdx);
    spacingGrid->addWidget(m_cbAlignment, 2, 1);

    mainLayout->addWidget(spacingGroup);

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

double ParagraphDialog::leftIndent() const {
    double factor = (m_unit == UnitType::Centimeters) ? (1.0 / 2.54) : 1.0;
    return m_spinLeft->value() * factor;
}

double ParagraphDialog::rightIndent() const {
    double factor = (m_unit == UnitType::Centimeters) ? (1.0 / 2.54) : 1.0;
    return m_spinRight->value() * factor;
}

double ParagraphDialog::firstLineIndent() const {
    double factor = (m_unit == UnitType::Centimeters) ? (1.0 / 2.54) : 1.0;
    return m_spinFirstLine->value() * factor;
}

double ParagraphDialog::lineSpacing() const {
    return m_cbLineSpacing->currentData().toDouble();
}

bool ParagraphDialog::addSpaceAfter() const {
    return m_chkSpaceAfter->isChecked();
}

Qt::Alignment ParagraphDialog::alignment() const {
    return static_cast<Qt::Alignment>(m_cbAlignment->currentData().toInt());
}

} // namespace OpenWordPad
