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

// Define the static mutex
QMutex InputManager::instanceMutex;
std::mutex ioports_mutex;

// Static method to get the singleton instance
InputManager& InputManager::getInstance() {
    QMutexLocker locker(&instanceMutex);
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
    QMutexLocker locker(&instanceMutex);  // Lock the mutex
    if (instance != nullptr) {
        qDebug() << "Destroying InputManager instance...";

        // Stop emulator thread if running
        if (instance->emulatorThread && instance->emulatorThread->isRunning()) {
            qDebug() << "Shutting down emulator thread...";

            if (instance->emulatorWrapper) {
                instance->emulatorWrapper->cleanup();  // Cleanup EmulatorWrapper
            }

            instance->emulatorThread->quit();
            instance->emulatorThread->wait();
            qDebug() << "Emulator thread stopped.";
        }

        // Destroy the EmulatorWrapper singleton
        EmulatorWrapper::destroyInstance();
        instance->emulatorWrapper = nullptr;

        // Disconnect all signals
        QObject::disconnect(instance, nullptr, nullptr, nullptr);

        // Cleanup thread
        instance->emulatorThread = nullptr;

        delete instance;
        instance = nullptr;

        qDebug() << "InputManager instance destroyed.";
    } else {
        qDebug() << "InputManager instance already null. Nothing to destroy.";
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
    emulatorThread = new QThread(this);
    emulatorWrapper = &EmulatorWrapper::getInstance();
    qDebug() << "Current thread of emulatorWrapper:" << emulatorWrapper->thread();
    ioports_ptr = emulatorWrapper->getIOptr();

    qDebug() << "InputManager ioports_ptr linked to EmulatorWrapper.";
    // Move EmulatorWrapper to a separate thread
    emulatorWrapper->moveToThread(emulatorThread);

    // Connect signal to start emulation
    connect(this, &InputManager::startEmulatorSignal, emulatorWrapper, &EmulatorWrapper::startEmulation, Qt::QueuedConnection);

    // Start the thread
    emulatorThread->start();

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
    qDebug() << "Destroying InputManager...";

    // Safely stop the thread if running
    if (emulatorThread && emulatorThread->isRunning()) {
        qDebug() << "Stopping emulator thread...";
        emulatorThread->quit();
        emulatorThread->wait();  // Ensure the thread finishes its tasks
    }

    // Cleanup thread
    delete emulatorThread;
    emulatorThread = nullptr;

    qDebug() << "EmulatorWrapper singleton destroyed by InputManager";
}

/**
 * @brief Moves the player to the left.
 *
 * This function processes the input to move the player character to the left.
 */
void InputManager::moveLeft() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= P1LEFT;
        ioports_ptr->read02 |= P2LEFT;
    }, Qt::QueuedConnection);
}

/**
 * @brief Moves the player to the right.
 *
 * This function processes the input to move the player character to the right.
 */
void InputManager::moveRight() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= P1RIGHT;
        ioports_ptr->read02 |= P2RIGHT;
    }, Qt::QueuedConnection);
}

/**
 * @brief Triggers the first player's button press.
 *
 * This function processes the input corresponding to the first player's action button.
 */
void InputManager::p1Button() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= P1START;
    }, Qt::QueuedConnection);
}

/**
 * @brief Triggers the second player's button press.
 *
 * This function processes the input corresponding to the second player's action button.
 */
void InputManager::p2Button() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= P2START;
    }, Qt::QueuedConnection);
}

/**
 * @brief Triggers the fire button press.
 *
 * This function processes the input corresponding to the fire action.
 */
void InputManager::fireButton() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= P1SHOT;
        ioports_ptr->read02 |= P2SHOT;
    }, Qt::QueuedConnection);
}

/**
 * @brief Simulates a coin insert.
 *
 * This function processes the input to simulate inserting a coin into the game.
 */
void InputManager::insertCoin() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 |= CREDIT;
    }, Qt::QueuedConnection);
}

// Methods for handling release of keys
void InputManager::moveLeftKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~P1LEFT;
        ioports_ptr->read02 &= ~P2LEFT;
    }, Qt::QueuedConnection);
}

void InputManager::moveRightKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~P1RIGHT;
        ioports_ptr->read02 &= ~P2RIGHT;
    }, Qt::QueuedConnection);
}

void InputManager::p1ButtonKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~P1START;
    }, Qt::QueuedConnection);
}

void InputManager::p2ButtonKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~P2START;
    }, Qt::QueuedConnection);
}

void InputManager::fireButtonKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~P1SHOT;
        ioports_ptr->read02 &= ~P2SHOT;
    }, Qt::QueuedConnection);
}

void InputManager::insertCoinKeyup() {
    QMetaObject::invokeMethod(this, [this]() {
        std::lock_guard<std::mutex> lock(ioports_mutex);
        ioports_ptr->read01 &= ~CREDIT;
    }, Qt::QueuedConnection);

}

/**
 * @brief Exits the game.
 *
 * This function processes the input to exit the game.
 */
void InputManager::exitGame(){
    QMetaObject::invokeMethod(this, [this]() {
        qDebug() << "Game exited.";
    }, Qt::QueuedConnection);
}
