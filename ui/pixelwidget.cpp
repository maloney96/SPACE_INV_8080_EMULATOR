#include "pixelwidget.h"
#include <QPainter>
#include <QDebug>

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent),
    image(OutputManager::SCREEN_WIDTH, OutputManager::SCREEN_HEIGHT, QImage::Format_Mono) {
    image.fill(Qt::black);  // Initialize with black
}

PixelWidget::~PixelWidget() {}

// void PixelWidget::renderFrame() {
//     // Update the QImage with the current frame from the singleton VideoEmulator
//     updatePixelData();
//     update(); // Trigger a repaint
// }

void PixelWidget::updatePixelData() {
    const uint8_t* memory = OutputManager::getInstance()->getFrame();
    if (!memory) {
        qDebug() << "VideoEmulator frame memory is null!";
        return;
    }

    for (int y = 0; y < OutputManager::SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < OutputManager::SCREEN_WIDTH; ++x) {
            int pixelState = OutputManager::getInstance()->getPixel(x, y);
            image.setPixel(x, y, (pixelState == 1) ? 1 : 0);
        }
    }
    update();
}

void PixelWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(rect(), image);
}
