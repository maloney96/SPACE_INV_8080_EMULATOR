/*
 * Created by Colin Cummins on 10/20/24.
 * Modified by Ian McCubbin, 10/25/24
 * -  Modified to singleton class so that the manager only has one instance running
 * -  Enabled QDebug statements
 * -  moved bypassKey function (handled in mainwindow)
 * -  moved processKeystroke function(handled in mainwindow)
 */

#include "../inputManager/inputManager.h"
#include "../emulator/io_bits.h"

// Initialize the static instance ptr to nullptr
InputManager* InputManager::instance = nullptr;

// Static method to get the singleton instance
InputManager& InputManager::getInstance() {
    if (instance == nullptr) {
        instance = new InputManager();  // Create the instance if it doesn't exist
    }
    return *instance;
}

/**
 * @brief Destroys the singleton instance of InputManager.
 *
 * This method should be called when the InputManager is no longer needed.
 */
void InputManager::destroyInstance() {
    if (instance != nullptr) {
        delete instance;  // Delete the singleton instance
        instance = nullptr;  // Set the pointer to nullptr
        qDebug() << "InputManager instance destroyed";
    }
}

/**
 * @brief Private constructor for the singleton pattern.
 *
 * Initializes the InputManager. This constructor is private to prevent
 * direct instantiation, ensuring that only one instance exists.
 */
InputManager::InputManager() {
    // Get the singleton EmulatorWrapper instance
    emulatorWrapper = &EmulatorWrapper::getInstance();
    ioports_ptr = emulatorWrapper->getIOptr();

    qDebug() << "InputManager ioports_ptr linked to EmulatorWrapper.";

    // Move EmulatorWrapper to a separate thread
    emulatorWrapper->moveToThread(&emulatorThread);

    // Connect signal to start emulation
    connect(this, &InputManager::startEmulatorSignal, emulatorWrapper, &EmulatorWrapper::startEmulation, Qt::QueuedConnection);

    // Start the thread
    emulatorThread.start();

    qDebug() << "InputManager initialized with EmulatorWrapper running in a separate thread.";

    // Emit signal to start emulation after setup is complete
    QMetaObject::invokeMethod(this, "startEmulatorSignal", Qt::QueuedConnection);
}

/**
 * @brief Destructor for InputManager.
 *
 * Cleans up resources used by the InputManager. This destructor is private
 * to control the destruction of the singleton instance.
 */
InputManager::~InputManager() {
    EmulatorWrapper::getInstance().cleanup();  // Destroy the EmulatorWrapper
    qDebug() << "EmulatorWrapper singleton destroyed by InputManager";
}

/**
 * @brief Moves the player to the left.
 *
 * This function processes the input to move the player character to the left.
 */
void InputManager::moveLeft() {
    //qDebug() << "Move left";
    ioports_ptr->read01 |= P1LEFT;
    ioports_ptr->read02 |= P2LEFT;
}

/**
 * @brief Moves the player to the right.
 *
 * This function processes the input to move the player character to the right.
 */
void InputManager::moveRight() {
    //qDebug() << "Move right";
    ioports_ptr->read01 |= P1RIGHT;
    ioports_ptr->read02 |= P2RIGHT;
}

/**
 * @brief Triggers the first player's button press.
 *
 * This function processes the input corresponding to the first player's action button.
 */
void InputManager::p1Button() {
    //qDebug() << "P1 Button";
    ioports_ptr->read01 |= P1START;
}

/**
 * @brief Triggers the second player's button press.
 *
 * This function processes the input corresponding to the second player's action button.
 */
void InputManager::p2Button() {
    //qDebug() << "P2 Button";
    ioports_ptr->read01 |= P2START;
}

/**
 * @brief Triggers the fire button press.
 *
 * This function processes the input corresponding to the fire action.
 */
void InputManager::fireButton() {
    //qDebug() << "Fire Button";
    ioports_ptr->read01 |= P1SHOT;
    ioports_ptr->read02 |= P2SHOT;
}


/**
 * @brief Simulates a coin insert.
 *
 * This function processes the input to simulate inserting a coin into the game.
 */
void InputManager::insertCoin() {
    //qDebug() << "Coin Inserted";
    ioports_ptr->read01 |= CREDIT;
}

// Methods for handling release of keys
void InputManager::moveLeftKeyup() {
    ioports_ptr->read01 &= ~P1LEFT;
    ioports_ptr->read02 &= ~P2LEFT;
}

void InputManager::moveRightKeyup() {
    ioports_ptr->read01 &= ~P1RIGHT;
    ioports_ptr->read02 &= ~P2RIGHT;
}

void InputManager::p1ButtonKeyup() {
    ioports_ptr->read01 &= ~P1START;
}

void InputManager::p2ButtonKeyup() {
    ioports_ptr->read01 &= ~P2START;
}

void InputManager::fireButtonKeyup() {
    ioports_ptr->read01 &= ~P1SHOT;
    ioports_ptr->read02 &= ~P2SHOT;
}

void InputManager::insertCoinKeyup() {
    ioports_ptr->read01 &= ~CREDIT;
}

/**
 * @brief Exits the game.
 *
 * This function processes the input to exit the game.
 */
void InputManager::exitGame(){
    //qDebug() << "Game Exited";
}
