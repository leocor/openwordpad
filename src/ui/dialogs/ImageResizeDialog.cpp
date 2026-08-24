#include "ImageResizeDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

namespace OpenWordPad {

ImageResizeDialog::ImageResizeDialog(int currentWidth, int currentHeight, QWidget *parent)
    : QDialog(parent), m_origWidth(currentWidth), m_origHeight(currentHeight)
{
    setWindowTitle(QStringLiteral("Resize Picture"));
    setFixedSize(300, 200);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    auto group = new QGroupBox(QStringLiteral("Scaling"), this);
    auto grid = new QGridLayout(group);

    grid->addWidget(new QLabel(QStringLiteral("Horizontal:"), this), 0, 0);
    m_spinHoriz = new QDoubleSpinBox(this);
    m_spinHoriz->setRange(1, 500);
    m_spinHoriz->setSuffix(QStringLiteral(" %"));
    m_spinHoriz->setValue(100);
    grid->addWidget(m_spinHoriz, 0, 1);

    grid->addWidget(new QLabel(QStringLiteral("Vertical:"), this), 1, 0);
    m_spinVert = new QDoubleSpinBox(this);
    m_spinVert->setRange(1, 500);
    m_spinVert->setSuffix(QStringLiteral(" %"));
    m_spinVert->setValue(100);
    grid->addWidget(m_spinVert, 1, 1);

    m_chkLockAspect = new QCheckBox(QStringLiteral("Lock aspect ratio"), this);
    m_chkLockAspect->setChecked(true);
    grid->addWidget(m_chkLockAspect, 2, 0, 1, 2);

    mainLayout->addWidget(group);

    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto btnOk = new QPushButton(QStringLiteral("OK"), this);
    btnOk->setDefault(true);
    auto btnCancel = new QPushButton(QStringLiteral("Cancel"), this);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(m_spinHoriz, &QDoubleSpinBox::valueChanged, this, &ImageResizeDialog::onHorizPercentChanged);
    connect(m_spinVert, &QDoubleSpinBox::valueChanged, this, &ImageResizeDialog::onVertPercentChanged);
}

void ImageResizeDialog::onHorizPercentChanged(double val) {
    if (m_chkLockAspect->isChecked()) {
        m_spinVert->blockSignals(true);
        m_spinVert->setValue(val);
        m_spinVert->blockSignals(false);
    }
}

void ImageResizeDialog::onVertPercentChanged(double val) {
    if (m_chkLockAspect->isChecked()) {
        m_spinHoriz->blockSignals(true);
        m_spinHoriz->setValue(val);
        m_spinHoriz->blockSignals(false);
    }
}

int ImageResizeDialog::newWidth() const {
    return static_cast<int>(m_origWidth * (m_spinHoriz->value() / 100.0) + 0.5);
}

int ImageResizeDialog::newHeight() const {
    return static_cast<int>(m_origHeight * (m_spinVert->value() / 100.0) + 0.5);
}

} // namespace OpenWordPad
