//
// Created by Colin Cummins on 10/20/24.
//


#include "../emulator/emulatorWrapper.h"
#include "./io_bits.h"
#include <thread>
#include <chrono>
#include <string>


#define MEMORY_SIZE 0x10000 // 64KB total memory

EmulatorWrapper* EmulatorWrapper::instance = nullptr;

// Static method to get the singleton instance
EmulatorWrapper& EmulatorWrapper::getInstance() {
    if (instance == nullptr) {
        instance = new EmulatorWrapper();  // Create the instance if it doesn't exist
    }
    return *instance;
}

// Private constructor
EmulatorWrapper::EmulatorWrapper() : running(false) {
    // Constructor body
    EmulatorWrapper::ram = create_mem_block(MEMORY_SIZE);
    qDebug() << "EmulatorWrapper Created";
    load_rom(ram, "invaders.rom");
    qDebug() << "ROM Loaded";

    //Initialize state and ioports
    state.memory = ram->mem;
    state.pc = 0;
    state.sp = 0;
    //The first two inputs have register bits that are always 'on'
    state.ioports.read00 = 0b00001110;
    state.ioports.read01 = 0b00001000;
    state.ioports.read02 = 0;
    state.ioports.read03 = 0;
    state.ioports.write02 = 0;
    state.ioports.write03 = 0;
    state.ioports.write04 = 0;
    state.ioports.write05 = 0;
    state.ioports.write06 = 0;
}


// Destructor
EmulatorWrapper::~EmulatorWrapper() {
    qDebug() << "EmulatorWrapper destroyed";
}

// Return pointer to ioports
ioports_t* EmulatorWrapper::getIOptr() {
    return &EmulatorWrapper::state.ioports;
}

// This is where most of the emulator loop logic should go
void EmulatorWrapper::runCycle() {
    emulate_8080cpu(&state);
    //dummyIOportReader();
}

// Function to start the emulator loop
void EmulatorWrapper::startEmulation() {
    running = true;
    qDebug() << "Starting emulation...";
    while (running) {
        runCycle();  // Run one cycle of the CPU
    }
}

// This is a dummy function previously used in the runCycle() to test keystroke detection and io port handling
void EmulatorWrapper::dummyIOportReader() {
    //READ01

    std::string iostate = "";

    if (state.ioports.read01 & CREDIT) {
        iostate.append("CREDIT ");
    }
    if (state.ioports.read01 & P2START) {
        iostate.append("P2START ");
    }
    if (state.ioports.read01 & P1START) {
        iostate.append("P1START ");
    }
    if (state.ioports.read01 & P1SHOT) {
        iostate.append("P1SHOT ");
    }
    if (state.ioports.read01 & P1LEFT) {
        iostate.append("P1LEFT ");
    }
    if (state.ioports.read01 & P1RIGHT) {
        iostate.append("P1RIGHT ");
    }
    // READ02
    if (state.ioports.read02 & DIPSW3) {
        iostate.append("DIPSW3 ");
    }
    if (state.ioports.read02 & DIPSW5) {
        iostate.append("DIPSW5 ");
    }
    if (state.ioports.read02 & TILT) {
        iostate.append("TILT ");
    }
    if (state.ioports.read02 & DIPSW6) {
        iostate.append("DIPSW6 ");
    }
    if (state.ioports.read02 & P2SHOT) {
        iostate.append("P2SHOT ");
    }
    if (state.ioports.read02 & P2LEFT) {
        iostate.append("P2LEFT ");
    }
    if (state.ioports.read02 & P2RIGHT) {
        iostate.append("P2RIGHT ");
    }
    if (state.ioports.read02 & CINFO) {
        iostate.append("CINFO ");
    }
    if (!iostate.empty()){
        qDebug() << iostate;
    }
}