#ifndef EMULATOR_H
#define EMULATOR_H

#include <stddef.h>
#include <stdint.h>
#include "disassembler/disassembler.h"

void unimplemented_instruction(state_8080cpu* state);

void handle_LXI(uint8_t* high, uint8_t* low, uint8_t* opcode, state_8080cpu* state);
void handle_MOVwithMemory(uint8_t* reg, state_8080cpu* state, int direction);

int emulate_8080cpu(state_8080cpu* state);

#endif // EMULATOR_H