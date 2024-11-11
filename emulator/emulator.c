#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"
#include "../disassembler/disassembler.h"

const uint8_t cycles_8080[256] = {
    4, 10, 7, 5, 5, 7, 4, 4,  // 0x00 - 0x07
    3, 10, 7, 5, 5, 7, 4, 4,  // 0x08 - 0x0F
    4, 10, 7, 5, 5, 7, 4, 4,  // 0x10 - 0x17
    3, 10, 7, 5, 5, 7, 4, 4,  // 0x18 - 0x1F
    4, 10, 7, 5, 5, 7, 4, 4,  // 0x20 - 0x27
    3, 10, 7, 5, 5, 7, 4, 4,  // 0x28 - 0x2F
    4, 10, 7, 5, 5, 7, 4, 4,  // 0x30 - 0x37
    3, 10, 7, 5, 5, 7, 4, 4,  // 0x38 - 0x3F
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x40 - 0x47
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x48 - 0x4F
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x50 - 0x57
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x58 - 0x5F
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x60 - 0x67
    4, 4, 4, 4, 4, 4, 4, 4,  // 0x68 - 0x6F
    7, 7, 7, 7, 7, 7, 7, 7,  // 0x70 - 0x77
    7, 7, 7, 7, 7, 7, 7, 7,  // 0x78 - 0x7F
    5, 5, 5, 5, 5, 5, 5, 5,  // 0x80 - 0x87
    5, 5, 5, 5, 5, 5, 5, 5,  // 0x88 - 0x8F
    5, 5, 5, 5, 5, 5, 5, 5,  // 0x90 - 0x97
    5, 5, 5, 5, 5, 5, 5, 5,  // 0x98 - 0x9F
    5, 5, 5, 5, 5, 5, 5, 5,  // 0xA0 - 0xA7
    5, 5, 5, 5, 5, 5, 5, 5,  // 0xA8 - 0xAF
    5, 5, 5, 5, 5, 5, 5, 5,  // 0xB0 - 0xB7
    5, 5, 5, 5, 5, 5, 5, 5,  // 0xB8 - 0xBF
    7, 10, 10, 10, 11, 11, 7, 7,  // 0xC0 - 0xC7
    7, 10, 10, 18, 11, 11, 7, 7,  // 0xC8 - 0xCF
    7, 10, 10, 10, 17, 17, 7, 7,  // 0xD0 - 0xD7
    7, 10, 10, 18, 17, 17, 7, 7,  // 0xD8 - 0xDF
    7, 10, 10, 10, 11, 11, 7, 7,  // 0xE0 - 0xE7
    7, 5, 10, 18, 11, 11, 7, 7,  // 0xE8 - 0xEF
    7, 10, 10, 10, 17, 17, 7, 7,  // 0xF0 - 0xF7
    7, 10, 10, 18, 17, 17, 7, 7   // 0xF8 - 0xFF
};

void unimplemented_instruction(state_8080cpu *state) {
    state->pc--; // Undo PC increment

    // Display error message along with the disassembled instruction
    fprintf(stderr, "Error: No instruction implemented at address %04x: ", state->pc);
    disassemble_opcode(state->memory, state->pc);  // Show the problematic instruction
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
};

void write_memory(state_8080cpu *state, uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        printf("Writing ROM not allowed on %x\n", address);
        return;
     }
     if (address >= 0x4000) {
        printf("Writing out of Space Invaders ROM not allowed on %x\n", address);
        return;
     }
     state->memory[address] = value;
};

void flags_logicA(state_8080cpu *state) {
    state->cc.cy = state->cc.ac = 0;
    state->cc.z = (state->a == 0);
    state->cc.s = (0x80 == (state->a & 0x80));;
    state->cc.p = parity(state->a, 8);
};

void flags_arithA(state_8080cpu *state, uint16_t res) {
    state->cc.cy = (res > 0xff);
    state->cc.z = ((res&0xff) == 0);
    state->cc.s = (0x80 == (res & 0x80));
    state->cc.p = parity(res&0xff, 8);
};

