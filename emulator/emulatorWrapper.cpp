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
EmulatorWrapper::EmulatorWrapper() : running(false) {
    qDebug() << "Creating EmulatorWrapper...";

    // Allocate memory and load ROM
    ram = create_mem_block(MEMORY_SIZE);
    load_rom(ram, "invaders.rom");
    qDebug() << "ROM Loaded";

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

    // Shift registers for bit shifting hardware
    shift0 = 0; // Low register
    shift1 = 0; // High register
    shift_amt = 0; // Shift amount

    // Initialize timer for interrupts
    previous_interrupt_time = std::chrono::high_resolution_clock::now();
    previous_cycle_time = previous_interrupt_time;
    cycles_used = 0;
    interrupt_toggle = 0; // 0 = middle of screen, 1 = bottom of screen

    qDebug() << "EmulatorWrapper initialized.";
}

// Destructor
EmulatorWrapper::~EmulatorWrapper() {
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


// Emulator cycle execution
void EmulatorWrapper::runCycle() {
    // Wait if debug paused
    {
        std::unique_lock<std::mutex> lock(pauseMutex);
        pauseCondition.wait(lock, [this]() { return !paused; });
    }

    auto current_timepoint = std::chrono::high_resolution_clock::now();

    if (current_timepoint - previous_cycle_time >= std::chrono::nanoseconds(cycles_used * NS_PER_CYCLE)) {
        previous_cycle_time = current_timepoint;

        unsigned char* opcode = &state.memory[state.pc];
        if (*opcode == 0xd3) { // OUT instruction
            handleOUT(opcode);
        } else if (*opcode == 0xdb) { // IN instruction
            handleIN(opcode);
        }
        cycles_used = emulate_8080cpu(&state);
    }

    if ((current_timepoint - previous_interrupt_time) > std::chrono::nanoseconds(INTERRUPT_INTERVAL)) {
        if (state.int_enable) {
            int interrupt_num = interrupt_toggle + 1;
            generateInterrupt(&state, interrupt_num);
            state.int_enable = false;
            previous_interrupt_time = current_timepoint;
            interrupt_toggle ^= 1;
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

        if (stepping) {
            std::lock_guard<std::mutex> lock(pauseMutex);
            stepping = false;
            paused = true;
        }
    }
}

void EmulatorWrapper::handleOUT(unsigned char* opcode) {
    uint8_t newly_triggered_ports{0};
    switch (opcode[1]) {
    case 2:
        shift_amt = state.a & 0x7;
        break;
    case 3:
        OutputManager::getInstance()->handleSoundUpdates(3, state.ioports.write03, state.a);
        state.ioports.write03 = state.a;
        break;
    case 4:
        shift0 = shift1;
        shift1 = state.a;
        break;
    case 5:
        OutputManager::getInstance()->handleSoundUpdates(5, state.ioports.write05, state.a);
        state.ioports.write05 = state.a;
        break;
    case 6:
        state.ioports.write06 = opcode[2];
        break;
    }
}

void EmulatorWrapper::handleIN(unsigned char* opcode) {
    switch (opcode[1]) {
    case 0:
        state.a = state.ioports.read00;
        break;
    case 1:
        state.a = state.ioports.read01;
        break;
    case 2:
        state.a = state.ioports.read02;
        break;
    case 3:
        uint16_t v = (shift1 << 8) | shift0;
        state.ioports.read03 = ((v >> (8 - shift_amt)) & 0xff);
        state.a = state.ioports.read03;
        break;
    }
}

// Pause emulation
void EmulatorWrapper::pauseEmulation() {
    std::lock_guard<std::mutex> lock(pauseMutex);
    paused = true;
    pauseCondition.notify_all();
    qDebug() << "Emulation paused.";
}

// Resume emulation
void EmulatorWrapper::resumeEmulation() {
    std::lock_guard<std::mutex> lock(pauseMutex);
    paused = false;
    pauseCondition.notify_all();
    qDebug() << "Emulation resumed.";
}

// Step emulation
void EmulatorWrapper::stepEmulation() {
    if (paused) {
        stepping = true;
        qDebug() << "Stepping one cycle.";
    } else {
        qDebug() << "Cannot step while running. Pause the emulator first.";
    }
}
