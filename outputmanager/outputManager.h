#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include "../outputmanager/videoemulator.h"

/**
 * @brief A singleton class that manages the game's video output.
 *
 * This class handles the rendering of frames and provides access to the video emulator.
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
     * @return A pointer to the singleton OutputManager instance.
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
     */
    Q_INVOKABLE void updateFrame();

    /**
     * @brief Accessor for the VideoEmulator instance.
     * @return Pointer to the VideoEmulator.
     */
    Q_INVOKABLE VideoEmulator* getVideoEmulator();

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

    VideoEmulator emulator; ///< Holds video memory data
};

#endif // OUTPUTMANAGER_H
