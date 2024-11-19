#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QPointF>
#include "../outputmanager/videoemulator.h"

class PixelWidget : public QWidget {
    Q_OBJECT

public:
    explicit PixelWidget(QWidget *parent = nullptr);
    ~PixelWidget();

    void renderFrame(VideoEmulator *emulator);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    VideoEmulator *videoEmulator = nullptr;

    void updatePixelData();
};

#endif // PIXELWIDGET_H
