#ifndef EMULATOR_H
#define EMULATOR_H

#include <stddef.h>
#include <stdint.h>
#include "../disassembler/disassembler.h"

int parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

void unimplemented_instruction(state_8080cpu *state);

void handle_DAD(uint8_t reg_h, uint8_t reg_l, state_8080cpu *state);

void handle_DCR(uint8_t *reg, condition_codes *cc);

void handle_LXI(uint8_t *high, uint8_t *low, uint8_t *opcode, state_8080cpu *state);

void handle_INX(uint8_t *high, uint8_t *low);

void handle_MOVwithMemory(uint8_t *reg, state_8080cpu *state, int direction);

void handle_MVI(uint8_t *reg, uint8_t *opcode, state_8080cpu *state);

int emulate_8080cpu(state_8080cpu *state);

#endif // EMULATOR_H