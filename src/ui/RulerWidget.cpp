#include "RulerWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

namespace OpenWordPad {

RulerWidget::RulerWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(24);
    setMouseTracking(true);
}

void RulerWidget::setUnit(UnitType unit) {
    m_unit = unit;
    update();
}

void RulerWidget::setPageWidth(double inches) {
    m_pageWidth = inches;
    update();
}

void RulerWidget::setMargins(double leftInches, double rightInches) {
    m_leftMargin = leftInches;
    m_rightMargin = rightInches;
    update();
}

void RulerWidget::setIndents(double firstLineInches, double leftIndentInches, double rightIndentInches) {
    m_firstLineIndent = firstLineInches;
    m_leftIndent = leftIndentInches;
    m_rightIndent = rightIndentInches;
    update();
}

void RulerWidget::setOriginX(int originX) {
    m_originX = originX;
    update();
}

double RulerWidget::inchesToPixelX(double inches) const {
    return m_originX + (inches * Units::DPI);
}

double RulerWidget::pixelXToInches(double px) const {
    return (px - m_originX) / Units::DPI;
}

void RulerWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();

    // Auto-center page on ruler
    int pageWidthPx = static_cast<int>(m_pageWidth * Units::DPI);
    m_originX = std::max(20, (w - pageWidthPx) / 2);

    // Background outside ruler (classic WordPad blue-grey)
    p.fillRect(0, 0, w, h, QColor(0xd7, 0xe4, 0xf2));

    // Page white printable strip
    double pageLeftPx = inchesToPixelX(0);
    double printLeftPx = inchesToPixelX(m_leftMargin);
    double printRightPx = inchesToPixelX(m_pageWidth - m_rightMargin);

    // Unprintable margin areas (soft grey-blue)
    p.fillRect(QRectF(pageLeftPx, 1, m_leftMargin * Units::DPI, h - 2), QColor(0xec, 0xf2, 0xf9));
    p.fillRect(QRectF(printRightPx, 1, m_rightMargin * Units::DPI, h - 2), QColor(0xec, 0xf2, 0xf9));

    // Printable area (pure white)
    p.fillRect(QRectF(printLeftPx, 1, (m_pageWidth - m_leftMargin - m_rightMargin) * Units::DPI, h - 2), Qt::white);

    // Top and Bottom border lines
    p.setPen(QColor(0xb0, 0xc4, 0xde));
    p.drawLine(0, 0, w, 0);
    p.drawLine(0, h - 1, w, h - 1);

    // Ticks and numbers
    p.setFont(QFont(QStringLiteral("Segoe UI"), 7));
    p.setPen(QColor(0x33, 0x33, 0x33));

    double unitStepInches = (m_unit == UnitType::Centimeters) ? (1.0 / 2.54) : 1.0;

    // 1. Left Margin Countdown (e.g. 3, 2, 1)
    int leftMarginUnits = static_cast<int>(m_leftMargin / unitStepInches);
    for (int m = leftMarginUnits; m >= 1; --m) {
        double unitInches = m_leftMargin - (m * unitStepInches);
        double px = inchesToPixelX(unitInches);
        if (px >= pageLeftPx) {
            p.drawText(QRectF(px - 10, 2, 20, 10), Qt::AlignHCenter | Qt::AlignTop, QString::number(m));
            // Dot at half
            double dotPx = inchesToPixelX(unitInches + (unitStepInches / 2.0));
            p.fillRect(QRectF(dotPx - 0.5, 6, 1.5, 1.5), QColor(0x66, 0x66, 0x66));
        }
    }

    // 2. Printable Area (1, 2, 3 ... 17)
    double printableInches = m_pageWidth - m_leftMargin - m_rightMargin;
    int printUnits = static_cast<int>(printableInches / unitStepInches);

    for (int u = 1; u <= printUnits; ++u) {
        double unitInches = m_leftMargin + (u * unitStepInches);
        double px = inchesToPixelX(unitInches);

        if (px <= printRightPx) {
            // Number
            p.drawText(QRectF(px - 10, 2, 20, 10), Qt::AlignHCenter | Qt::AlignTop, QString::number(u));

            // Dot at half cm / inch
            double dotPx = inchesToPixelX(unitInches - (unitStepInches / 2.0));
            p.fillRect(QRectF(dotPx - 0.5, 6, 1.5, 1.5), QColor(0x66, 0x66, 0x66));
        }
    }

    // Draggable Markers (Hourglass top, Triangle bottom, Right triangle)
    // 1. First Line Indent (Top Hourglass / inverted triangle)
    double firstLineX = inchesToPixelX(m_leftMargin + m_firstLineIndent);
    QPolygonF topTriangle;
    topTriangle << QPointF(firstLineX - 4, 1)
                << QPointF(firstLineX + 4, 1)
                << QPointF(firstLineX, 7);
    p.setBrush(QColor(0xff, 0xff, 0xff));
    p.setPen(QColor(0x00, 0x5a, 0x9e));
    p.drawPolygon(topTriangle);

    // 2. Left Indent (Bottom Triangle pointing up + square at bottom)
    double leftIndentX = inchesToPixelX(m_leftMargin + m_leftIndent);
    QPolygonF bottomTriangle;
    bottomTriangle << QPointF(leftIndentX - 4, h - 6)
                   << QPointF(leftIndentX + 4, h - 6)
                   << QPointF(leftIndentX, h - 12);
    p.drawPolygon(bottomTriangle);
    p.drawRect(QRectF(leftIndentX - 4, h - 6, 8, 4));

    // 3. Right Indent (Bottom Triangle pointing up on right side)
    double rightIndentX = inchesToPixelX(m_pageWidth - m_rightMargin - m_rightIndent);
    QPolygonF rightTriangle;
    rightTriangle << QPointF(rightIndentX - 4, h - 1)
                  << QPointF(rightIndentX + 4, h - 1)
                  << QPointF(rightIndentX, h - 8);
    p.drawPolygon(rightTriangle);
}

void RulerWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    int pageWidthPx = static_cast<int>(m_pageWidth * Units::DPI);
    m_originX = std::max(20, (width() - pageWidthPx) / 2);

    double x = event->position().x();
    double y = event->position().y();

    double firstLineX = inchesToPixelX(m_leftMargin + m_firstLineIndent);
    double leftIndentX = inchesToPixelX(m_leftMargin + m_leftIndent);
    double rightIndentX = inchesToPixelX(m_pageWidth - m_rightMargin - m_rightIndent);

    if (std::abs(x - firstLineX) < 6 && y < 10) {
        m_activeDrag = DragTarget::FirstLineIndent;
    } else if (std::abs(x - leftIndentX) < 6 && y >= 10) {
        m_activeDrag = DragTarget::LeftIndent;
    } else if (std::abs(x - rightIndentX) < 6) {
        m_activeDrag = DragTarget::RightIndent;
    }
}

void RulerWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_activeDrag == DragTarget::None) return;

    double inches = pixelXToInches(event->position().x());

    if (m_activeDrag == DragTarget::FirstLineIndent) {
        m_firstLineIndent = inches - m_leftMargin;
        emit firstLineIndentChanged(m_firstLineIndent);
    } else if (m_activeDrag == DragTarget::LeftIndent) {
        m_leftIndent = std::max(0.0, inches - m_leftMargin);
        emit leftIndentChanged(m_leftIndent);
    } else if (m_activeDrag == DragTarget::RightIndent) {
        m_rightIndent = std::max(0.0, (m_pageWidth - m_rightMargin) - inches);
        emit rightIndentChanged(m_rightIndent);
    }
    update();
}

void RulerWidget::mouseReleaseEvent(QMouseEvent *) {
    m_activeDrag = DragTarget::None;
}

} // namespace OpenWordPad
