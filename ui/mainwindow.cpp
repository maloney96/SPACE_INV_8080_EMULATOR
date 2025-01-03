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
 *  Modified by Ian McCubbin, 11/2/2024
 *  - Added glWidget controls
 *
 *  Modified by Ian McCubbin, 11/9/2024
 *  - BUGFIX- implemented resize method override for GLWidget
 *  Modified by Noah Freeman, 11/20/2024
 *  - Added Pause, Step, Resume functionalities
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "instructions.h"
#include "settings.h"
#include "../inputmanager/keymap.h"
#include "../outputmanager/outputManager.h"
#include "../inputmanager/romassembler.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QTimer>
#include <QResizeEvent>
#include <QPushButton>
#include <QShortcut>

void MainWindow::startAudioMixer()
{
    audioMixer = AudioMixer::getInstance();
    audioMixerThread = new QThread(this);
    audioMixer->moveToThread(audioMixerThread);
    audioMixerThread->start();
    QMetaObject::invokeMethod(audioMixer, &AudioMixer::startMenuMusic, Qt::QueuedConnection);
}

/**
 * @brief Constructs the MainWindow object.
 *
 * Initializes the user interface, sets up button connections, and initializes member variables.
 *
 * @param parent Pointer to the parent widget, default is nullptr.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    keycodes(7),
    inputManager(nullptr),
    outputManager(nullptr)
{
    // load UI file
    ui->setupUi(this);
    setMinimumSize(OutputManager::SCREEN_WIDTH, OutputManager::SCREEN_HEIGHT);

    startAudioMixer();

    // assembles .ROM file from segragated files.
    std::unique_ptr<RomAssembler> r = std::make_unique<RomAssembler>();

    // Connect the buttons to their respective slots
    connect(ui->buttonPlay, &QPushButton::clicked, this, &MainWindow::onButtonPlayClicked);
    connect(ui->buttonSettings, &QPushButton::clicked, this, &MainWindow::onButtonSettingsClicked);
    connect(ui->buttonInstructions, &QPushButton::clicked, this, &MainWindow::onButtonInstructionsClicked);

    // Manage shortcuts for debug
    QShortcut* pauseShortcut = new QShortcut(QKeySequence("P"), this);
    QShortcut* resumeShortcut = new QShortcut(QKeySequence("R"), this);
    QShortcut* stepShortcut = new QShortcut(QKeySequence("S"), this);

    // Connect the shortcuts to emulator actions
    connect(pauseShortcut, &QShortcut::activated, this, []() {
        EmulatorWrapper::getInstance().pauseEmulation();
        qDebug() << "Pause shortcut activated!";
    });

    connect(resumeShortcut, &QShortcut::activated, this, []() {
        EmulatorWrapper::getInstance().resumeEmulation();
        qDebug() << "Resume shortcut activated!";
    });

    connect(stepShortcut, &QShortcut::activated, this, []() {
        EmulatorWrapper::getInstance().stepEmulation();
        qDebug() << "Step shortcut activated!";
    });
}


/**
 * @brief Destructor for MainWindow.
 *
 * Ensures that the threads are properly terminated and cleans up the user interface.
 */
MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructor called.";

    stopAudioMixer();

    // Clean up OutputManager thread
    if (outputManagerThread.isRunning()) {
        qDebug() << "Terminating OutputManager thread.";
        outputManagerThread.quit();
        outputManagerThread.wait();
    }

    // Destroy OutputManager instance safely
    if (outputManager) {
        QMetaObject::invokeMethod(outputManager, "destroyInstance", Qt::BlockingQueuedConnection);
        outputManager = nullptr;
        qDebug() << "OutputManager destroyed.";
    }

    if (pixelWidget) {
        qDebug() << "Cleaning up PixelWidget.";
        delete pixelWidget;
        pixelWidget = nullptr;
    }

    // Clean up InputManager thread
    if (inputManagerThread.isRunning()) {
        qDebug() << "Terminating InputManager thread.";
        inputManagerThread.quit();
        inputManagerThread.wait();
    }

    // Destroy InputManager instance safely
    if (inputManager) {
        QMetaObject::invokeMethod(inputManager, "destroyInstance", Qt::BlockingQueuedConnection);
        inputManager = nullptr;
        qDebug() << "InputManager destroyed.";
    }

    // Clean up UI
    delete ui;
    qDebug() << "MainWindow cleanup complete.";
}



