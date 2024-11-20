#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 *  Created by Ian McCubbin, 10/19/2024
 *  Mainwindow for SpaceInvaders Emulator, contains options to play,
 *  set controls, and show instructions
 *
 *  Modified by Ian McCubbin, 10/23/2024
 *  -  Integrated instructions page
 *  -  Additional optimiziations to main window for input handling
 *
 *  Modified by Ian McCubbin, 10/23/2024
 *  -  Integrated settings page
 *
 *  Modified by Ian McCubbin, 10/28/2024
 *  - Added ability to read keymap.json to set game key controls.
 *
 *  Modified by Noah Freeman, 11/20/2024
 *  - Added debug pause, step, resume functionality
*/

#include <QMainWindow>
#include <QKeyEvent>
#include <QThread>
#include <QMap>
#include <QOpenGLWidget>
#include <QTimer>
#include <QShortcut>

#include "pixelwidget.h"
#include "../inputManager/inputManager.h"
#include "../outputmanager/outputManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief The MainWindow class represents the main interface of the Space Invaders emulator.
 *
 * This class provides the main window with options to play the game, access settings,
 * view instructions, and handle user inputs during gameplay.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the MainWindow object.
     * @param parent Pointer to the parent widget, default is nullptr.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

protected:
    /**
     * @brief Handles key press events.
     * @param event Pointer to the QKeyEvent object.
     *
     * This function overrides the default keyPressEvent to process game-related
     * keystrokes when the game is running.
     */
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    /**
     * @brief Handles resize events for MainWindow.
     * @param event Pointer to the QResizeEvent object containing details of the resize event.
     *
     * This function is triggered every time the MainWindow is resized.
     * If the GLWidget exists, it resizes the GLWidget to match the size
     * of the frame in the MainWindow.
     *
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief Slot for handling the Play Game button click event.
     *
     * Initiates the game by hiding navigation buttons, starting the InputManager
     * in a separate thread, loading key mappings, and displaying the OpenGL widget.
     */
    void onButtonPlayClicked();

    /**
     * @brief Slot for handling the Settings button click event.
     *
     * Opens the settings dialog for configuring game settings.
     */
    void onButtonSettingsClicked();

    /**
     * @brief Slot for handling the Instructions button click event.
     *
     * Opens the instructions dialog to display game instructions.
     */
    void onButtonInstructionsClicked();

    /**
     * @brief Slot for handling the Pause button click event.
     *
     * Pauses emulator for debugging.
     */
    void onButtonPauseClicked();

    /**
     * @brief Slot for handling the Pause button click event.
     *
     * Resumes emulator for debugging.
     */
    void onButtonResumeClicked();

    /**
     * @brief Slot for handling the Pause button click event.
     *
     * Emulator moves by a step for debugging.
     */
    void onButtonStepClicked();

private:
    Ui::MainWindow *ui;                 ///< Pointer to the UI components.
    QThread inputManagerThread;         ///< Thread for running the InputManager.
    InputManager* inputManager;         ///< Pointer to the InputManager instance.
    QThread outputManagerThread;         ///< Thread for running the OutputManager.
    OutputManager* outputManager = nullptr;  ///< Pointer to the OutputManager instance.
    QTimer* frameTimer;                 ///< Pointer to Timer for updating gameframes.
    PixelWidget *pixelWidget = nullptr;       ///< Pointer to the OpenGL widget for rendering the game.

    bool isGameRunning = false;         ///< Flag indicating if the game is currently running.

    QMap<QString, int> keyMappings;     ///< Map storing key mappings for game controls.
    QVector<int> keycodes;              ///< Vector storing key codes for quick access.

    QShortcut* pauseShortcut;           ///< For debugging pause emulator.
    QShortcut* resumeShortcut;          ///< For debugging resume emulator.
    QShortcut* stepShortcut;            ///< For debugging step to next instruction.

    /**
     * @brief Loads key mappings from a JSON file or uses default mappings.
     *
     * This function attempts to load key mappings from a `.keymap.json` file.
     * If the file does not exist, it uses default key mappings defined in `keymap.h`
     * and optionally saves them to a new file.
     */
    void loadKeyMappings();

    /**
     * @brief Saves the current key mappings to a JSON file.
     *
     * This function writes the current key mappings to a `.keymap.json` file
     * in the application's current directory.
     */
    void saveKeyMappings();

    /**
     * @brief Restores the main menu interface.
     *
     * This function restores the main menu by showing navigation buttons,
     * resetting styles, and stopping the game-related processes.
     */
    void restoreMainMenu();

    /**
     * @brief Removes the OpenGL widget from the interface.
     *
     * This function hides and deletes the OpenGL widget used for rendering the game.
     */
    void removePixelWidget();

    /**
     * @brief Stops retrieving frame updates
     */
    void stopFrameUpdates();

};

#endif // MAINWINDOW_H
