#pragma once

#include <QDialog>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include "../../core/Units.h"

namespace OpenWordPad {

class ParagraphDialog : public QDialog {
    Q_OBJECT

public:
    explicit ParagraphDialog(double leftIndent, double rightIndent, double firstLineIndent,
                             double lineSpacing, bool spaceAfter, Qt::Alignment align,
                             UnitType unit, QWidget *parent = nullptr);

    double leftIndent() const;
    double rightIndent() const;
    double firstLineIndent() const;
    double lineSpacing() const;
    bool addSpaceAfter() const;
    Qt::Alignment alignment() const;

private:
    UnitType m_unit;
    QDoubleSpinBox *m_spinLeft;
    QDoubleSpinBox *m_spinRight;
    QDoubleSpinBox *m_spinFirstLine;
    QComboBox *m_cbLineSpacing;
    QCheckBox *m_chkSpaceAfter;
    QComboBox *m_cbAlignment;
};

} // namespace OpenWordPad
