#ifndef VIDEOEMULATOR_H
#define VIDEOEMULATOR_H

#include <cstdint>

class VideoEmulator {
public:
    static constexpr int SCREEN_WIDTH = 224;
    static constexpr int SCREEN_HEIGHT = 256;
    static constexpr int FRAME_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 8;

    explicit VideoEmulator(const uint8_t* emulatorMemory);
    ~VideoEmulator() = default;

    const uint8_t* getFrame() const;
    int getPixel(int x, int y) const;

private:
    const uint8_t* memory; // Read-only pointer to the video memory managed by EmulatorWrapper
};

#endif // VIDEOEMULATOR_H
