#ifndef EMULATOR_H
#define EMULATOR_H

#include <stddef.h>
#include <stdint.h>
#include "disassembler.h"

void unimplemented_instruction(state_8080cpu* state);

int emulate_8080cpu(state_8080cpu* state);

#endif // EMULATOR_H