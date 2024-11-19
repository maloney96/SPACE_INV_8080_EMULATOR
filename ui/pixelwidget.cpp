#include "pixelwidget.h"
#include <QPainter>
#include <QDebug>

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent),
    image(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB32) {  // Initialize the image size
    image.fill(Qt::black);  // Start with a black background
}

PixelWidget::~PixelWidget() {
}

void PixelWidget::updatePixelData() {
    if (!videoEmulator) return;

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            float pixelState = VideoEmulator_getPixel(videoEmulator, x, y);

            // Map pixelState black or white
            QRgb color = (pixelState == 1) ? qRgb(255, 255, 255) : qRgb(0, 0, 0);
            image.setPixel(x, y, color);
        }
    }
}

void PixelWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    // Scale the image to fit the widget
    painter.drawImage(rect(), image);
}

void PixelWidget::renderFrame(VideoEmulator *emulator) {
    videoEmulator = emulator;
    updatePixelData();  // Update the image based on the emulator's video memory
    update();           // Trigger a repaint
}
