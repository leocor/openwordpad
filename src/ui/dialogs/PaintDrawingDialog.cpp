#include "PaintDrawingDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QColorDialog>
#include <QLabel>
#include <QButtonGroup>

namespace OpenWordPad {

PaintDrawingDialog::PaintDrawingDialog(QWidget *parent)
    : QDialog(parent), m_canvasImage(400, 300, QImage::Format_ARGB32_Premultiplied)
{
    setWindowTitle(QStringLiteral("Paint Drawing"));
    setFixedSize(520, 420);

    m_canvasImage.fill(Qt::white);
    m_canvasRect = QRect(10, 50, 400, 300);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Toolbar
    auto toolLayout = new QHBoxLayout();
    auto btnGroup = new QButtonGroup(this);

    auto makeToolBtn = [&](const QString &name, Tool tool, bool checked = false) {
        auto btn = new QToolButton(this);
        btn->setText(name);
        btn->setCheckable(true);
        btn->setChecked(checked);
        btnGroup->addButton(btn);
        toolLayout->addWidget(btn);
        connect(btn, &QToolButton::clicked, this, [this, tool]() { m_tool = tool; });
        return btn;
    };

    makeToolBtn(QStringLiteral("Pen"), Tool::Pen, true);
    makeToolBtn(QStringLiteral("Brush"), Tool::Brush);
    makeToolBtn(QStringLiteral("Line"), Tool::Line);
    makeToolBtn(QStringLiteral("Rect"), Tool::Rectangle);
    makeToolBtn(QStringLiteral("Circle"), Tool::Ellipse);
    makeToolBtn(QStringLiteral("Eraser"), Tool::Eraser);

    toolLayout->addSpacing(10);
    toolLayout->addWidget(new QLabel(QStringLiteral("Size:"), this));
    auto spinSize = new QSpinBox(this);
    spinSize->setRange(1, 30);
    spinSize->setValue(3);
    connect(spinSize, &QSpinBox::valueChanged, this, [this](int v) { m_penWidth = v; });
    toolLayout->addWidget(spinSize);

    auto btnColor = new QPushButton(QStringLiteral("Color"), this);
    connect(btnColor, &QPushButton::clicked, this, &PaintDrawingDialog::chooseColor);
    toolLayout->addWidget(btnColor);

    auto btnClear = new QPushButton(QStringLiteral("Clear"), this);
    connect(btnClear, &QPushButton::clicked, this, &PaintDrawingDialog::clearCanvas);
    toolLayout->addWidget(btnClear);

    toolLayout->addStretch();
    mainLayout->addLayout(toolLayout);

    mainLayout->addSpacing(310); // Spacing for canvas region

    // Bottom action buttons
    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto btnInsert = new QPushButton(QStringLiteral("Insert into Document"), this);
    btnInsert->setDefault(true);
    auto btnCancel = new QPushButton(QStringLiteral("Cancel"), this);
    connect(btnInsert, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnInsert);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);
}

void PaintDrawingDialog::chooseColor() {
    QColor c = QColorDialog::getColor(m_color, this, QStringLiteral("Select Drawing Color"));
    if (c.isValid()) {
        m_color = c;
    }
}

void PaintDrawingDialog::clearCanvas() {
    m_canvasImage.fill(Qt::white);
    update();
}

void PaintDrawingDialog::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(m_canvasRect, Qt::white);
    p.drawImage(m_canvasRect.topLeft(), m_canvasImage);
    p.setPen(QColor(0x99, 0x99, 0x99));
    p.drawRect(m_canvasRect.adjusted(-1, -1, 0, 0));
}

void PaintDrawingDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_canvasRect.contains(event->pos())) {
        m_lastPoint = event->pos() - m_canvasRect.topLeft();
        m_drawing = true;
    }
}

void PaintDrawingDialog::mouseMoveEvent(QMouseEvent *event) {
    if (m_drawing && m_canvasRect.contains(event->pos())) {
        QPoint curPoint = event->pos() - m_canvasRect.topLeft();
        QPainter p(&m_canvasImage);
        p.setRenderHint(QPainter::Antialiasing, true);

        if (m_tool == Tool::Eraser) {
            p.setPen(QPen(Qt::white, m_penWidth * 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawLine(m_lastPoint, curPoint);
            m_lastPoint = curPoint;
        } else if (m_tool == Tool::Pen || m_tool == Tool::Brush) {
            int w = (m_tool == Tool::Brush) ? m_penWidth * 2 : m_penWidth;
            p.setPen(QPen(m_color, w, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawLine(m_lastPoint, curPoint);
            m_lastPoint = curPoint;
        }
        update();
    }
}

void PaintDrawingDialog::mouseReleaseEvent(QMouseEvent *event) {
    if (m_drawing && event->button() == Qt::LeftButton) {
        QPoint curPoint = event->pos() - m_canvasRect.topLeft();
        QPainter p(&m_canvasImage);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(QPen(m_color, m_penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        if (m_tool == Tool::Line) {
            p.drawLine(m_lastPoint, curPoint);
        } else if (m_tool == Tool::Rectangle) {
            p.drawRect(QRect(m_lastPoint, curPoint).normalized());
        } else if (m_tool == Tool::Ellipse) {
            p.drawEllipse(QRect(m_lastPoint, curPoint).normalized());
        }
        m_drawing = false;
        update();
    }
}

} // namespace OpenWordPad
