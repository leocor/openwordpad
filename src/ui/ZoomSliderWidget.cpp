#include "ZoomSliderWidget.h"
#include <QMenu>
#include <QAction>
#include <QStyle>

namespace OpenWordPad {

ZoomSliderWidget::ZoomSliderWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 0, 8, 0);
    layout->setSpacing(4);

    m_btnZoomOut = new QToolButton(this);
    m_btnZoomOut->setText(QStringLiteral("-"));
    m_btnZoomOut->setAutoRaise(true);
    m_btnZoomOut->setToolTip(QStringLiteral("Zoom Out (Ctrl+-)"));
    m_btnZoomOut->setFixedSize(18, 18);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(10, 500);
    m_slider->setValue(100);
    m_slider->setFixedWidth(100);
    m_slider->setToolTip(QStringLiteral("Zoom Slider"));

    m_btnZoomIn = new QToolButton(this);
    m_btnZoomIn->setText(QStringLiteral("+"));
    m_btnZoomIn->setAutoRaise(true);
    m_btnZoomIn->setToolTip(QStringLiteral("Zoom In (Ctrl++)"));
    m_btnZoomIn->setFixedSize(18, 18);

    m_lblPercent = new QLabel(QStringLiteral("100%"), this);
    m_lblPercent->setFixedWidth(40);
    m_lblPercent->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_lblPercent->setCursor(Qt::PointingHandCursor);
    m_lblPercent->setToolTip(QStringLiteral("Click to choose zoom level"));

    layout->addWidget(m_btnZoomOut);
    layout->addWidget(m_slider);
    layout->addWidget(m_btnZoomIn);
    layout->addWidget(m_lblPercent);

    connect(m_slider, &QSlider::valueChanged, this, &ZoomSliderWidget::onSliderValueChanged);
    connect(m_btnZoomOut, &QToolButton::clicked, this, &ZoomSliderWidget::onZoomOutClicked);
    connect(m_btnZoomIn, &QToolButton::clicked, this, &ZoomSliderWidget::onZoomInClicked);

    // Zoom percent menu
    m_lblPercent->installEventFilter(this);
}

void ZoomSliderWidget::setZoomPercentage(int percent) {
    percent = qBound(10, percent, 500);
    m_slider->setValue(percent);
    m_lblPercent->setText(QString::number(percent) + QStringLiteral("%"));
    emit zoomChanged(percent);
}

void ZoomSliderWidget::onSliderValueChanged(int value) {
    m_lblPercent->setText(QString::number(value) + QStringLiteral("%"));
    emit zoomChanged(value);
}

void ZoomSliderWidget::onZoomInClicked() {
    int current = m_slider->value();
    int next = (current / 10 + 1) * 10;
    m_slider->setValue(next);
}

void ZoomSliderWidget::onZoomOutClicked() {
    int current = m_slider->value();
    int prev = (current % 10 == 0) ? current - 10 : (current / 10) * 10;
    m_slider->setValue(prev);
}

void ZoomSliderWidget::showZoomMenu() {
    QMenu menu(this);
    QList<int> levels = { 50, 75, 100, 125, 150, 200, 300, 400 };
    for (int lvl : levels) {
        QAction *act = menu.addAction(QString::number(lvl) + QStringLiteral("%"));
        connect(act, &QAction::triggered, this, [this, lvl]() {
            m_slider->setValue(lvl);
        });
    }
    menu.exec(mapToGlobal(m_lblPercent->pos()));
}

} // namespace OpenWordPad
