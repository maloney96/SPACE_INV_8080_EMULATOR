#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QWidget>
#include <QImage>

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
     */
    void renderFrame();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image; ///< The QImage used for rendering.

    /**
     * @brief Updates the pixel data in the QImage based on emulator memory.
     */
    void updatePixelData();
};

#endif // PIXELWIDGET_H
