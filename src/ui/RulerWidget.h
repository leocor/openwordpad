#pragma once

#include <QWidget>
#include "../core/Units.h"

namespace OpenWordPad {

class RulerWidget : public QWidget {
    Q_OBJECT

public:
    explicit RulerWidget(QWidget *parent = nullptr);

    void setUnit(UnitType unit);
    UnitType unit() const { return m_unit; }

    void setPageWidth(double inches);
    void setMargins(double leftInches, double rightInches);
    void setIndents(double firstLineInches, double leftIndentInches, double rightIndentInches);
    void setOriginX(int originX);

signals:
    void firstLineIndentChanged(double inches);
    void leftIndentChanged(double inches);
    void rightIndentChanged(double inches);
    void leftMarginChanged(double inches);
    void rightMarginChanged(double inches);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    UnitType m_unit = UnitType::Centimeters;
    double m_pageWidth = 8.27; // A4 default width in inches (21cm)
    double m_leftMargin = 1.0;
    double m_rightMargin = 1.0;
    double m_firstLineIndent = 0.0;
    double m_leftIndent = 0.0;
    double m_rightIndent = 0.0;
    int m_originX = 20;

    enum class DragTarget {
        None,
        FirstLineIndent,
        LeftIndent,
        LeftMargin,
        RightIndent,
        RightMargin
    };
    DragTarget m_activeDrag = DragTarget::None;

    double inchesToPixelX(double inches) const;
    double pixelXToInches(double px) const;
};

} // namespace OpenWordPad