/**
 * @brief Loads key mappings from a JSON file or sets default mappings.
 *
 * Checks for the existence of a `.settings.json` file in the current directory. If the file exists,
 * it loads the key mappings from it. If not, it sets the default key mappings and optionally saves
 * them to a new file.
 */
void MainWindow::loadKeyMappings()
{
    // set keymap path (file will be available after build).
    QString keymapPath = QDir::currentPath() + "/.settings.json";
    QFile keymapFile(keymapPath);


    // If the keymap file exists, load it into memory
    if (keymapFile.exists()) {
        if (keymapFile.open(QIODevice::ReadOnly)) {
            QByteArray data = keymapFile.readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            QJsonObject jsonObject = jsonDoc.object();

            // Load keys into the keyMappings map
            keyMappings["left"] = keycodes[0] = jsonObject["left"].toInt();
            keyMappings["right"] = keycodes[1] =  jsonObject["right"].toInt();
            keyMappings["fire"] = keycodes[2] = jsonObject["fire"].toInt();
            keyMappings["p1_button"] = keycodes[3] = jsonObject["p1_button"].toInt();
            keyMappings["p2_button"] = keycodes[4] = jsonObject["p2_button"].toInt();
            keyMappings["insert_coin"] = keycodes[5] = jsonObject["insert_coin"].toInt();
            keyMappings["exit_game"] = keycodes[6] = jsonObject["exit_game"].toInt();
        }
    } else {
        // If the file doesn't exist, use default from keymap.h
        keyMappings["left"] = keycodes[0]  = DEFAULT_LEFT;
        keyMappings["right"] = keycodes[1] = DEFAULT_RIGHT;
        keyMappings["fire"] = keycodes[2] = DEFAULT_FIRE;
        keyMappings["p1_button"] = keycodes[3] = DEFAULT_P1_BUTTON;
        keyMappings["p2_button"] = keycodes[4] = DEFAULT_P2_BUTTON;
        keyMappings["insert_coin"] = keycodes[5] = DEFAULT_INSERT_COIN;
        keyMappings["exit_game"] = keycodes[6] = DEFAULT_EXIT;
        keyMappings["lives"] = DEFAULT_EXTRA_LIVES;
        keyMappings["extra_life_at"] = DEFAULT_EXTRA_LIFE_AT;
        // Save the default key mappings to a new file
        saveKeyMappings();
    }
}

/**
 * @brief Saves the current key mappings to a JSON file.
 *
 * Writes the current key mappings to a `.settings.json` file in the current directory.
 */
void MainWindow::saveKeyMappings()
{
    // set keymap path.
    QString keymapPath = QDir::currentPath() + "/.settings.json";
    QFile keymapFile(keymapPath);


    // verify that the file can open, write new values and save
    if (keymapFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;
        jsonObject["left"] = keyMappings["left"];
        jsonObject["right"] = keyMappings["right"];
        jsonObject["fire"] = keyMappings["fire"];
        jsonObject["p1_button"] = keyMappings["p1_button"];
        jsonObject["p2_button"] = keyMappings["p2_button"];
        jsonObject["insert_coin"] = keyMappings["insert_coin"];
        jsonObject["exit_game"] = keyMappings["exit_game"];
        jsonObject["lives"] = keyMappings["lives"] = DEFAULT_EXTRA_LIVES;
        jsonObject["extra_life_at"]= keyMappings["extra_life_at"] = DEFAULT_EXTRA_LIFE_AT;

        QJsonDocument jsonDoc(jsonObject);
        keymapFile.write(jsonDoc.toJson());
        keymapFile.close();
    }
}

