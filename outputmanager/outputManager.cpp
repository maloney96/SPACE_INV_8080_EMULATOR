#include "../outputmanager/outputManager.h"
#include "../emulator/emulatorWrapper.h"
#include <QDebug>

// Initialize the static instance pointer to nullptr
OutputManager* OutputManager::instance = nullptr;

OutputManager* OutputManager::getInstance() {
    if (instance == nullptr) {
        instance = new OutputManager();  // Create the instance if it doesn't exist
    }
    return instance;
}

void OutputManager::destroyInstance() {
    if (instance != nullptr) {
        delete instance;  // Delete the singleton instance
        instance = nullptr;  // Set the pointer to nullptr
        qDebug() << "OutputManager instance destroyed";
    }
}

OutputManager::OutputManager(QObject *parent)
    : QObject(parent), emulator(EmulatorWrapper::getInstance().getVideoMemory()) {
    qDebug() << "OutputManager created";
}

OutputManager::~OutputManager() {
    qDebug() << "OutputManager destroyed";
}

void OutputManager::updateFrame() {
    // No operation needed in updateFrame as VideoEmulator is already synchronized
    qDebug() << "Frame updated from emulator memory";
}

const VideoEmulator& OutputManager::getVideoEmulator() const {
    return emulator;
}
