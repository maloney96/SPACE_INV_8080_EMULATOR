#include "audiomixer.h"
#include <QDebug>
#include <QMediaDevices>
#include <QSoundEffect>
#include <QFile>
#include <QDir>

// Initialize static member
AudioMixer* AudioMixer::instance = nullptr;

AudioMixer::AudioMixer(QObject *parent)
    : QObject(parent),
    menuMusic(nullptr)
{
    audioDevice = QMediaDevices::defaultAudioOutput();
    audioOutput = new QAudioOutput(this);
    audioEngine = new QAudioEngine();

    configureAudioEngine();
    qDebug() << "AudioMixer initialized.";
}

AudioMixer::~AudioMixer() {
    qDebug() << "AudioMixer destructor called.";

    // Stop and delete the menu music
    if (menuMusic) {
        menuMusic->stop();
        QObject::disconnect(menuMusic, nullptr, nullptr, nullptr);
        menuMusic->setSource(QUrl());
        menuMusic = nullptr;
        qDebug() << "Menu music stopped and deleted.";
    }

    // Stop and delete all sound effects in the soundBoard
    for (auto it = soundBoard.begin(); it != soundBoard.end(); ++it) {
        QSoundEffect* soundEffect = it.value();
        if (soundEffect) {
            soundEffect->stop();
        }
    }
    soundBoard.clear();
    qDebug() << "SoundBoard cleared and all sound effects deleted.";

    // Delete the audio engine
    if (audioEngine) {
        audioEngine->stop();
        audioEngine = nullptr;
        qDebug() << "Audio engine stopped and deleted.";
    }

    // Delete the audio output
    if (audioOutput) {
        audioOutput = nullptr;
        qDebug() << "Audio output deleted.";
    }

    qDebug() << "AudioMixer destructor completed.";
}


AudioMixer* AudioMixer::getInstance() {
    if (!instance) {
        instance = new AudioMixer();
    }
    return instance;
}

void AudioMixer::configureAudioEngine() {
    // Configure audio engine
    audioEngine->setOutputDevice(audioDevice);
    audioEngine->setOutputMode(QAudioEngine::Stereo);
    audioEngine->setMasterVolume(1.0);
    audioEngine->start();

    // Configure menu music
    menuMusic = new QAmbientSound(audioEngine);
    menuMusic->setSource(QUrl("qrc:/sounds/sounds/MenuTrack.mp3"));
    menuMusic->setLoops(QAmbientSound::Infinite);
    menuMusic->setVolume(0.5f);

    // Prepare sound board of all playable sounds
    constructSoundBoard();
}

void AudioMixer::constructSoundBoard() {
    QMap<QString, QSoundEffect *> soundBoard;
    QString resourcePath = ":/sounds/sounds"; // Path in the Qt Resource System
    QDir dir(resourcePath);

    // Check if the directory exists
    if (!dir.exists()) {
        qWarning() << "The sound resource folder does not exist:" << resourcePath;
    }

    // List all files in the directory
    QStringList soundFiles = dir.entryList(QDir::Files);
    qDebug() << soundFiles;

    if (soundFiles.isEmpty()) {
        qWarning() << "No sound files found in the resource folder:" << resourcePath;
    }

    // Debug: List all found files
    qDebug() << "Found files:" << soundFiles;

    // Populate the map with only .wav files
    for (const QString &file : soundFiles) {
        if (file.endsWith(".wav", Qt::CaseInsensitive)) { // Only include .wav files
            QString key = file;                          // Use the file name as the key
            QString filePath = resourcePath + "/" + file; // Full resource path
            QSoundEffect *sound = new QSoundEffect(); // Create QSoundEffect object
            sound->setSource(QUrl("qrc:/sounds/sounds/" + file)); // Set source
            sound->moveToThread(thread());

            if (sound) {
                AudioMixer::soundBoard.insert(key, sound);      // Add to the map
                qDebug() << "Added QSound for:" << key << "->" << filePath;
            } else {
                qDebug() << "Failed to create QSound for:" << filePath;
            }
        } else {
            qDebug() << "Skipping non-.wav file:" << file;
        }
    }

    if (soundBoard.isEmpty()) {
        qDebug() << "No .wav files found in the resource folder:" << resourcePath;
    } else {
        qDebug() << "Soundboard keys:";
        qDebug() << AudioMixer::soundBoard.keys();
    }

}

void AudioMixer::startMenuMusic() {
    if (menuMusic) {
        menuMusic->stop();
        menuMusic->play();
    }
}

void AudioMixer::stopMenuMusic() {
    if (menuMusic) {
        menuMusic->stop();
    }
}

void AudioMixer::playSoundEffect(const QString &fileName, bool loop) {
    if (!AudioMixer::soundBoard.contains(fileName)) {
        qWarning() << "Sound effect not found for file:" << fileName;
        return;
    }

    QSoundEffect* sound = AudioMixer::soundBoard[fileName];
    if (!sound) {
        qWarning() << "Null QSoundEffect for file:" << fileName;
        return;
    }

    // Used for the UFO
    if (loop) {
        sound->setLoopCount(QSoundEffect::Infinite);
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(sound, "play", Qt::QueuedConnection);
    } else {
        sound->play();
    }

}

void AudioMixer::stopSoundEffect(const QString &fileName) {
    if (!AudioMixer::soundBoard.contains(fileName)) {
        qWarning() << "Sound effect not found for file:" << fileName;
        return;
    }

    QSoundEffect* sound = AudioMixer::soundBoard[fileName];
    if (!sound) {
        qWarning() << "Null QSoundEffect for file:" << fileName;
        return;
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(sound, "stop", Qt::QueuedConnection);
    } else {
        sound->stop();
    }
}
