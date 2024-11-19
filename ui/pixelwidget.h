#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QWidget>
#include <QImage>
#include "../outputmanager/videoemulator.h"

/**
 * @brief PixelWidget is responsible for rendering the video frames.
 *
 * This widget renders a frame of pixels based on the data retrieved
 * from the emulator's video memory.
 */
class PixelWidget : public QWidget {
    Q_OBJECT

public:
    explicit PixelWidget(QWidget *parent = nullptr);
    ~PixelWidget();

    /**
     * @brief Updates the widget with a new frame from the emulator.
     *
     * @param emulator A reference to the VideoEmulator instance.
     */
    void renderFrame(const VideoEmulator& emulator);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image; ///< The QImage used for rendering.

    /**
     * @brief Updates the pixel data in the QImage based on emulator memory.
     *
     * @param emulator A reference to the VideoEmulator instance.
     */
    void updatePixelData(const VideoEmulator& emulator);
};

#endif // PIXELWIDGET_H
