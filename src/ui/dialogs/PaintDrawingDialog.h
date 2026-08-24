#pragma once

#include <QDialog>
#include <QImage>
#include <QColor>
#include <QToolButton>
#include <QSpinBox>

namespace OpenWordPad {

class PaintDrawingDialog : public QDialog {
    Q_OBJECT

public:
    explicit PaintDrawingDialog(QWidget *parent = nullptr);

    QImage drawnImage() const { return m_canvasImage; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void chooseColor();
    void clearCanvas();

private:
    enum class Tool {
        Pen,
        Brush,
        Line,
        Rectangle,
        Ellipse,
        Eraser
    };

    Tool m_tool = Tool::Pen;
    QColor m_color = Qt::black;
    int m_penWidth = 3;
    QImage m_canvasImage;
    QPoint m_lastPoint;
    bool m_drawing = false;
    QRect m_canvasRect;
};

} // namespace OpenWordPad
