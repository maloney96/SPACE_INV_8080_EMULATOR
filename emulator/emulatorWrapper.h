#ifndef EMULATORWRAPPER_H
#define EMULATORWRAPPER_H

#include <QObject>
#include <QDebug>
#include <chrono>
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
    std::chrono::high_resolution_clock::time_point previous_timepoint;
    uint8_t interrupt_toggle;

    // Used to emulate specialized bitshifting hardware
    uint8_t shift0;
    uint8_t shift1;

    // We handle IN and OUT opcodes in the wrapper to avoid introducing extra elements like bitshift hardware into emulator core
    void handleOUT(unsigned char* opcode);
    void handleIN(unsigned char* opcode);

    // VideoEmulator instance (manages read-only video memory)
    VideoEmulator* videoEmulator;
};

#endif // EMULATORWRAPPER_H
