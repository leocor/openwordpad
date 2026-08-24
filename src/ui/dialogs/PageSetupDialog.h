#pragma once

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "../../core/DocumentManager.h"
#include "../../core/Units.h"

namespace OpenWordPad {

class PageSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit PageSetupDialog(const PageSettings &settings, UnitType unit, QWidget *parent = nullptr);

    PageSettings pageSettings() const;

private slots:
    void updatePreview();

private:
    UnitType m_unit;
    QComboBox *m_cbSize;
    QRadioButton *m_rbPortrait;
    QRadioButton *m_rbLandscape;
    QDoubleSpinBox *m_spinLeft;
    QDoubleSpinBox *m_spinRight;
    QDoubleSpinBox *m_spinTop;
    QDoubleSpinBox *m_spinBottom;
    QCheckBox *m_chkPageNumbers;
    QWidget *m_previewWidget;
};

} // namespace OpenWordPad
