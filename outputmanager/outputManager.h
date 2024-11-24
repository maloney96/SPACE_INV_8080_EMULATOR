#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include "../outputmanager/videoemulator.h"
#include "../outputmanager/audiomixer.h"

/**
 * @brief A singleton class that manages the game's video and audio output.
 *
 * This class handles rendering frames, managing audio playback,
 * and provides access to the VideoEmulator.
 */
class OutputManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Gets the singleton instance of the OutputManager.
     *
     * If the instance does not exist, it is created.
     *
     * @return Pointer to the singleton OutputManager instance.
     */
    static OutputManager* getInstance();

    /**
     * @brief Destroys the singleton instance of the OutputManager.
     *
     * This method should be called when the OutputManager is no longer needed.
     */
    static void destroyInstance();

    // Delete copy constructor and assignment operator
    OutputManager(const OutputManager&) = delete;
    OutputManager& operator=(const OutputManager&) = delete;

    /**
     * @brief Updates the video frame.
     *
     * Fetches the current video frame from the EmulatorWrapper.
     */
    Q_INVOKABLE void updateFrame();

    /**
     * @brief Accessor for the VideoEmulator instance.
     * @return Reference to the VideoEmulator singleton.
     */
    Q_INVOKABLE const VideoEmulator* getVideoEmulator() const;

    /**
     * @brief Sets the AudioMixer instance.
     *
     * This method provides the AudioMixer instance for managing audio operations.
     * @param mixer Pointer to the AudioMixer instance.
     */
    void setAudioMixer(AudioMixer* mixer);

    /**
     * @brief Plays a sound effect.
     *
     * Plays the specified sound effect using the AudioMixer.
     * @param filePath Path to the sound effect file.
     */
    void playSoundEffect(const QString& filePath);

    /**
     * @brief Starts background music.
     */
    void startBackgroundMusic();

    /**
     * @brief Stops background music.
     */
    void stopBackgroundMusic();

private:
    /**
     * @brief Private constructor for the singleton pattern.
     */
    explicit OutputManager(QObject *parent = nullptr);

    /**
     * @brief Private destructor for the singleton pattern.
     */
    ~OutputManager();

    /**
     * @brief Static pointer to hold the singleton instance.
     */
    static OutputManager* instance;

    AudioMixer* audioMixer = nullptr; ///< Pointer to the AudioMixer instance
};

#endif // OUTPUTMANAGER_H
