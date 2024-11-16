//
// Created by Colin Cummins on 10/20/24.
// Processes game inputs and routes them to the emulator
//

/*
 * Modified by Ian McCubbin, 10/25/24
 * - Modified to singleton class
 * - Reclassified gameplay keypress functions as public (need to be accessed via MainWindow)
 *
 * Modified by Ian McCubbin, 10/28/24
 * - Integrated EmulatorWrapper initialization directly into InputManager.
 */

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>  // Include QObject for threading
#include <QDebug>
#include "../emulator/ioports_t.h"
#include "../emulator/emulatorWrapper.h"  // Include EmulatorWrapper for integration

class InputManager : public QObject  // Inherit from QObject
{
    Q_OBJECT  // Enable Qt's meta-object system for QObject features

public:
    // Pointer to IO ports in emulator
    ioports_t* ioports_ptr {nullptr};

    // Static method to get the singleton instance
    static InputManager& getInstance();

    static void destroyInstance();

    // Delete copy constructor and assignment operator to enforce singleton pattern
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Pointer to EmulatorWrapper, initialized when InputManager is created
    EmulatorWrapper* emulatorWrapper {nullptr};

    // Actions Triggered By User Input (keyboard, joystick, etc.)
    // Key down
    void moveLeft();
    void moveRight();
    void p1Button();
    void p2Button();
    void fireButton();
    void exitGame();
    void insertCoin();

    // Key up
    void moveLeftKeyup();
    void moveRightKeyup();
    void p1ButtonKeyup();
    void p2ButtonKeyup();
    void fireButtonKeyup();
    void insertCoinKeyup();

private:
    // Private constructor (singleton pattern)
    InputManager();

    // Private destructor
    ~InputManager();

    // Static pointer to hold the singleton instance
    static InputManager* instance;

    // Commands to change game settings via DIP switches
    void setLives(int numLives);
    void setExtraLife(int extraLifeAt);
};

#endif // INPUTMANAGER_H
