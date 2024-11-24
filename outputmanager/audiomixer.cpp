#include "audiomixer.h"
#include <QDebug>
#include <QMediaDevices>

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
        qDebug() << "Menu music started.";
    }
}

void AudioMixer::stopMenuMusic() {
    if (menuMusic) {
        menuMusic->pause();
        qDebug() << "Menu music stopped.";
    }
}

void AudioMixer::playSoundEffect(const QString &filePath) {
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0); // Adjust volume as needed
    mediaPlayer->play();
    qDebug() << "Playing sound effect from:" << filePath;
}