/**
 * @brief Handles key press events.
 *
 * This function overrides the default key press event handler to process game-related keystrokes when the game is running.
 * It maps specific keys to corresponding actions in the InputManager.
 *
 * @param event Pointer to the QKeyEvent object containing details of the key press event.
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    // if the game is running and the inputManager thread is up
    if(inputManagerThread.isRunning() && isGameRunning)
    {
        // process game related keys
        if(key == keycodes[0]) { inputManager->moveLeft(); }
        if(key == keycodes[1]) { inputManager->moveRight(); }
        if(key == keycodes[2]) { inputManager->fireButton(); }
        if(key == keycodes[3]) { inputManager->p1Button(); }
        if(key == keycodes[4]) { inputManager->p2Button(); }
        if(key == keycodes[5]) { inputManager->insertCoin(); }
        if(key == keycodes[6])
        {
            // exit key pressed, exit game
            isGameRunning = false;
            outputManager->stopVideo();

            // terminate the input manager thread
            if (inputManagerThread.isRunning()) {
                inputManagerThread.quit();
                inputManagerThread.wait();  // Wait for the thread to finish before deleting
            }
            inputManager->destroyInstance();
            inputManager = nullptr;

            // pixelWidget
            if (pixelWidget) {
                pixelWidget->hide();
                pixelWidget->setEnabled(false);
                pixelWidget->setVisible(false);
                delete pixelWidget;
                pixelWidget = nullptr;
            }

            if (outputManagerThread.isRunning()){
                outputManagerThread.quit();
                outputManagerThread.wait();
            }

            if (outputManager) {
                outputManager->destroyInstance();
                outputManager = nullptr;
            }

            // restore navigation buttons and background
            setUIMode("Menu");
        }
    }else{ QMainWindow::keyPressEvent(event); } // handle keypresses normally if game is not running
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();

    // if the game is running and the inputManager thread is up
    if(inputManagerThread.isRunning() && isGameRunning)
    {
        // process game related keys
        if(key == keycodes[0]) { inputManager->moveLeftKeyup(); }
        if(key == keycodes[1]) { inputManager->moveRightKeyup(); }
        if(key == keycodes[2]) { inputManager->fireButtonKeyup(); }
        if(key == keycodes[3]) { inputManager->p1ButtonKeyup(); }
        if(key == keycodes[4]) { inputManager->p2ButtonKeyup(); }
        if(key == keycodes[5]) { inputManager->insertCoinKeyup(); }
    } else { QMainWindow::keyReleaseEvent(event); } // handle keypresses normally if game is not running
}


void MainWindow::setUIMode(const QString &mode)
{
    bool isMenuMode = (mode != "Game");
    const QList<QPushButton*> buttons = {
        ui->buttonPlay,
        ui->buttonInstructions,
        ui->buttonSettings
    };

    for (QPushButton *button : buttons) {
        button->setVisible(isMenuMode);
        button->setEnabled(isMenuMode);
    }
    if(isMenuMode){
        setMenuBackground();
    }else{setGameBackground();}
}

void MainWindow::setGameBackground()
{
    this->setStyleSheet("background-image: none;");
    ui->frame->setStyleSheet("background-color: black;");
}

void MainWindow::setMenuBackground()
{
    this->setStyleSheet("background-image: url(:/Images/images/spaceSky.jpg);");
    ui->frame->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    if(audioMixer) {
        QMetaObject::invokeMethod(audioMixer, &AudioMixer::startMenuMusic, Qt::QueuedConnection);
    }
}

/**
 * @brief Slot triggered when the "Play Game" button is clicked.
 *
 * This function initializes the game environment by hiding navigation buttons, setting up the InputManager in a separate thread,
 * loading key mappings, and displaying the OpenGL widget for the game.
 */
