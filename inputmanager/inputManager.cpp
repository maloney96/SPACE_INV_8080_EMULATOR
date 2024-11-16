//
// Created by Colin Cummins on 10/20/24.
//

/*
 * Modified by Ian McCubbin, 10/25/24
 * - Modified to singleton class so that the manager only has one instance running
 * - Enabled QDebug statements
 * - Moved bypassKey function (handled in mainwindow)
 * - Moved processKeystroke function (handled in mainwindow)
 *
 * Modified by Ian McCubbin, 10/28/24
 * - Integrated EmulatorWrapper into InputManager during initialization.
 */

#include "../inputmanager/inputManager.h"
#include "../emulator/io_bits.h"
#include "../emulator/emulatorWrapper.h"

InputManager* InputManager::instance = nullptr;  // Initialize static instance pointer

// Static method to get the singleton instance
InputManager& InputManager::getInstance() {
    if (instance == nullptr) {
        instance = new InputManager();  // Create the instance if it doesn't exist
    }
    return *instance;
}

void InputManager::destroyInstance() {
    if (instance != nullptr) {
        delete instance;  // Delete the singleton instance
        instance = nullptr;  // Set the pointer to nullptr
        qDebug() << "InputManager instance destroyed.";
    }
}

// Private constructor
InputManager::InputManager() {
    qDebug() << "InputManager initialized.";

    // Initialize EmulatorWrapper

    emulatorWrapper = &EmulatorWrapper::getInstance();  // Get the singleton instance of EmulatorWrapper
    qDebug() << "EmulatorWrapper initialized and linked to InputManager.";

    ioports_ptr = emulatorWrapper->getIOptr();
    qDebug() << "InputManager ioports_ptr linked to EmulatorWrapper.";

    QMetaObject::invokeMethod(emulatorWrapper, "startEmulation", Qt::QueuedConnection);
    qDebug() << "Emulation loop started in EmulatorWrapper.";
}

// Destructor
InputManager::~InputManager() {
    qDebug() << "InputManager destroyed.";
}

// Methods for handling input keystrokes
// When a key is pressed, the corresponding ioport bit is set
// When a key is released, the corresponding ioport bit is cleared
// This way the io port object always has a picture of what inputs are currently live so the emulator can read them with an IN opcode

void InputManager::moveLeft() {
    ioports_ptr->read01 |= P1LEFT;
}

void InputManager::moveRight() {
    ioports_ptr->read01 |= P1RIGHT;
}

void InputManager::p1Button() {
    ioports_ptr->read01 |= P1START;
}

void InputManager::p2Button() {
    ioports_ptr->read01 |= P2START;
}

void InputManager::fireButton() {
    ioports_ptr->read01 |= P1SHOT;
}

void InputManager::insertCoin() {
    ioports_ptr->read01 |= CREDIT;
}

// Methods for handling release of keys
void InputManager::moveLeftKeyup() {
    ioports_ptr->read01 &= ~P1LEFT;
}

void InputManager::moveRightKeyup() {
    ioports_ptr->read01 &= ~P1RIGHT;
}

void InputManager::p1ButtonKeyup() {
    ioports_ptr->read01 &= ~P1START;
}

void InputManager::p2ButtonKeyup() {
    ioports_ptr->read01 &= ~P2START;
}

void InputManager::fireButtonKeyup() {
    ioports_ptr->read01 &= ~P1SHOT;
}

void InputManager::insertCoinKeyup() {
    ioports_ptr->read01 &= ~CREDIT;
}

// Special keydown command for exiting the game
void InputManager::exitGame() {
    // TODO: This may need to signal the emulator in some way outside the interrupt scheme
    qDebug() << "Game Exited.";
}
