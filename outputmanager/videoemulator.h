#ifndef VIDEO_MEMORY_EMULATOR_H
#define VIDEO_MEMORY_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define FRAME_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8) // Total bytes for 224x256 resolution

typedef struct {
    unsigned char memory[FRAME_SIZE];
} VideoEmulator;

void VideoEmulator_init(VideoEmulator *emulator);
void VideoEmulator_generateFrame(VideoEmulator *emulator);
void VideoEmulator_setPixel(VideoEmulator *emulator, int x, int y, int state);
int VideoEmulator_getPixel(const VideoEmulator *emulator, int x, int y);
unsigned char* VideoEmulator_getFrame(VideoEmulator *emulator);

#ifdef __cplusplus
}
#endif

#endif // VIDEO_MEMORY_EMULATOR_H