void flags_zerosignparity(state_8080cpu *state, uint8_t value) {
    state->cc.z = (value == 0);
    state->cc.s = (0x80 == (value & 0x80));
    state->cc.p = parity(value, 8);
};

uint8_t read_HL(state_8080cpu *state) {
    uint16_t offset = (state->h << 8) | state->l;
    return state->memory[offset];
};

void write_HL(state_8080cpu *state, uint8_t value) {
    uint16_t offset = (state->h << 8) | state->l;
    write_memory(state, offset, value); 
};

int parity(int x, int size) {
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
};
 
void handle_DAD(uint8_t reg_h, uint8_t reg_l, state_8080cpu *state) {
    uint32_t hl = (state->h << 8) | state->l;
    uint32_t reg_pair = (reg_h << 8) | reg_l;
    uint32_t res = hl + reg_pair;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->cc.cy = ((res & 0xffff0000) != 0);
}

void handle_DCR(uint8_t *reg, state_8080cpu *state) {
    uint8_t res = *reg - 1;
    flags_zerosignparity(state, res);
    *reg = res;
};

void handle_LXI(uint8_t *high, uint8_t *low, uint8_t *opcode, state_8080cpu *state) {
    *low = opcode[1];
    *high = opcode[2];
    state->pc += 2;
};

void handle_INX(uint8_t *high, uint8_t *low) {
    (*low)++;
    if (*low == 0) {
        (*high)++;
    }
};

void handle_MOVwithMemory(uint8_t *reg, state_8080cpu *state, int direction) {
    uint16_t offset = (state->h << 8) | state-> l;
    direction == 0 ? (*reg = state->memory[offset]) : (state->memory[offset] = *reg);
};

void handle_MVI(uint8_t *reg, uint8_t *opcode, state_8080cpu *state) {
    *reg = opcode[1];
    state->pc++;
};

void handle_POP(uint8_t  *high, uint8_t *low, state_8080cpu *state) {
    *low = state->memory[state->sp];
    *high = state->memory[state->sp + 1];
    state->sp += 2;
};

void handle_PUSH(uint8_t high, uint8_t low, state_8080cpu *state) {
    state->memory[state->sp - 1] = high;
    state->memory[state->sp - 2] = low;
    state->sp -= 2;
};

