#include "PageSetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QPainter>

namespace OpenWordPad {

PageSetupDialog::PageSetupDialog(const PageSettings &settings, UnitType unit, QWidget *parent)
    : QDialog(parent), m_unit(unit)
{
    setWindowTitle(QStringLiteral("Page Setup"));
    setFixedSize(450, 380);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    auto topLayout = new QHBoxLayout();

    // Left controls column
    auto controlsLayout = new QVBoxLayout();

    // Paper Group
    auto paperGroup = new QGroupBox(QStringLiteral("Paper"), this);
    auto paperGrid = new QGridLayout(paperGroup);
    paperGrid->addWidget(new QLabel(QStringLiteral("Size:"), this), 0, 0);
    m_cbSize = new QComboBox(this);
    m_cbSize->addItem(QStringLiteral("A4 (210 x 297 mm)"), QPageSize::A4);
    m_cbSize->addItem(QStringLiteral("Letter (8.5 x 11 in)"), QPageSize::Letter);
    m_cbSize->addItem(QStringLiteral("Legal (8.5 x 14 in)"), QPageSize::Legal);
    m_cbSize->addItem(QStringLiteral("A3 (297 x 420 mm)"), QPageSize::A3);
    m_cbSize->addItem(QStringLiteral("A5 (148 x 210 mm)"), QPageSize::A5);
    m_cbSize->addItem(QStringLiteral("Executive (7.25 x 10.5 in)"), QPageSize::Executive);
    
    int sizeIdx = m_cbSize->findData(settings.pageSizeId);
    if (sizeIdx >= 0) m_cbSize->setCurrentIndex(sizeIdx);
    paperGrid->addWidget(m_cbSize, 0, 1);
    controlsLayout->addWidget(paperGroup);

    // Orientation Group
    auto orientGroup = new QGroupBox(QStringLiteral("Orientation"), this);
    auto orientLayout = new QVBoxLayout(orientGroup);
    m_rbPortrait = new QRadioButton(QStringLiteral("Portrait"), this);
    m_rbLandscape = new QRadioButton(QStringLiteral("Landscape"), this);
    if (settings.orientation == QPageLayout::Landscape) {
        m_rbLandscape->setChecked(true);
    } else {
        m_rbPortrait->setChecked(true);
    }
    orientLayout->addWidget(m_rbPortrait);
    orientLayout->addWidget(m_rbLandscape);
    controlsLayout->addWidget(orientGroup);

    // Margins Group
    QString suffix = QStringLiteral(" ") + Units::unitSuffix(m_unit);
    auto marginGroup = new QGroupBox(QStringLiteral("Margins (") + Units::unitName(m_unit) + QStringLiteral(")"), this);
    auto marginGrid = new QGridLayout(marginGroup);

    auto makeSpin = [&](double valInches) {
        auto spin = new QDoubleSpinBox(this);
        spin->setRange(0.0, 10.0);
        spin->setSingleStep(0.1);
        spin->setSuffix(suffix);
        double val = (m_unit == UnitType::Centimeters) ? valInches * 2.54 : valInches;
        spin->setValue(val);
        connect(spin, &QDoubleSpinBox::valueChanged, this, &PageSetupDialog::updatePreview);
        return spin;
    };

    marginGrid->addWidget(new QLabel(QStringLiteral("Left:"), this), 0, 0);
    m_spinLeft = makeSpin(settings.leftMargin);
    marginGrid->addWidget(m_spinLeft, 0, 1);

    marginGrid->addWidget(new QLabel(QStringLiteral("Right:"), this), 0, 2);
    m_spinRight = makeSpin(settings.rightMargin);
    marginGrid->addWidget(m_spinRight, 0, 3);

    marginGrid->addWidget(new QLabel(QStringLiteral("Top:"), this), 1, 0);
    m_spinTop = makeSpin(settings.topMargin);
    marginGrid->addWidget(m_spinTop, 1, 1);

    marginGrid->addWidget(new QLabel(QStringLiteral("Bottom:"), this), 1, 2);
    m_spinBottom = makeSpin(settings.bottomMargin);
    marginGrid->addWidget(m_spinBottom, 1, 3);

    controlsLayout->addWidget(marginGroup);

    // Page Numbers Checkbox
    m_chkPageNumbers = new QCheckBox(QStringLiteral("Print page numbers"), this);
    m_chkPageNumbers->setChecked(settings.printPageNumbers);
    controlsLayout->addWidget(m_chkPageNumbers);

    topLayout->addLayout(controlsLayout);

    // Right Preview Group
    auto previewGroup = new QGroupBox(QStringLiteral("Preview"), this);
    auto prevLayout = new QVBoxLayout(previewGroup);
    m_previewWidget = new QWidget(this);
    m_previewWidget->setFixedSize(120, 170);
    m_previewWidget->setStyleSheet(QStringLiteral("background: white; border: 1px solid #999;"));
    prevLayout->addWidget(m_previewWidget, 0, Qt::AlignCenter);
    topLayout->addWidget(previewGroup);

    mainLayout->addLayout(topLayout);

    // Dialog Buttons
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

    connect(m_cbSize, &QComboBox::currentIndexChanged, this, &PageSetupDialog::updatePreview);
    connect(m_rbPortrait, &QRadioButton::toggled, this, &PageSetupDialog::updatePreview);
}

void PageSetupDialog::updatePreview() {
    m_previewWidget->update();
}

PageSettings PageSetupDialog::pageSettings() const {
    PageSettings s;
    s.pageSizeId = static_cast<QPageSize::PageSizeId>(m_cbSize->currentData().toInt());
    s.orientation = m_rbLandscape->isChecked() ? QPageLayout::Landscape : QPageLayout::Portrait;
    
    double factor = (m_unit == UnitType::Centimeters) ? (1.0 / 2.54) : 1.0;
    s.leftMargin = m_spinLeft->value() * factor;
    s.rightMargin = m_spinRight->value() * factor;
    s.topMargin = m_spinTop->value() * factor;
    s.bottomMargin = m_spinBottom->value() * factor;
    s.printPageNumbers = m_chkPageNumbers->isChecked();
    return s;
}

} // namespace OpenWordPad
