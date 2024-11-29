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
#include <QThread>
#include <QMutex>
#include "../emulator/emulatorWrapper.h"
#include "../emulator/ioports_t.h"


/**
 * @brief A singleton class that manages game input.
 *
 * This class handles input from the user (keyboard, joystick, etc.) and routes it to the emulator.
 * It is implemented as a singleton to ensure that only one instance of the InputManager exists.
 */
class InputManager : public QObject  // Inherit from QObject
{
    Q_OBJECT  // Enable Qt's meta-object system for QObject features

public:


     /**
     * @brief Gets the singleton instance of the InputManager.
     *
     * If the instance does not exist, it is created.
     *
     * @return A reference to the singleton InputManager instance.
     */
    static InputManager& getInstance();

    /**
     * @brief Destroys the singleton instance of the InputManager.
     *
     * This method should be called when the InputManager is no longer needed.
     */
    static void destroyInstance();

    // Delete copy constructor and assignment operator
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    /**
     * @brief Moves the player to the left.
     */
    void moveLeft();
    
    /**
     * @brief Moves the player to the right.
     */    
    void moveRight();
    
    /**
     * @brief Triggers the first player's button press.
     */
    void p1Button();

    /**
     * @brief Triggers the second player's button press.
     */    
    void p2Button();

    /**
     * @brief Triggers the fire button press.
     */    
    void fireButton();

    /*
    * @brief Simulates a coin insert
    */
    void insertCoin();

    /*
    * @brief exits the game
    */
    void exitGame();

    void moveLeftKeyup();
    void moveRightKeyup();
    void p1ButtonKeyup();
    void p2ButtonKeyup();
    void fireButtonKeyup();
    void insertCoinKeyup();

signals:
    void startEmulatorSignal();

private:

    /**
     * @brief Private constructor for the singleton pattern.
     */
    InputManager();

    /**
    * @brief Private destructor for the singleton pattern.
    */
    ~InputManager();

    /**
     * @brief Static pointer to hold the singleton instance.
     */
    static InputManager* instance;
    static QMutex instanceMutex;

    EmulatorWrapper* emulatorWrapper;
    QThread* emulatorThread;

    ioports_t* ioports_ptr {nullptr};

    // Special commands to change game settings through 1 or more dip switches
    void setLives(int numLives);
    void setExtraLife(int extraLifeAt);

};

#endif // INPUTMANAGER_H