int emulate_8080cpu(state_8080cpu *state) {
	unsigned char *opcode = &state->memory[state->pc];
    int cycles = cycles_8080[*opcode]; // Get the number of cycles for the current opcode

	disassemble_opcode(state->memory, state->pc);
	
	state->pc+=1;	

    switch (*opcode) {
        // NOP
        case 0x00: break;

        // DCR cases
        case 0x05: handle_DCR(&state->b, state); break; // DCR B
        case 0x0d: handle_DCR(&state->c, state); break; // DCR C
        case 0x15: handle_DCR(&state->d, state); break; // DCR D
        case 0x1d: handle_DCR(&state->e, state); break; // DCR E
        case 0x25: handle_DCR(&state->h, state); break; // DCR H
        case 0x2d: handle_DCR(&state->l, state); break; // DCR L
        case 0x35: 							            // DCR M
			uint8_t res = read_HL(state) - 1;
            flags_zerosignparity(state, res);
            write_HL(state, res);
			break;
        case 0x3d: handle_DCR(&state->a, state); break; // DCR A

        // LXI cases
        case 0x01: handle_LXI(&state->b, &state->c, opcode, state); break; // LXI B, word
        case 0x11: handle_LXI(&state->d, &state->e, opcode, state); break; // LXI D, word
        case 0x21: handle_LXI(&state->h, &state->l, opcode, state); break; // LXI H, word
        case 0x31: // LXI SP, word
            state->sp = (opcode[2] << 8) | opcode[1]; 
            state->pc += 2; 
            break;
        
        // STAX cases
        case 0x02: // STAX B
            uint16_t offset = (state->b<<8) | state->c;
            write_memory(state, offset, state->a); 
            break;
        case 0x12:  // STAX D
            uint16_t offset = (state->d<<8) | state->e;
            write_memory(state, offset, state->a);
            break;
        
        // MVI cases
        case 0x06: handle_MVI(&state->b, opcode, state); break; // MVI B, byte
        case 0x0e: handle_MVI(&state->c, opcode, state); break; // MVI C, byte
        case 0x16: handle_MVI(&state->d, opcode, state); break; // MVI D, byte
        case 0x1e: handle_MVI(&state->e, opcode, state); break; // MVI E, byte
        case 0x26: handle_MVI(&state->h, opcode, state); break; // MVI H, byte
        case 0x2e: handle_MVI(&state->l, opcode, state); break; // MVI L, byte
        case 0x36:                                              // MVI M, byte
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->memory[offset] = opcode[1];
                state->pc++;
            }
            break;
        case 0x3e: handle_MVI(&state->a, opcode, state); break; // MVI A, byte

        // RLC case
        case 0x07:
            uint8_t x = state->a;
            state->a = ((x & 0x80) >> 7) | (x << 1);
            state->cc.cy = (0x80 == (x&0x80));
            break;

        // RAL case
        case 0x17:
            uint8_t x = state->a;
            state->a = state->cc.cy  | (x << 1);
            state->cc.cy = (0x80 == (x&0x80));
            break;


        // SHLD case
        case 0x22:
            uint16_t offset = opcode[1] | (opcode[2] << 8);
            write_memory(state, offset, state->l);
            write_memory(state, offset+1, state->h);
            state->pc += 2;
			break;
        
        // DAA case
        case 0x27: 
            if ((state->a &0xf) > 9) {
                state->a += 6;
            }
            if ((state->a&0xf0) > 0x90) {
                uint16_t res = (uint16_t) state->a + 0x60;
                state->a = res & 0xff;
                flags_arithA(state, res);
            }
            break;

        // DAD cases
        case 0x09: handle_DAD(state->b, state->c, state); break; // DAD B
        case 0x19: handle_DAD(state->d, state->e, state); break; // DAD D
        case 0x29:                                               // DAD H
            {
                uint32_t hl = (state->h << 8) | state->l;
                uint32_t res = hl + hl;
                state->h = (res & 0xff00) >> 8;
                state->l = res & 0xff;
                state->cc.cy = ((res & 0xffff0000) != 0);
            }
            break;
        case 0x39: 							                       // DAD SP
            {
                uint32_t hl = (state->h << 8) | state->l;
                uint32_t res = hl + state->sp;
                state->h = (res & 0xff00) >> 8;
                state->l = res & 0xff;
                state->cc.cy = ((res & 0xffff0000) > 0);
            }
                break;
       
        // STC case
        case 0x37: state->cc.cy = 1; break; 
        
        // RRC case
        case 0x0f:
            {
				uint8_t x = state->a;
				state->a = ((x & 1) << 7) | (x >> 1);
				state->cc.cy = (1 == (x&1));
			}
			break;
        
        // LDAX cases
        case 0x0a: // LDAX B
        {
			uint16_t offset=(state->b << 8) | state->c;
			state->a = state->memory[offset];
        }
			break;

        case 0x1a:  // LDAX D
            {
			uint16_t offset = (state->d << 8) | state->e;
			state->a = state->memory[offset];
			}
			break;
        
        // INX cases
        case 0x13: handle_INX(&state->d, &state->e); break; // INX D
        case 0x23: handle_INX(&state->h, &state->l); break; // INX H

        // RAR case
        case 0x1f:
            uint8_t x = state->a;
            state->a = (state->cc.cy << 7) | (x >> 1);
            state->cc.cy = (1 == (x&1));
            break;

        // LHLD case
        case 0x2a:
            uint16_t offset = opcode[1] | (opcode[2] << 8);
            state->l = state->memory[offset];
            state->h = state->memory[offset+1];
            state->pc += 2;
            break;

        // CMA case
        case 0x2f:
            state->a = ~state->a;
            break;

        // STA case
        case 0x32:
            {
			uint16_t offset = (opcode[2] << 8) | (opcode[1]);
			state->memory[offset] = state->a;
			state->pc += 2;
			}
			break;
        
        // LDA case
        case 0x3a: 
            {
			uint16_t offset = (opcode[2]<<8) | (opcode[1]);
			state->a = state->memory[offset];
			state->pc+=2;
			}
			break;

        // MOV cases - MOV DESTINATION, SOURCE
        // DESTINATION B
        case 0x40: state->b = state->b; break;      				 // MOV B, B
        case 0x41: state->b = state->c; break;      				 // MOV B, C
        case 0x42: state->b = state->d; break;      				 // MOV B, D
        case 0x43: state->b = state->e; break;      				 // MOV B, E
        case 0x44: state->b = state->h; break;      				 // MOV B, H
        case 0x45: state->b = state->l; break;      				 // MOV B, L
        case 0x46: handle_MOVwithMemory(&state->b, state, 0); break; // MOV B, M
        case 0x47: state->b = state->a; break;      				 // MOV B, A

        // DESTINATION C
        case 0x48: state->c = state->b; break;      				 // MOV C, B
        case 0x49: state->c = state->c; break;      				 // MOV C, C
        case 0x4a: state->c = state->d; break;      				 // MOV C, D
        case 0x4b: state->c = state->e; break;      				 // MOV C, E
        case 0x4c: state->c = state->h; break;      				 // MOV C, H
        case 0x4d: state->c = state->l; break;      				 // MOV C, L
        case 0x4e: handle_MOVwithMemory(&state->c, state, 0); break; // MOV C, M
        case 0x4f: state->c = state->a; break;      				 // MOV C, A

        // DESTINATION D
        case 0x50: state->d = state->b; break;      				 // MOV D, B
        case 0x51: state->d = state->c; break;      				 // MOV D, C
        case 0x52: state->d = state->d; break;      				 // MOV D, D
        case 0x53: state->d = state->e; break;      				 // MOV D, E
        case 0x54: state->d = state->h; break;      				 // MOV D, H
        case 0x55: state->d = state->l; break;      				 // MOV D, L
        case 0x56: handle_MOVwithMemory(&state->d, state, 0); break; // MOV D, M
        case 0x57: state->d = state->a; break;      				 // MOV D, A

        // DESTINATION E
        case 0x58: state->e = state->b; break;      				 // MOV E, B
        case 0x59: state->e = state->c; break;      				 // MOV E, C
        case 0x5a: state->e = state->d; break;      				 // MOV E, D
        case 0x5b: state->e = state->e; break;      				 // MOV E, E
        case 0x5c: state->e = state->h; break;      				 // MOV E, H
        case 0x5d: state->e = state->l; break;      				 // MOV E, L
        case 0x5e: handle_MOVwithMemory(&state->e, state, 0); break; // MOV E, M
        case 0x5f: state->e = state->a; break;      				 // MOV E, A

        // DESTINATION H
        case 0x60: state->h = state->b; break;      				 // MOV H, B
        case 0x61: state->h = state->c; break;      				 // MOV H, C
        case 0x62: state->h = state->d; break;      				 // MOV H, D
        case 0x63: state->h = state->e; break;      				 // MOV H, E
        case 0x64: state->h = state->h; break;      				 // MOV H, H
        case 0x65: state->h = state->l; break;      				 // MOV H, L
        case 0x66: handle_MOVwithMemory(&state->h, state, 0); break; // MOV H, M
        case 0x67: state->h = state->a; break;      				 // MOV H, A

        // DESTINATION L
        case 0x68: state->l = state->b; break;      				 // MOV L, B
        case 0x69: state->l = state->c; break;      				 // MOV L, C
        case 0x6a: state->l = state->d; break;      				 // MOV L, D
        case 0x6b: state->l = state->e; break;      				 // MOV L, E
        case 0x6c: state->l = state->h; break;      				 // MOV L, H
        case 0x6d: state->l = state->l; break;      				 // MOV L, L
        case 0x6e: handle_MOVwithMemory(&state->l, state, 0); break; // MOV L, M
        case 0x6f: state->l = state->a; break;      				 // MOV L, A

        // DESTINATION M
        case 0x70: handle_MOVwithMemory(&state->b, state, 1); break; // MOV M, B
        case 0x71: handle_MOVwithMemory(&state->c, state, 1); break; // MOV M, C
        case 0x72: handle_MOVwithMemory(&state->d, state, 1); break; // MOV M, D
        case 0x73: handle_MOVwithMemory(&state->e, state, 1); break; // MOV M, E
        case 0x74: handle_MOVwithMemory(&state->h, state, 1); break; // MOV M, H
        case 0x75: handle_MOVwithMemory(&state->l, state, 1); break; // MOV M, L
        //0x76 is HLT
        case 0x76: break;
        case 0x77: handle_MOVwithMemory(&state->a, state, 1); break; // MOV M, A

        // DESTINATION A
        case 0x78: state->a = state->b; break;      				 // MOV A, B
        case 0x79: state->a = state->c; break;      				 // MOV A, C
        case 0x7a: state->a = state->d; break;      				 // MOV A, D
        case 0x7b: state->a = state->e; break;      				 // MOV A, E
        case 0x7c: state->a = state->h; break;      				 // MOV A, H
        case 0x7d: state->a = state->l; break;      				 // MOV A, L
        case 0x7e: handle_MOVwithMemory(&state->a, state, 0); break; // MOV A, M
        case 0x7f: state->a = state->a; break;      				 // MOV A, A

        // ANA case
        case 0xa7:                                                  // ANA A
            state->a = state->a & state->a;
            flags_logicA(state);
            break;
        
        // XRA case
        case 0xaf:                                                  // XRA A
            state->a = state->a ^ state->a;
            flags_logicA(state);
            break;

        // CMP cases
        case 0xb8: {uint16_t res = (uint16_t) state->a - (uint16_t) state->b; flags_arithA(state, res);} break; //CMP B
        case 0xb9: {uint16_t res = (uint16_t) state->a - (uint16_t) state->c; flags_arithA(state, res);} break; //CMP C
        case 0xba: {uint16_t res = (uint16_t) state->a - (uint16_t) state->d; flags_arithA(state, res);} break; //CMP D
        case 0xbb: {uint16_t res = (uint16_t) state->a - (uint16_t) state->e; flags_arithA(state, res);} break; //CMP E
        case 0xbc: {uint16_t res = (uint16_t) state->a - (uint16_t) state->h; flags_arithA(state, res);} break; //CMP H
        case 0xbd: {uint16_t res = (uint16_t) state->a - (uint16_t) state->l; flags_arithA(state, res);} break; //CMP L
        case 0xbe: {uint16_t res = (uint16_t) state->a - (uint16_t) read_HL(state); flags_arithA(state, res);} break; //CMP HL
        case 0xbf: {uint16_t res = (uint16_t) state->a - (uint16_t) state->a; flags_arithA(state, res);} break; //CMP A

        // RNZ case
        case 0xc0:
            if (state->cc.z == 0) {
            state->pc = state->memory[state->sp] | (state->memory[state->sp+1]<<8);
            state->sp += 2;
            }
            break;

        // POP cases
        case 0xc1: handle_POP(&state->b, &state->c, state); break; // POP B
        case 0xd1: handle_POP(&state->d, &state->e, state); break; // POP D
        case 0xe1: handle_POP(&state->h, &state->l, state); break; // POP H
        case 0xf1:                                                 // POP PSW
            {
				state->a = state->memory[state->sp+1];
				uint8_t psw = state->memory[state->sp];
				state->cc.z  = (0x01 == (psw & 0x01));
				state->cc.s  = (0x02 == (psw & 0x02));
				state->cc.p  = (0x04 == (psw & 0x04));
				state->cc.cy = (0x05 == (psw & 0x08));
				state->cc.ac = (0x10 == (psw & 0x10));
				state->sp += 2;
			}
			break;
        
        // RZ case
        case 0xc8:
			if (state->cc.z) {
				state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
				state->sp += 2;
			}
			break;
        
        // CZ case
        case 0xcc: 
            if (state->cc.z == 1) {
                uint16_t ret = state->pc+2;
                write_memory(state, state->sp-1, (ret >> 8) & 0xff);
                write_memory(state, state->sp-2, (ret & 0xff));
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else {
                state->pc += 2;
            }
            break;

        // ACI case
        case 0xce:
            uint16_t x = state->a + opcode[1] + state->cc.cy;
            flags_zerosignparity(state, x&0xff);
            state->cc.cy = (x > 0xff);
            state->a = x & 0xff;
            state->pc++;
            break;
        
        // JNZ case
        case 0xc2:
			if (0 == state->cc.z)
				state->pc = (opcode[2] << 8) | opcode[1];
			else
				state->pc += 2;
			break;
        
        // RNC case
        case 0xd0:
            if (state->cc.cy == 0) {
            state->pc = state->memory[state->sp] | (state->memory[state->sp+1]<<8);
            state->sp += 2;
            }
            break;
		
        // JNC case
        case 0xd2:
            if (0 == state->cc.cy)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JPO case
        case 0xe2:
            if (0 == state->cc.p)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JP case
        case 0xf2:
            if (0 == state->cc.s)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JZ case
        case 0xca:
            if (1 == state->cc.z)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JC case
        case 0xda:
            if (1 == state->cc.cy)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JPE case
        case 0xea:
            if (1 == state->cc.p)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JM case
        case 0xfa:
            if (1 == state->cc.s)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JMP case
        case 0xc3: state->pc = (opcode[2] << 8) | opcode[1]; break;

        // RC case
        case 0xd8:
            if (state->cc.cy != 0) {
                        state->pc = state->memory[state->sp] | (state->memory[state->sp+1]<<8);
                        state->sp += 2;
            }
            break;

        // CC case
        case 0xdc:
            if (state->cc.cy != 0) {
                uint16_t ret = state->pc+2;
                write_memory(state, state->sp-1, (ret >> 8) & 0xff);
                write_memory(state, state->sp-2, (ret & 0xff));
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else {
                state->pc += 2;
            }
            break;
        
        // SBI case
        case 0xde:
         	uint16_t x = state->a - opcode[1] - state->cc.cy;
            flags_zerosignparity(state, x&0xff);
			state->cc.cy = (x > 0xff);
			state->a = x & 0xff;
			state->pc++;
			break;

        // PUSH cases
        case 0xc5: handle_PUSH(state->b, state->c, state); break; // PUSH B
        case 0xd5: handle_PUSH(state->d, state->e, state); break; // PUSH D
        case 0xe5: handle_PUSH(state->h, state->l, state); break; // PUSH H
        case 0xf5:                                                // PUSH PSW
            {
                state->memory[state->sp - 1] = state->a;
                uint8_t psw = (state->cc.z |
                            state->cc.s << 1 |
                            state->cc.p << 2 |
                            state->cc.cy << 3 |
                            state->cc.ac << 4);
                state->memory[state->sp - 2] = psw;
                state->sp -= 2;
            }
            break;
        
        // ADI case
        case 0xc6:
            {
                uint16_t x = (uint16_t) state->a + (uint16_t) opcode[1];
                state->cc.z = ((x & 0xff) == 0);
                state->cc.s = (0x80 == (x & 0x80));
                state->cc.p = parity((x&0xff), 8);
                state->cc.cy = (x > 0xff);
                state->a = (uint8_t) x;
                state->pc++;
			}
			break;

        // RST cases
        case 0xc7: generateInterrupt(state, 0); break;
        case 0xcf: generateInterrupt(state, 1); break;
        case 0xd7: generateInterrupt(state, 2); break;
        case 0xdf: generateInterrupt(state, 3); break;
        case 0xe7: generateInterrupt(state, 4); break;
        case 0xef: generateInterrupt(state, 5); break;
        case 0xf7: generateInterrupt(state, 6); break;
        case 0xff: generateInterrupt(state, 7); break;

        // RET case
        case 0xc9:
			state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
			state->sp += 2;
			break;

        // CALL case
        case 0xcd: 		
			{
                uint16_t ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
			}
 			break;
        
        // OUT case
        case 0xd3: state->pc++; break;

        // CNC case
        case 0xd4: 
            if (state->cc.cy == 0) {
                uint16_t ret = state->pc+2;
                write_memory(state, state->sp-1, (ret >> 8) & 0xff);
                write_memory(state, state->sp-2, (ret & 0xff));
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else {
                state->pc += 2;
            }
            break;

        // SUI case
        case 0xd6:
			uint8_t x = state->a - opcode[1];
            flags_zerosignparity(state, x&0xff);
			state->cc.cy = (state->a < opcode[1]);
			state->a = x;
			state->pc++;
			break;

        // RPO case
        case 0xe0:
			if (state->cc.p == 0) {
				state->pc = state->memory[state->sp] | (state->memory[state->sp+1]<<8);
				state->sp += 2;
			}
			break;

        // ANI case
        case 0xe6:
            {
                state->a = state->a & opcode[1];
                flags_logicA(state);
                state->pc++;
			}
			break;

        // XCHG case
        case 0xeb:
            {
				uint8_t save1 = state->d;
				uint8_t save2 = state->e;
				state->d = state->h;
				state->e = state->l;
				state->h = save1;
				state->l = save2;
			}
			break;

        // EI case
        case 0xfb: state->int_enable = 1;  break;

        // DI case
        case 0xf3: state->int_enable = 0;  break;

        // CPI case
        case 0xfe:
            {
                uint8_t x = state->a - opcode[1];
                state->cc.z = (x == 0);
                state->cc.s = (0x80 == (x & 0x80));
                state->cc.p = parity(x, 8);
                state->cc.cy = (state->a < opcode[1]);
                state->pc++;
			}
			break;

        // Otherwise, treat as unimplemented instruction
        default:
            unimplemented_instruction(state);
            break;
        
        
    }
    // Print Tab
    printf("\t");
    
    // Print Register Values and Flags
    printf("A $%02x B $%02x c $%02x D $%02x E $%02x H $%02x L $%02x SP %04x Flags: %c%c%c%c%c SP:%04x PC:%04x\n",
       state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->sp,
       state->cc.z ? 'Z' : '.', state->cc.s ? 'S' : '.', state->cc.p ? 'P' : '.', 
       state->cc.cy ? 'C' : '.', state->cc.ac ? 'A' : '.', state->sp, state->pc);

    return cycles;
};

void generateInterrupt(state_8080cpu* state, int interrupt_num)
{
    printf("Generating interrupt %d\n", interrupt_num);

    //perform "PUSH PC"
    printf("Pushing Program Counter: %04x\n", state->pc);
    state->memory[state->sp] = state->pc << 8;
    state->sp -= 1;
    state->memory[state->sp] = state->pc >> 8;
    state->sp -= 1;
    //Set the PC to the low memory vector.
    //This is identical to an "RST interrupt_num" instruction.
    state->pc = 8 * interrupt_num;
}
