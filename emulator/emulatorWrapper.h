//
// Created by Colin Cummins on 10/20/24.
//

#ifndef EMULATORWRAPPER_H
#define EMULATORWRAPPER_H

#include <QObject>  // Include QObject for threading
#include <QDebug>
#include "t_ioports.h"
#include "../memory/memory.h"
#include "../disassembler/disassembler.h"
#include "../emulator/emulator.h"
#include "../memory/mem_utils.h"

class EmulatorWrapper : public QObject  // Inherit from QObject
{
    Q_OBJECT  // Enable Qt's meta-object system for QObject features

public:
    // Static method to get the singleton instance
    static EmulatorWrapper& getInstance();


    // Delete copy constructor and assignment operator
    EmulatorWrapper(const EmulatorWrapper&) = delete;
    EmulatorWrapper& operator=(const EmulatorWrapper&) = delete;

    // Interrupts for other classes to manipulate
    t_ioports ioports;

public slots:
    void startEmulation();

    void runCycle();

private:
    // Private constructor (singleton pattern)
    EmulatorWrapper();

    // Private destructor
    ~EmulatorWrapper();

    // Static pointer to hold the singleton instance
    static EmulatorWrapper* instance;

    // Control on/off emulator logic loop
    bool running;

    void dummyIOportReader();

    mem_block_t *ram;

    state_8080cpu *state;
};

#endif // EMULATORWRAPPER_H
