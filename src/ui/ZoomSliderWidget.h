#pragma once

#include <QWidget>
#include <QSlider>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>

namespace OpenWordPad {

class ZoomSliderWidget : public QWidget {
    Q_OBJECT

public:
    explicit ZoomSliderWidget(QWidget *parent = nullptr);

    int zoomPercentage() const { return m_slider->value(); }
    void setZoomPercentage(int percent);

signals:
    void zoomChanged(int percent);

private slots:
    void onSliderValueChanged(int value);
    void onZoomInClicked();
    void onZoomOutClicked();
    void showZoomMenu();

private:
    QSlider *m_slider;
    QToolButton *m_btnZoomOut;
    QToolButton *m_btnZoomIn;
    QLabel *m_lblPercent;
};

} // namespace OpenWordPad
