#include "outputManager.h"
#include "../emulator/io_bits.h"
#include <QDebug>

OutputManager* OutputManager::instance = nullptr;

OutputManager* OutputManager::getInstance() {
    if (!instance) {
        instance = new OutputManager();
    }
    return instance;
}

void OutputManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
        qDebug() << "OutputManager instance destroyed";
    }
}

OutputManager::OutputManager(QObject* parent)
    : QObject(parent), videoMemory(nullptr), audioMixer(nullptr) {
    // Create a dedicated thread for the timer
    timerThread = new QThread(this);

    // Create the timer and move it to the thread
    frameTimer = new QTimer();
    frameTimer->moveToThread(timerThread);

    // Connect the timer's timeout signal to emit frameReady
    connect(frameTimer, &QTimer::timeout, this, &OutputManager::frameReady);

    // Start the timer when the thread starts
    connect(timerThread, &QThread::started, frameTimer, [=]() {
        frameTimer->start(16); // Roughly 60 FPS (16ms interval)
    });

    // Stop the timer and clean up when the thread finishes
    connect(timerThread, &QThread::finished, frameTimer, &QTimer::stop);
}

OutputManager::~OutputManager() {
    stopVideo();

    // Clean up the timer thread
    timerThread->quit();
    timerThread->wait();
    delete frameTimer;
}

void OutputManager::initializeVideo() {
    videoMemory = EmulatorWrapper::getInstance().getVideoMemory();
    if (!videoMemory) {
        qCritical() << "Error: Video memory is not initialized!";
    } else {
        qDebug() << "Video memory initialized at address:" << static_cast<const void*>(videoMemory);
    }
}

void OutputManager::startVideo() {
    if (!timerThread->isRunning()) {
        timerThread->start();
        qDebug() << "Video thread started.";
    }
}

const uint8_t* OutputManager::getFrame() const {
    if (!videoMemory) {
        qCritical() << "Error: Video memory not initialized!";
        return nullptr;
    }
    return videoMemory;
}

int OutputManager::getPixel(int x, int y) const {
    if (!videoMemory) {
        qCritical() << "Error: Video memory not initialized!";
        return 0;
    }
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return 0; // Out of bounds; treat as "off"
    }

    int byteIndex = ((x + 1) * SCREEN_HEIGHT / 8) - y / 8;
    int bitIndex = 7 - (y % 8);
    return (videoMemory[byteIndex] >> bitIndex) & 1;
}


void OutputManager::stopVideo() {
    if (timerThread->isRunning()) {
        timerThread->quit();
        timerThread->wait(); // Ensure thread exits cleanly
        qDebug() << "Video thread stopped.";
    }
}

// Audio management methods
void OutputManager::setAudioMixer(AudioMixer* mixer) {
    audioMixer = mixer;
    qDebug() << "AudioMixer set in OutputManager";
}

void OutputManager::playSoundEffect(const QString& filePath) {
    if (audioMixer) {
        audioMixer->playSoundEffect(filePath);
    } else {
        qWarning() << "AudioMixer not set. Cannot play sound effect.";
    }
}

void OutputManager::startBackgroundMusic() {
    if (audioMixer) {
        audioMixer->startMenuMusic();
    } else {
        qWarning() << "AudioMixer not set. Cannot start background music.";
    }
}

void OutputManager::stopBackgroundMusic() {
    if (audioMixer) {
        audioMixer->stopMenuMusic();
    } else {
        qWarning() << "AudioMixer not set. Cannot stop background music.";
    }
}

void OutputManager::handleSoundUpdates(uint8_t port_num, uint8_t port_value) {
    // Port 3 sounds
    if (port_num == 3){
        if (port_value & UFO) {
        AudioMixer::getInstance()->playSoundEffect("ufo_lowpitch.wav");
        }
        if (port_value & SHOTS) {
        AudioMixer::getInstance()->playSoundEffect("shoot.wav");
        }
        if (port_value & PLAYER_DIE) {
        AudioMixer::getInstance()->playSoundEffect("explosion.wav");
        }
        if (port_value & INVADER_DIE) {
        AudioMixer::getInstance()->playSoundEffect("invaderkilled.wav");
        }
    }

    // Port 5 sounds
    if (port_num == 5){
        if (port_value & FLEET1) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader1.wav");
        }
        if (port_value & FLEET2) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader2.wav");
        }
        if (port_value & FLEET3) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader3.wav");
        }
        if (port_value & FLEET4) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader4.wav");
        }
        if (port_value & UFO_HIT) {
        AudioMixer::getInstance()->playSoundEffect("ufo_highpitch.wav");
        }
    }

}
