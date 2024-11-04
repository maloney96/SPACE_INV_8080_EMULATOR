//
// Created by Colin Cummins on 10/20/24.
//


#include "../emulator/emulatorWrapper.h"
#include "./t_ioports.h"
#include "./io_bits.h"
#include <thread>
#include <chrono>
#include <string>
#include <unistd.h>

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
    //This works, so ram is definitely being loaded properly
    print_ram_contents(ram->mem, ram->size, "ramdump.txt");
    qDebug() << "Dumped ram contents to ramdump.txt";
    state.memory = ram->mem;
    print_ram_contents(state.memory, MEMORY_SIZE, "ramdump.txt");
}


// Destructor
EmulatorWrapper::~EmulatorWrapper() {
    qDebug() << "EmulatorWrapper destroyed";
}

// This is where most of the emulator loop logic should go
// Currently there's just a dummy function in there
void EmulatorWrapper::runCycle() {
    /* TODO: This still crashes the program
    emulate_8080cpu(&state);
     */

    //Just running dummy port reader for now
    dummyIOportReader();
}

// Function to start the emulator loop
void EmulatorWrapper::startEmulation() {
    running = true;
    auto wait_time = std::chrono::nanoseconds(4000000);
    qDebug() << "Starting emulation...";
    while (running) {
        runCycle();  // Run one cycle of the CPU
        // This is just to slow down the clock cycle to something like the 8080 for testing
        std::this_thread::sleep_for(wait_time);
    }
}

// This is a dummy function so we can test input port handling until we're ready to slot in the emulator

// TODO Clean this up into a loop of some kind
void EmulatorWrapper::dummyIOportReader() {
    //READ01

    std::string iostate = "";

    if (ioports.read01 & CREDIT) {
        iostate.append("CREDIT ");
    }
    if (ioports.read01 & P2START) {
        iostate.append("P2START ");
    }
    if (ioports.read01 & P1START) {
        iostate.append("P1START ");
    }
    if (ioports.read01 & P1SHOT) {
        iostate.append("P1SHOT ");
    }
    if (ioports.read01 & P1LEFT) {
        iostate.append("P1LEFT ");
    }
    if (ioports.read01 & P1RIGHT) {
        iostate.append("P1RIGHT ");
    }
    // READ02
    if (ioports.read02 & DIPSW3) {
        iostate.append("DIPSW3 ");
    }
    if (ioports.read02 & DIPSW5) {
        iostate.append("DIPSW5 ");
    }
    if (ioports.read02 & TILT) {
        iostate.append("TILT ");
    }
    if (ioports.read02 & DIPSW6) {
        iostate.append("DIPSW6 ");
    }
    if (ioports.read02 & P2SHOT) {
        iostate.append("P2SHOT ");
    }
    if (ioports.read02 & P2LEFT) {
        iostate.append("P2LEFT ");
    }
    if (ioports.read02 & P2RIGHT) {
        iostate.append("P2RIGHT ");
    }
    if (ioports.read02 & CINFO) {
        iostate.append("CINFO ");
    }
    if (!iostate.empty()){
        qDebug() << iostate;
    }
}
