#include "pixelwidget.h"
#include "../outputmanager/outputManager.h"
#include <QPainter>
#include <QDebug>

const int PixelWidget::frameSz = OutputManager::FRAME_SIZE;
const int PixelWidget::frameHt = OutputManager::SCREEN_HEIGHT;
const int PixelWidget::frameWd = OutputManager::SCREEN_WIDTH;

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent),
    image(frameWd, frameHt, QImage::Format_Mono)
{
    image.fill(false);  // Initialize with black
    previous.resize(frameSz, 0);
}

PixelWidget::~PixelWidget() {}

void PixelWidget::updatePixelData() {
    current = OutputManager::getInstance()->getFrame();
    for (int y = 0; y < frameHt; ++y) {
        for (int x = 0; x < frameWd; ++x) {
            int pixelState = getPixel(x, y);
            image.setPixel(x, y, pixelState);
        }
    }
    update(); // Trigger UI refresh
}

void PixelWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(rect(), image);
}

int PixelWidget::getPixel(int x, int y) const {
    int byteIndex = ((x + 1) * frameHt / 8) - y / 8;
    int bitIndex = 7 - (y % 8);
    return (current[byteIndex] >> bitIndex) & 1;
}
