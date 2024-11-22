//
// Created by Colin Cummins on 10/20/24.
//

#include "emulatorWrapper.h"
#include "io_bits.h"
#include <thread>

#define INTERRUPT_INTERVAL 8333333
#define MEMORY_SIZE 0x10000 // 64KB total memory
#define CPU_CLOCK_HZ 2000000 // 2 MHz clock speed
#define NS_PER_CYCLE 500 // Nanoseconds per clock cycle in 8080

// Static member initialization
EmulatorWrapper* EmulatorWrapper::instance = nullptr;

// Static method to get the singleton instance
EmulatorWrapper& EmulatorWrapper::getInstance() {
    if (instance == nullptr) {
        instance = new EmulatorWrapper();
    }
    return *instance;
}

// Private constructor
EmulatorWrapper::EmulatorWrapper() : running(false), videoEmulator(nullptr) {
    qDebug() << "Creating EmulatorWrapper...";

    // Allocate memory and load ROM
    ram = create_mem_block(MEMORY_SIZE);
    load_rom(ram, "invaders.rom");
    qDebug() << "ROM Loaded";

    // Initialize video memory read-only interface
    const uint8_t* videoMemory = &ram->mem[0x2400];
    videoEmulator = new VideoEmulator(videoMemory);

    // Initialize CPU state
    state.memory = ram->mem;
    state.pc = 0;
    state.sp = 0;

    // Initialize IO ports
    state.ioports.read00 = 0b00001110; // Default state for port 0
    state.ioports.read01 = 0b00001000; // Default state for port 1
    state.ioports.read02 = 0;
    state.ioports.read03 = 0;
    state.ioports.write02 = 0;
    state.ioports.write03 = 0;
    state.ioports.write04 = 0;
    state.ioports.write05 = 0;
    state.ioports.write06 = 0;

    //Shift registers for bit shifting hardware
    shift0 = 0; //Low register
    shift1 = 0; //High register

    // Initialize timer for interrupts
    previous_interrupt_time = std::chrono::high_resolution_clock::now();
    previous_cycle_time = previous_interrupt_time;
    cycles_used =0;
    interrupt_toggle = 0; // 0 = middle of screen, 1 = bottom of screen

    qDebug() << "EmulatorWrapper initialized.";
}

// Destructor
EmulatorWrapper::~EmulatorWrapper() {
    delete videoEmulator;
    delete ram;
    qDebug() << "EmulatorWrapper destroyed.";
}

// Provide access to IO ports
ioports_t* EmulatorWrapper::getIOptr() {
    return &state.ioports;
}

// Provide read-only access to video memory
const uint8_t* EmulatorWrapper::getVideoMemory() const {
    return &state.memory[0x2400];
}

// Provide access to the VideoEmulator
const VideoEmulator* EmulatorWrapper::getVideoEmulator() const {
    return videoEmulator;
}

// Special handling for OUT instructions
void EmulatorWrapper::handleOUT(unsigned char* opcode) {
    switch(opcode[1]){
    //Change shift amount
    case 2:
        state.ioports.write02 = opcode[2] & 0x7;
        break;
    case 3:
        state.ioports.write03 = opcode[2];
        break;
    //Write to shift register
    case 4:
        shift0 = shift1;
        shift1 = state.a;
        break;
    case 5:
        state.ioports.write05 = opcode[2];
        break;
    case 6:
        state.ioports.write06 = opcode[2];
        break;
    }
}

void EmulatorWrapper::handleIN(unsigned char* opcode){
    switch(opcode[1]){
    //Regular read of input port
    case 0: state.a = state.ioports.read00; break;
    case 1: state.a = state.ioports.read01; break;
    case 2: state.a = state.ioports.read02; break;

    //Engage bitshifter
    //Method adapted from Emulator101 code
    case 3:
        uint16_t v = (shift1<<8) | shift0;
        state.ioports.read03 = ((v >> (8-state.ioports.write02)) & 0xff);
        state.a = state.ioports.read03;
        break;
    }
}

// Emulator cycle execution
void EmulatorWrapper::runCycle() {
    // Wait if debug paused
    {
        std::unique_lock<std::mutex> lock(pauseMutex);
        pauseCondition.wait(lock, [this]() { return !paused; });
    }

    // Get current time for cycle synchronization and interrupt generation
    auto current_timepoint = std::chrono::high_resolution_clock::now();

    if (current_timepoint - previous_cycle_time >= std::chrono::nanoseconds(cycles_used * NS_PER_CYCLE)){
        // Opcode handling and cycle emulation
        // Carried out if enough time has elapsed since previous cycle

        // The bulk of IN/OUT opcode handling is done in the wrapper to keep machine-specific elements out of emulator
        // This method adapted from Emulator101 approach
        previous_cycle_time = current_timepoint;

        unsigned char *opcode = &state.memory[state.pc];
        if (*opcode == 0xd3) {
        EmulatorWrapper::handleOUT(opcode);
        } else if (*opcode == 0xdb){
        EmulatorWrapper::handleIN(opcode);
        }
        cycles_used = emulate_8080cpu(&state);
    }

    // Handle interrupts
    if ((current_timepoint - previous_interrupt_time) > std::chrono::nanoseconds(INTERRUPT_INTERVAL)) {
        if (state.int_enable) {
            int interrupt_num = interrupt_toggle + 1; // 1 for mid-screen, 2 for bottom
            generateInterrupt(&state, interrupt_num);
            state.int_enable = false;
            previous_interrupt_time = current_timepoint;
            interrupt_toggle ^= 1; // Toggle interrupt state
        }
    }
}

// Cleanup resources
void EmulatorWrapper::cleanup() {
    running = false;
    delete instance;
    instance = nullptr;
    qDebug() << "EmulatorWrapper cleanup completed.";
}

// Start the emulation loop
void EmulatorWrapper::startEmulation() {
    running = true;
    qDebug() << "Starting emulation...";
    while (running) {
         // Wait if paused and not stepping
        {
            std::unique_lock<std::mutex> lock(pauseMutex);
            pauseCondition.wait(lock, [this]() { return !paused || stepping; });
        }

        runCycle();

        // If stepping, clear the stepping flag and re-enter pause mode
        if (stepping) {
            std::lock_guard<std::mutex> lock(pauseMutex);
            stepping = false;
            paused = true;
        }
    }
}

void EmulatorWrapper::pauseEmulation() {
    std::lock_guard<std::mutex> lock(pauseMutex);
    paused = true;
    pauseCondition.notify_all(); // Notify any waiting threads
    qDebug() << "Emulation paused.";
}

void EmulatorWrapper::resumeEmulation() {
    std::lock_guard<std::mutex> lock(pauseMutex);
    paused = false;
    pauseCondition.notify_all(); // Notify any waiting threads
    qDebug() << "Emulation resumed.";
}

void EmulatorWrapper::stepEmulation() {
    if (paused) {
        stepping = true;
        qDebug() << "Stepping one cycle.";
    } else {
        qDebug() << "Cannot step while running. Pause the emulator first.";
    }
}

// Dummy function for IO testing
void EmulatorWrapper::dummyIOportReader() {
    std::string iostate;

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
    if (!iostate.empty()) {
        qDebug() << QString::fromStdString(iostate);
    }
}
