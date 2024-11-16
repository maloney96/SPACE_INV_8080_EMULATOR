#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include <QVector>
#include <stdint.h>
#include "../emulator/emulatorWrapper.h"

class OutputManager : public QObject {
    Q_OBJECT

#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define FRAME_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

public:
    // Singleton access
    static OutputManager& getInstance(EmulatorWrapper *emulatorWrapper = nullptr, QObject *parent = nullptr);

    // Fetch video memory for rendering
    QVector<uint8_t> fetchFrame();

private:
    // Private constructor for singleton
    explicit OutputManager(EmulatorWrapper *emulatorWrapper, QObject *parent = nullptr);

    // Delete copy constructor and assignment operator
    OutputManager(const OutputManager&) = delete;
    OutputManager& operator=(const OutputManager&) = delete;

    EmulatorWrapper *emulatorWrapper; // Access to emulator memory via EmulatorWrapper

    static OutputManager *instance; // Singleton instance
};

#endif // OUTPUTMANAGER_H
