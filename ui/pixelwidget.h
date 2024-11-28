#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QWidget>
#include <QImage>

/**
 * @brief PixelWidget is responsible for rendering the video frames.
 *
 * This widget renders a frame of pixels based on the binary data
 * retrieved from the emulator's video memory.
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

public slots:
    /**
     * @brief Updates the pixel data in the QImage based on emulator memory.
     */
    void updatePixelData();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image; ///< The QImage used for rendering.
    const uint8_t* current;
    std::vector<uint8_t> previous; ///< Buffer to store the previous frame.
    int getPixel(int x, int y) const;

    static const int frameSz;
    static const int frameHt;
    static const int frameWd;

};

#endif // PIXELWIDGET_H
