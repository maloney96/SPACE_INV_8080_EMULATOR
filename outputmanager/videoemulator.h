#ifndef VIDEOEMULATOR_H
#define VIDEOEMULATOR_H

#include <cstdint>
#include <QDebug>

/**
 * @brief The VideoEmulator class provides a read-only interface to the video memory
 * for rendering and pixel management in the Space Invaders emulator.
 */
class VideoEmulator {
public:
    static constexpr int SCREEN_WIDTH = 224;
    static constexpr int SCREEN_HEIGHT = 256;
    static constexpr int FRAME_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 8;

    /**
     * @brief Gets the singleton instance of the VideoEmulator.
     *
     * @param emulatorMemory Pointer to the emulator's video memory. This must
     *        be passed during the first call to initialize the singleton.
     * @return A pointer to the singleton VideoEmulator instance.
     */
    static VideoEmulator* getInstance(const uint8_t* emulatorMemory = nullptr);

    // Delete copy constructor and assignment operator
    VideoEmulator(const VideoEmulator&) = delete;
    VideoEmulator& operator=(const VideoEmulator&) = delete;

    /**
     * @brief Accesses the raw video frame memory.
     * @return A read-only pointer to the video memory.
     */
    const uint8_t* getFrame() const;

    /**
     * @brief Gets the state of a pixel at a specific (x, y) coordinate.
     * @param x The x-coordinate of the pixel.
     * @param y The y-coordinate of the pixel.
     * @return 1 if the pixel is "on", 0 if "off".
     */
    int getPixel(int x, int y) const;

private:
    // Private constructor for the singleton
    explicit VideoEmulator(const uint8_t* emulatorMemory);

    // Destructor
    ~VideoEmulator() = default;

    static VideoEmulator* instance; ///< Singleton instance of VideoEmulator
    const uint8_t* memory; ///< Pointer to the read-only video memory
};

#endif // VIDEOEMULATOR_H
