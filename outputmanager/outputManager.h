#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include "../outputmanager/videoemulator.h"

/**
 * @brief A singleton class that manages the game's video output.
 *
 * This class handles rendering frames and provides access to the VideoEmulator.
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
     * @return Reference to the VideoEmulator.
     */
    Q_INVOKABLE const VideoEmulator& getVideoEmulator() const;

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

    VideoEmulator emulator; ///< VideoEmulator instance for managing video frames
};

#endif // OUTPUTMANAGER_H
