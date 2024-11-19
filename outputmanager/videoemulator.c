#include "../outputmanager/videoemulator.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

void VideoEmulator_init(VideoEmulator *emulator) {
    srand((unsigned)time(NULL));
    memset(emulator->memory, 0, FRAME_SIZE); // Initialize to black
}

void VideoEmulator_generateFrame(VideoEmulator *emulator) {
    for (int i = 0; i < FRAME_SIZE; i++) {
        emulator->memory[i] = rand() % 256; // Each byte is randomized (8 random pixels)
    }
}

// Returns a pointer to the memory frame for other files to access
unsigned char* VideoEmulator_getFrame(VideoEmulator *emulator) {
    return emulator->memory;
}

// Get the state of a specific pixel (x, y): returns 1 if "on" or 0 if "off"
int VideoEmulator_getPixel(const VideoEmulator *emulator, int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return 0;  // Out of bounds; treat as "off"
    }
    int byte_index = (y * SCREEN_WIDTH + x) / 8;
    int bit_index = x % 8;
    return (emulator->memory[byte_index] >> (7 - bit_index)) & 1;
}
