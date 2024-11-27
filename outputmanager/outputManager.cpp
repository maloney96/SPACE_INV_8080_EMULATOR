#include "outputManager.h"
#include "../emulator/io_bits.h"
#include "../emulator/emulatorWrapper.h"
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

void OutputManager::playSoundEffect(const QString& filePath, bool loop = false) {
    if (audioMixer) {
        audioMixer->playSoundEffect(filePath, loop);
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

void OutputManager::handleSoundUpdates(uint8_t port_num, uint8_t old_value, uint8_t new_value) {
    // Don't bother processing if nothing has changed
    if (old_value == new_value){
        return;
    }

    uint8_t new_bits = ~old_value & new_value;

    // Port 3 sounds
    if (port_num == 3){

        //Special Handling for UFO
        //Loop it if a new bit arrives
        //Turn it off if the newly arrived port bit for it is 0
        if (new_bits & UFO) {
        AudioMixer::getInstance()->playSoundEffect("ufo_lowpitch.wav", true);
        }
        if (!(new_value & UFO)) {
        AudioMixer::getInstance()->stopSoundEffect("ufo_lowpitch.wav");
        }

        // Regular Port 3 handling
        if (new_bits & SHOTS) {
        AudioMixer::getInstance()->playSoundEffect("shoot.wav");
        }
        if (new_bits & PLAYER_DIE) {
        AudioMixer::getInstance()->playSoundEffect("explosion.wav");
        }
        if (new_bits & INVADER_DIE) {
        AudioMixer::getInstance()->playSoundEffect("invaderkilled.wav");
        }
    }

    // Port 5 sounds
    if (port_num == 5){
        if (new_bits & FLEET1) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader1.wav");
        }
        if (new_bits & FLEET2) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader2.wav");
        }
        if (new_bits & FLEET3) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader3.wav");
        }
        if (new_bits & FLEET4) {
        AudioMixer::getInstance()->playSoundEffect("fastinvader4.wav");
        }
        if (new_bits & UFO_HIT) {
        AudioMixer::getInstance()->playSoundEffect("ufo_highpitch.wav");
        }
    }

}
