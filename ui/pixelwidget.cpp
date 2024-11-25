#include "pixelwidget.h"
#include <QPainter>
#include <QDebug>
#include "../outputmanager/videoemulator.h"

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent),
    image(VideoEmulator::SCREEN_WIDTH, VideoEmulator::SCREEN_HEIGHT, QImage::Format_Mono) {
    image.fill(Qt::black);  // Initialize with black
}

PixelWidget::~PixelWidget() {}

void PixelWidget::renderFrame() {
    // Update the QImage with the current frame from the singleton VideoEmulator
    updatePixelData();
    update(); // Trigger a repaint
}

void PixelWidget::updatePixelData() {
    const uint8_t* memory = VideoEmulator::getInstance()->getFrame();
    if (!memory) {
        qDebug() << "VideoEmulator frame memory is null!";
        return;
    }

    for (int y = 0; y < VideoEmulator::SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < VideoEmulator::SCREEN_WIDTH; ++x) {
            int pixelState = VideoEmulator::getInstance()->getPixel(x, y);
            image.setPixel(x, y, (pixelState == 1) ? 1 : 0);
        }
    }
}

void PixelWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Scale the image to fit the widget
    painter.drawImage(rect(), image);
}
