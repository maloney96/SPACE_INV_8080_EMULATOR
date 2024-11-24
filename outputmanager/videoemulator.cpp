#include "videoemulator.h"

// Static member initialization
VideoEmulator* VideoEmulator::instance = nullptr;

/**
 * @brief Returns the singleton instance of the VideoEmulator.
 *
 * If the instance is not initialized, it is created using the provided
 * emulator memory. Subsequent calls will ignore the parameter.
 *
 * @param emulatorMemory Pointer to the emulator's video memory (required on first call).
 * @return Pointer to the VideoEmulator singleton.
 */
VideoEmulator* VideoEmulator::getInstance(const uint8_t* emulatorMemory) {
    if (!instance) {
        if (!emulatorMemory) {
            qCritical() << "Error: VideoEmulator instance requested without initializing memory!";
            return nullptr;
        }
        instance = new VideoEmulator(emulatorMemory);
        qDebug() << "VideoEmulator instance created.";
    }
    return instance;
}


/**
 * @brief Private constructor for the VideoEmulator class.
 * @param emulatorMemory Pointer to the emulator's video memory.
 */
VideoEmulator::VideoEmulator(const uint8_t* emulatorMemory)
    : memory(emulatorMemory) {
    qDebug() << "VideoEmulator initialized with memory address:" << static_cast<const void*>(memory);
}

/**
 * @brief Gets the raw video memory frame.
 * @return Pointer to the raw video memory.
 */
const uint8_t* VideoEmulator::getFrame() const {
    return memory;
}

/**
 * @brief Gets the state of a pixel at a specific (x, y) coordinate.
 *
 * Handles bounds checking to ensure valid pixel coordinates.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @return 1 if the pixel is "on", 0 if "off".
 */
int VideoEmulator::getPixel(int x, int y) const {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return 0; // Out of bounds; treat as "off"
    }

    int byteIndex = ((x + 1) * SCREEN_HEIGHT / 8) - y / 8;
    int bitIndex = 7 - (y % 8);
    return (memory[byteIndex] >> bitIndex) & 1;
}
