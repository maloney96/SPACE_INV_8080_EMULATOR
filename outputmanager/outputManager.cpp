#include "../outputmanager/outputManager.h"
#include <iostream>
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
    : QObject(parent) {
    VideoEmulator_init(&emulator);
}

OutputManager::~OutputManager() {
    qDebug() << "OutputManager destroyed";
}

void OutputManager::updateFrame() {
    VideoEmulator_generateFrame(&emulator);
}

VideoEmulator* OutputManager::getVideoEmulator() {
    return &emulator;
}
