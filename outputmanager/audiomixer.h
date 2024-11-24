#ifndef AUDIOMIXER_H
#define AUDIOMIXER_H

#include <QObject>
#include <QAudioEngine>
#include <QAmbientSound>
#include <QAudioDevice>
#include <QMediaPlayer>
#include <QAudioOutput>

/**
 * @brief The AudioMixer class handles all audio-related functionality in the application.
 */
class AudioMixer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the AudioMixer object.
     * @param parent Pointer to the parent QObject, default is nullptr.
     */
    explicit AudioMixer(QObject *parent = nullptr);

    /**
     * @brief Destroys the AudioMixer object.
     */
    ~AudioMixer();

    /**
     * @brief Plays the background music in a loop.
     */
    void startMenuMusic();

    /**
     * @brief Stops the background music.
     */
    void stopMenuMusic();

    /**
     * @brief Plays a one-shot sound effect.
     * @param filePath Path to the sound effect file.
     */
    void playSoundEffect(const QString &filePath);

    /**
     * @brief Initializes the audio mixer (to be called after moving to a thread).
     */
    void initialize();

private:
    QAudioEngine* audioEngine;      ///< Audio engine for managing audio objects.
    QAudioDevice audioDevice;      ///< Audio device for playback.
    QAmbientSound* menuMusic;      ///< Ambient sound for looping background music.
    QMediaPlayer* mediaPlayer;     ///< Media player for sound effects.
    QAudioOutput* audioOutput;     ///< Audio output for media playback.

    /**
     * @brief Configures the audio engine and initializes components.
     */
    void configureAudioEngine();
};

#endif // AUDIOMIXER_H
