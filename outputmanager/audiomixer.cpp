#include "audiomixer.h"
#include <QDebug>
#include <QMediaDevices>
#include <QSoundEffect>
#include <QFile>
#include <qthread.h>

// Initialize static member
AudioMixer* AudioMixer::instance = nullptr;

AudioMixer::AudioMixer(QObject *parent)
    : QObject(parent),
    audioEngine(new QAudioEngine(this)),
    audioDevice(QMediaDevices::defaultAudioOutput()),
    menuMusic(nullptr),
    mediaPlayer(new QMediaPlayer(this)),
    audioOutput(new QAudioOutput(this))
{
    configureAudioEngine();
    qDebug() << "AudioMixer initialized.";
}

AudioMixer::~AudioMixer() {
    delete menuMusic;
    delete mediaPlayer;
    delete audioOutput;
    delete audioEngine;
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
}

void AudioMixer::startMenuMusic() {
    if (menuMusic) {
        menuMusic->play();
    }
}

void AudioMixer::stopMenuMusic() {
    if (menuMusic) {
        menuMusic->pause();
    }
}

void AudioMixer::playSoundEffect(const QString &filePath) {
    if (QThread::currentThread() != thread()) {
        // Ensure the method is called in the correct thread
        QMetaObject::invokeMethod(this, "playSoundEffect", Qt::QueuedConnection, Q_ARG(QString, filePath));
        return;
    }

    if (filePath.startsWith("qrc:/") || filePath.startsWith(":/")) {
        QSoundEffect* soundEffect = new QSoundEffect(this);
        QUrl soundUrl = QUrl(filePath.startsWith("qrc:/") ? filePath : "qrc" + filePath.mid(1));
        soundEffect->setSource(soundUrl);
        soundEffect->setVolume(1.0);

        connect(soundEffect, &QSoundEffect::playingChanged, this, [soundEffect]() {
            if (!soundEffect->isPlaying()) {
                soundEffect->deleteLater(); // Clean up after playback
            }
        });
        soundEffect->play();
    }
}
