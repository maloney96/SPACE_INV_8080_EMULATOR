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
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "instructions.h"
#include "settings.h"
#include "../inputmanager/keymap.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , keycodes(7)
    , inputManager(nullptr)  // Initialize InputManager pointer to nullptr
    , emulatorWrapper(nullptr)  // Initialize InputManager pointer to nullptr
{
    // load UI file
    ui->setupUi(this);


    // Connect the buttons to their respective slots
    connect(ui->buttonPlay, &QPushButton::clicked, this, &MainWindow::onButtonPlayClicked);
    connect(ui->buttonSettings, &QPushButton::clicked, this, &MainWindow::onButtonSettingsClicked);
    connect(ui->buttonInstructions, &QPushButton::clicked, this, &MainWindow::onButtonInstructionsClicked);
}

MainWindow::~MainWindow()
{
    // kills emulator Input Manager
    if (inputManagerThread.isRunning()) {
        inputManagerThread.quit();
        inputManagerThread.wait();  // Wait for the thread to finish before deleting
    }
    if (emulatorWrapperThread.isRunning()) {
        emulatorWrapperThread.quit();
        emulatorWrapperThread.wait();  // Wait for the thread to finish before deleting
    }
    delete ui;
}


// load keymappings from json file, if it exists, otherwise use default keymap
void MainWindow::loadKeyMappings()
{
    // set keymap path (file will be available after build).
    QString keymapPath = QDir::currentPath() + "/.keymap.json";
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

        // Optionally, save the default key mappings to a new file
        saveKeyMappings();
    }
}

// save keymap configuration to json file
void MainWindow::saveKeyMappings()
{
    // set keymap path.
    QString keymapPath = QDir::currentPath() + "/.keymap.json";
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

        QJsonDocument jsonDoc(jsonObject);
        keymapFile.write(jsonDoc.toJson());
        keymapFile.close();
    }
}

// Overriding the standard keyPressEvent function to process all game related keystrokes when game is running
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
            inputManager->exitGame();
            isGameRunning = false;

            // terminate the input manager thread
            if (inputManagerThread.isRunning()) {
                inputManagerThread.quit();
                inputManagerThread.wait();  // Wait for the thread to finish before deleting
            }
            inputManager->destroyInstance();

            // terminate openGL widget
            if (glWidget) {
                glWidget->hide();
                delete glWidget;
                glWidget = nullptr;
            }

            // restore navigation buttons and background
            ui->buttonPlay->show();
            ui->buttonPlay->setEnabled(true);
            ui->buttonSettings->show();
            ui->buttonSettings->setEnabled(true);
            ui->buttonInstructions->show();
            ui->buttonInstructions->setEnabled(true);
            this->setStyleSheet("background-image: url(:/Images/images/spcaeSky.jpg);");
            ui->frame->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
        }
    }else{ QMainWindow::keyPressEvent(event); } // handle keypresses normally if game is not running
}


// Overriding the standard keyReleaseEvent function to process all game related keystrokes when game is running
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


// Event after Play Game is clicked
// hides/disables all navigation buttons
// instantiates input manager instance (new thread)
// gets the defined keymap, if any.  If not- use default keys from keymap.h.
// instantiates an openGL widget.
void MainWindow::onButtonPlayClicked()
{
    qDebug() << "Play Game button clicked! Starting the game...";

    // Hide all buttons
    ui->buttonPlay->hide();
    ui->buttonPlay->setEnabled(false);
    ui->buttonInstructions->hide();
    ui->buttonInstructions->setEnabled(false);
    ui->buttonSettings->hide();
    ui->buttonSettings->setEnabled(false);

    // Set the frame background color to black
    this->setStyleSheet("background-image: none;");
    ui->frame->setStyleSheet("background-color: black;");

    // Start the InputManager and EmulatorWrapper, each in a separate thread
    // Get the key mappings from file- if it doesn't exist, make a file with default keys
    loadKeyMappings();

    // Start the InputManager in a separate thread
    if (!isGameRunning) {
        emulatorWrapper = &EmulatorWrapper::getInstance();  // Get the singleton instance
        emulatorWrapper->moveToThread(&emulatorWrapperThread);  // Move InputManager to its own thread
        emulatorWrapperThread.start();  // Start the thread

        inputManager = &InputManager::getInstance();  // Get the singleton instance
        inputManager->ioports_ptr = &emulatorWrapper->ioports; // Input manager needs to manipulate interrupts
        inputManager->moveToThread(&inputManagerThread);  // Move InputManager to its own thread
        inputManagerThread.start();  // Start the thread
        isGameRunning = true;  // Set game running flag

        QMetaObject::invokeMethod(emulatorWrapper, "startEmulation", Qt::QueuedConnection); //Start emulation loop inside the wrapper thread
    }

    if(!glWidget) {
        glWidget = new GLWidget(ui->frame);
        glWidget->setGeometry(ui->frame->rect());
        glWidget->show();
    }
}

// Event after settings button is clicked.
// brings up modal settings pop-up
void MainWindow::onButtonSettingsClicked()
{
    qDebug() << "Settings button clicked! Opening settings...";
    // Add your settings logic here
    Settings settingsDialog(this);
    settingsDialog.exec();
}

// Event after instuctions button is clicked
// brings up Instructions pop-up
void MainWindow::onButtonInstructionsClicked()
{
    qDebug() << "Instructions button clicked! Showing instructions...";

    // Create the Instructions dialog
    Instructions instructionsDialog(this);  // Modal dialog with the MainWindow as parent

    // Show the dialog in a modal way (it will block the main window until closed)
    instructionsDialog.exec();
}
