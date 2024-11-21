#include "pixelwidget.h"
#include <QPainter>
#include <QDebug>

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent),
    image(VideoEmulator::SCREEN_WIDTH, VideoEmulator::SCREEN_HEIGHT, QImage::Format_RGB32) {
    image.fill(Qt::black);  // Initialize with black
}

PixelWidget::~PixelWidget() {}

void PixelWidget::renderFrame(const VideoEmulator& emulator) {
    // Update the QImage with the current frame from the emulator
    updatePixelData(emulator);
    update(); // Trigger a repaint
}

void PixelWidget::updatePixelData(const VideoEmulator& emulator) {
    const uint8_t* memory = emulator.getFrame();
    if (!memory) {
        qDebug() << "VideoEmulator frame memory is null!";
        return;
    }

    // qDebug() << "Updating pixel data...";



    for (int y = 0; y < VideoEmulator::SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < VideoEmulator::SCREEN_WIDTH; ++x) {
            int pixelState = emulator.getPixel(x, y);

            // Debug the pixel extraction
            /*
            if (x < 10 && y < 10) { // Only debug the top-left corner for simplicity
                qDebug() << "Pixel (" << x << "," << y << ") state:" << pixelState;
            }
            */

            QRgb color = (pixelState == 1) ? qRgb(255, 255, 255) : qRgb(0, 0, 0);
            image.setPixel(x, y, color);

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
