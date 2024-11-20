//
// Created by Colin Cummins on 10/20/24.
//

#include "emulatorWrapper.h"
#include "io_bits.h"
#include <thread>

#define INTERRUPT_INTERVAL 8333333
#define MEMORY_SIZE 0x10000 // 64KB total memory
#define CPU_CLOCK_HZ 2000000 // 2 MHz clock speed

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

    // Initialize timer for interrupts
    previous_timepoint = std::chrono::high_resolution_clock::now();
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

// Emulator cycle execution
void EmulatorWrapper::runCycle() {
    // Wait if debug paused
    {
        std::unique_lock<std::mutex> lock(pauseMutex);
        pauseCondition.wait(lock, [this]() { return !paused; });
    }

    int cycles = emulate_8080cpu(&state);

    // Simulate time delay based on cycles
    int sleep_time = (cycles * 1000000) / CPU_CLOCK_HZ;
    std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));

    // Handle interrupts
    auto current_timepoint = std::chrono::high_resolution_clock::now();
    if ((current_timepoint - previous_timepoint) > std::chrono::nanoseconds(INTERRUPT_INTERVAL)) {
        if (state.int_enable) {
            int interrupt_num = interrupt_toggle + 1; // 1 for mid-screen, 2 for bottom
            generateInterrupt(&state, interrupt_num);
            state.int_enable = false;

            previous_timepoint = current_timepoint;
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
