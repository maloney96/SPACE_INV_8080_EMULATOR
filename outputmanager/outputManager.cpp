#include "../outputmanager/outputManager.h"
#include "../emulator/emulatorWrapper.h"
#include <QDebug>

// Initialize the static instance pointer to nullptr
OutputManager* OutputManager::instance = nullptr;

// Get the singleton instance of the OutputManager
OutputManager* OutputManager::getInstance() {
    if (instance == nullptr) {
        instance = new OutputManager();
    }
    return instance;
}

// Destroy the singleton instance of the OutputManager
void OutputManager::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
        qDebug() << "OutputManager instance destroyed";
    }
}

// Constructor (private for singleton pattern)
OutputManager::OutputManager(QObject *parent)
    : QObject(parent) {
    qDebug() << "OutputManager created";
}

// Destructor
OutputManager::~OutputManager() {
    qDebug() << "OutputManager destroyed";
}

// Update the video frame (placeholder function)
void OutputManager::updateFrame() {
    // Example: Log the frame update
    qDebug() << "Frame updated";
}

// Get a reference to the VideoEmulator singleton
const VideoEmulator* OutputManager::getVideoEmulator() const {
    return VideoEmulator::getInstance(EmulatorWrapper::getInstance().getVideoMemory());
}

// Set the AudioMixer instance
void OutputManager::setAudioMixer(AudioMixer* mixer) {
    audioMixer = mixer;
    qDebug() << "AudioMixer set in OutputManager";
}

// Play a sound effect using AudioMixer
void OutputManager::playSoundEffect(const QString& filePath) {
    if (audioMixer) {
        audioMixer->playSoundEffect(filePath);
    } else {
        qWarning() << "AudioMixer not set. Cannot play sound effect.";
    }
}

// Start background music using AudioMixer
void OutputManager::startBackgroundMusic() {
    if (audioMixer) {
        audioMixer->startMenuMusic();
    } else {
        qWarning() << "AudioMixer not set. Cannot start background music.";
    }
}

// Stop background music using AudioMixer
void OutputManager::stopBackgroundMusic() {
    if (audioMixer) {
        audioMixer->stopMenuMusic();
    } else {
        qWarning() << "AudioMixer not set. Cannot stop background music.";
    }
}
