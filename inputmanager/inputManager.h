//
// Created by Colin Cummins on 10/20/24.
// Proccesses game inputs and routes to emulator
//

/*
 * Modified by Ian McCubbin, 10/25/24
 * - Modified to singleton class
 * - Reclassified gameplay keypress functions as public (need to be accessed via mainwindow)
 */
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>  // Include QObject for threading
#include <QDebug>
#include "../emulator/t_ioports.h"

class InputManager : public QObject  // Inherit from QObject
{
    Q_OBJECT  // Enable Qt's meta-object system for QObject features

public:
    // Pointer to interrupts in emulator
    t_ioports* ioports_ptr {nullptr};

    // Static method to get the singleton instance
    static InputManager& getInstance();

    static void destroyInstance();

    // Delete copy constructor and assignment operator
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Actions Triggered By User Input (keyboard, joystick, etc)
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
    void exitGameKeyup(); //Unused because exiting the game will destroy emulator and io ports along with it
    void insertCoinKeyup();
private:
    // Private constructor (singleton pattern)
    InputManager();

    // Private destructor
    ~InputManager();

    // Static pointer to hold the singleton instance
    static InputManager* instance;


    // Special commands to change game settings through 1 or more dip switches
    void setLives(int numLives);
    void setExtraLife(int extraLifeAt);

};

#endif // INPUTMANAGER_H
