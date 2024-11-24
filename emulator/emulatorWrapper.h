#ifndef EMULATORWRAPPER_H
#define EMULATORWRAPPER_H

#include <QObject>
#include <QDebug>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include "../memory/memory.h"
#include "../disassembler/disassembler.h"
#include "../emulator/emulator.h"
#include "../memory/mem_utils.h"
#include "../outputmanager/videoemulator.h"
#include "ioports_t.h"

class EmulatorWrapper : public QObject {
    Q_OBJECT

public:
    // Static method to get the singleton instance
    static EmulatorWrapper& getInstance();

    // Delete copy constructor and assignment operator
    EmulatorWrapper(const EmulatorWrapper&) = delete;
    EmulatorWrapper& operator=(const EmulatorWrapper&) = delete;

    void cleanup();
    ioports_t* getIOptr();

    // Provide access to video memory (read-only)
    const uint8_t* getVideoMemory() const;

    // Provide access to VideoEmulator for rendering or analysis
    const VideoEmulator* getVideoEmulator() const;

public slots:
    void startEmulation();
    void runCycle();
    void pauseEmulation();
    void resumeEmulation();
    void stepEmulation();

private:
    // Private constructor (singleton pattern)
    EmulatorWrapper();
    ~EmulatorWrapper();

    // Static instance pointer
    static EmulatorWrapper* instance;

    // Emulator state and helper functions
    bool running;
    void dummyIOportReader();

    // CPU Memory and State
    mem_block_t* ram;
    state_8080cpu state;

    // Interrupt and timing handling
    std::chrono::high_resolution_clock::time_point previous_interrupt_time;
    std::chrono::high_resolution_clock::time_point previous_cycle_time;
    uint8_t cycles_used;
    uint8_t interrupt_toggle;

    // Used to emulate specialized bitshifting hardware
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shift_amt;

    // Handle IN and OUT opcodes
    void handleOUT(unsigned char* opcode);
    void handleIN(unsigned char* opcode);

    // For emulator debugging controls
    std::condition_variable pauseCondition;
    std::mutex pauseMutex;
    std::atomic<bool> paused;
    std::atomic<bool> stepping;
};

#endif // EMULATORWRAPPER_H
