#pragma once

#include <QDialog>
#include <QDoubleSpinBox>
#include <QCheckBox>

namespace OpenWordPad {

class ImageResizeDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImageResizeDialog(int currentWidth, int currentHeight, QWidget *parent = nullptr);

    int newWidth() const;
    int newHeight() const;

private slots:
    void onHorizPercentChanged(double val);
    void onVertPercentChanged(double val);

private:
    int m_origWidth;
    int m_origHeight;
    QDoubleSpinBox *m_spinHoriz;
    QDoubleSpinBox *m_spinVert;
    QCheckBox *m_chkLockAspect;
};

} // namespace OpenWordPad