void MainWindow::onButtonPlayClicked() {
    qDebug() << "\nPlay Game button clicked! Starting the game...";

    // Set UI mode to "Game"
    setUIMode("Game");

    // Stop menu music if the AudioMixer is set
    if (audioMixer) {
        QMetaObject::invokeMethod(audioMixer, &AudioMixer::stopMenuMusic, Qt::QueuedConnection);
    }

    // Load key mappings for the game
    loadKeyMappings();

    // Start the InputManager thread if not already running
    if (!isGameRunning) {
        inputManager = &InputManager::getInstance();
        inputManager->moveToThread(&inputManagerThread);
        inputManagerThread.start();
        isGameRunning = true;
    }

    // Initialize and show the PixelWidget for rendering
    if (!pixelWidget) {
        pixelWidget = new PixelWidget(ui->frame);
    }
    pixelWidget->setGeometry(ui->frame->rect());
    pixelWidget->show();

    // Initialize and start the OutputManager
    if (!outputManager) {
        outputManager = OutputManager::getInstance();
        outputManager->initializeVideo(); // Set up video memory
        outputManager->moveToThread(&outputManagerThread);
        outputManagerThread.start();
        // Connect the frameReady signal to PixelWidget's renderFrame
        connect(outputManager, &OutputManager::frameReady, pixelWidget, &PixelWidget::updatePixelData);
        // Start the video thread
        outputManager->startVideo();
    }
}



/**
 * @brief Slot triggered when the "Settings" button is clicked.
 *
 * This function opens the settings dialog, allowing the user to configure game settings.
 */
void MainWindow::onButtonSettingsClicked()
{
    qDebug() << "Settings button clicked! Opening settings...";
    Settings settingsDialog(this);
    // Show the dialog in a modal way (it will block the main window until closed)
    settingsDialog.exec();
}

/**
 * @brief Slot triggered when the "Instructions" button is clicked.
 *
 * This function opens the instructions dialog, providing the user with game instructions.
 */
void MainWindow::onButtonInstructionsClicked()
{
    qDebug() << "Instructions button clicked! Showing instructions...";

    // Create the Instructions dialog
    Instructions instructionsDialog(this);  // Modal dialog with the MainWindow as parent

    // Show the dialog in a modal way (it will block the main window until closed)
    instructionsDialog.exec();
}

/**
 * @brief Slot triggered when "pause" button is clicked.
 *
 * This function opens the pause button for debugging.
 */
void MainWindow::onButtonPauseClicked() {
    EmulatorWrapper::getInstance().pauseEmulation();
    qDebug() << "Pause button clicked!";
}

/**
 * @brief Slot triggered when "resume" button is clicked.
 *
 * This function opens the resume button for debugging.
 */
void MainWindow::onButtonResumeClicked() {
    EmulatorWrapper::getInstance().resumeEmulation();
    qDebug() << "Resume button clicked!";
}

/**
 * @brief Slot triggered when "step" button is clicked.
 *
 * This function opens the step button for debugging.
 */
void MainWindow::onButtonStepClicked() {
    EmulatorWrapper::getInstance().stepEmulation();
    qDebug() << "Step button clicked!";
}

/**
 * @brief Slot triggered when the "Instructions" button is clicked.
 *
 * This function opens the instructions dialog, providing the user with game instructions.
 */
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);  // Call the base class implementation

    // If PixelWidget  exists, resize it to match the size of the frame
    if (pixelWidget) {
        pixelWidget->setGeometry(ui->frame->rect());
    }
}

void MainWindow::stopAudioMixer()
{
    if (audioMixerThread->isRunning()) {
        QMetaObject::invokeMethod(audioMixer, &AudioMixer::stopMenuMusic, Qt::QueuedConnection);
        audioMixerThread->quit();
        audioMixerThread->wait();
        delete audioMixer;
        delete audioMixerThread;
        qDebug() << "AudioMixer thread stopped and cleaned up.";
    }
}

