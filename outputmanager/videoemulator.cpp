#include "videoemulator.h"

// Constructor takes a pointer to the emulator's video memory
VideoEmulator::VideoEmulator(const uint8_t* emulatorMemory)
    : memory(emulatorMemory) {}

// Get a read-only pointer to the video frame memory
const uint8_t* VideoEmulator::getFrame() const {
    return memory;
}

// Get the state of a specific pixel (x, y)
int VideoEmulator::getPixel(int x, int y) const {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return 0; // Out of bounds; treat as "off"
    }
    int byteIndex = ((x + 1) * SCREEN_HEIGHT / 8) - y / 8;
    int bitIndex = 7- (y % 8);
    return (memory[byteIndex] >> (bitIndex)) & 1;
}
