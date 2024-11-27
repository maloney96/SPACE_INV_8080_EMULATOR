#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 *  Created by Ian McCubbin, 10/19/2024
 *  Mainwindow for Space Invaders Emulator, contains options to play,
 *  set controls, and show instructions
 *
 *  Modified by Ian McCubbin, 10/23/2024
 *  -  Integrated instructions page
 *  -  Additional optimizations to main window for input handling
 *
 *  Modified by Ian McCubbin, 10/28/2024
 *  - Added ability to read keymap.json to set game key controls
 *
 *  Modified by Ian McCubbin, 11/2/2024
 *  - Added glWidget controls
 *
 *  Modified by Ian McCubbin, 11/9/2024
 *  - BUGFIX - implemented resize method override for GLWidget
 *
 *  Modified by Noah Freeman, 11/20/2024
 *  - Added debug Pause, Step, Resume functionalities
 */

#include <QMainWindow>
#include <QKeyEvent>
#include <QThread>
#include <QMap>
#include <QTimer>
#include <QShortcut>

#include "pixelwidget.h"
#include "../inputManager/inputManager.h"
#include "../outputmanager/outputManager.h"
#include "../outputmanager/audiomixer.h"

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    AudioMixer* audioMixer; ///< Pointer to the AudioMixer instance
    QThread* audioMixerThread; ///< Thread for running the AudioMixer

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onButtonPlayClicked();
    void onButtonSettingsClicked();
    void onButtonInstructionsClicked();
    void onButtonPauseClicked();
    void onButtonResumeClicked();
    void onButtonStepClicked();

private:
    Ui::MainWindow *ui;
    QThread inputManagerThread;
    InputManager* inputManager;
    QThread outputManagerThread;
    OutputManager* outputManager = nullptr;
    PixelWidget *pixelWidget = nullptr;

    bool isGameRunning = false;

    QMap<QString, int> keyMappings;
    QVector<int> keycodes;

    QShortcut* pauseShortcut;
    QShortcut* resumeShortcut;
    QShortcut* stepShortcut;

    void loadKeyMappings();
    void saveKeyMappings();
    void stopAudioMixer();
    void setUIMode(const QString &mode);
    void setGameBackground();
    void setMenuBackground();
void startAudioMixer();
};

#endif // MAINWINDOW_H
