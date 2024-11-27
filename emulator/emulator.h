/*
 * Created by Noah Freeman on 10/22/24.
 * Adapted from emulator101.com.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMULATOR_H
#define EMULATOR_H

#include <stddef.h>
#include <stdint.h>
#include "../disassembler/disassembler.h"

int parity(int x, int size);

void unimplemented_instruction(state_8080cpu *state);

void write_memory(state_8080cpu *state, uint16_t address, uint8_t value);

void flags_logicA(state_8080cpu *state);

void flags_arithA(state_8080cpu *state, uint16_t res);

void flags_zerosignparity(state_8080cpu *state, uint8_t value);

uint8_t read_HL(state_8080cpu *state);

void write_HL(state_8080cpu *state, uint8_t value);

void handle_ADC(state_8080cpu *state, uint8_t *reg, uint8_t value);

void handle_ADD(state_8080cpu *state, uint8_t *reg, uint8_t value);

void handle_ANA(state_8080cpu *state, uint8_t value);

void handle_CALL(uint8_t conditional, state_8080cpu* state, uint8_t *opcode);

void handle_DAD(uint8_t reg_h, uint8_t reg_l, state_8080cpu *state);

void handle_DCR(uint8_t *reg, state_8080cpu *state);

void handle_DCX(uint8_t *high, uint8_t *low); 

void handle_LXI(uint8_t *high, uint8_t *low, uint8_t *opcode, state_8080cpu *state);

void handle_INX(uint8_t *high, uint8_t *low);

void handle_INR(state_8080cpu *state, uint8_t *reg);

void handle_MOVwithMemory(uint8_t *reg, state_8080cpu *state, int direction);

void handle_MVI(uint8_t *reg, uint8_t *opcode, state_8080cpu *state);

void handle_ORA(state_8080cpu *state, uint8_t value);

void handle_POP(uint8_t *high, uint8_t *low, state_8080cpu *state);

void handle_PUSH(uint8_t high, uint8_t low, state_8080cpu *state);

void handle_SBB(state_8080cpu *state, uint8_t *reg, uint8_t value);

void handle_SUB(state_8080cpu *state, uint8_t *reg, uint8_t value);

void handle_XRA(state_8080cpu *state, uint8_t value);

int emulate_8080cpu(state_8080cpu *state);

void generateInterrupt(state_8080cpu *state, int interrupt_num);

#endif // EMULATOR_H

#ifdef __cplusplus
}
#endif
