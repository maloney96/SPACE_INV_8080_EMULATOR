#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include "audiomixer.h"
#include "../emulator/emulatorWrapper.h"

class OutputManager : public QObject {
    Q_OBJECT

public:
    static constexpr int SCREEN_WIDTH = 224;
    static constexpr int SCREEN_HEIGHT = 256;
    static constexpr int FRAME_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 8;

    static OutputManager* getInstance();
    static void destroyInstance();

    // Video-related methods
    void initializeVideo();
    const uint8_t* getFrame() const; // Provide access to raw frame data
    int getPixel(int x, int y) const; // Access pixel state
    void startVideo();
    void stopVideo();

    // Audio-related methods
    void setAudioMixer(AudioMixer* mixer);
    void playSoundEffect(const QString& filePath);
    void startBackgroundMusic();
    void stopBackgroundMusic();

    void handleSoundUpdates(uint8_t port_num, uint8_t port_val);

signals:
    void frameReady(); // Forwarded signal for frame updates

private:
    explicit OutputManager(QObject* parent = nullptr);
    ~OutputManager();

    static OutputManager* instance;

    QTimer* frameTimer;    ///< Timer to emit frameReady
    QThread* timerThread;  ///< Thread for the timer
    const uint8_t* videoMemory; ///< Pointer to video memory
    AudioMixer* audioMixer;     ///< Pointer to the AudioMixer instance
};

#endif // OUTPUTMANAGER_H
