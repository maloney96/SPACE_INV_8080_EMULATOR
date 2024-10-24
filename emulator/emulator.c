#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"
#include "disassembler/disassembler.h"

void unimplemented_instruction(state_8080cpu *state) {
    state->pc--; // Undo PC increment

    // Display error message along with the disassembled instruction
    fprintf(stderr, "Error: No instruction implemented at address %04x: ", state->pc);
    disassemble_opcode(state->memory, state->pc);  // Show the problematic instruction
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
};

void handle_DAD(uint8_t reg_h, uint8_t reg_l, state_8080cpu *state) {
    uint32_t hl = (state->h << 8) | state->l;
    uint32_t reg_pair = (reg_h << 8) | reg_l;
    uint32_t res = hl + reg_pair;
    state->h = (res & 0xff00) >> 8;
    state->l = res & 0xff;
    state->cc.cy = ((res & 0xffff0000) != 0);
}

void handle_DCR(uint8_t *reg, condition_codes *cc) {
    uint8_t res = *reg - 1;
    cc->z = (res == 0);
    cc->s = (0x80 == (res & 0x80));
    cc->p = parity(res, 8);
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

int emulate_8080cpu(state_8080cpu *state) {
    int cycles = 4;
	unsigned char *opcode = &state->memory[state->pc];

	disassemble_opcode(state->memory, state->pc);
	
	state->pc+=1;	

    switch (*opcode) {
        // NOP
        case 0x00: break;

        // DCR cases
        case 0x05: handle_DCR(&state->b, &state->cc); break; // DCR B
        case 0x0d: handle_DCR(&state->c, &state->cc); break; // DCR C
        
        // LXI cases
        case 0x01: handle_LXI(&state->b, &state->c, opcode, state); break; // LXI B, word
        case 0x11: handle_LXI(&state->d, &state->e, opcode, state); break; // LXI D, word
        case 0x21: handle_LXI(&state->h, &state->l, opcode, state); break; // LXI H, word
        case 0x31: // LXI SP, word
            state->sp = (opcode[2] << 8) | opcode[1]; 
            state->pc += 2; 
            break;
        
        // MVI cases
        case 0x06: handle_MVI(&state->b, opcode, state); break; // MVI B, byte
        case 0x0e: handle_MVI(&state->c, opcode, state); break; // MVI C, byte
        case 0x26: handle_MVI(&state->h, opcode, state); break; // MVI H, byte
        case 0x36:                                              // MVI M, byte
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->memory[offset] = opcode[1];
                state->pc++;
            }
            break;
        case 0x3e: handle_MVI(&state->a, opcode, state); break; // MVI A, byte

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
        
        // RRC case
        case 0x0f:
            {
				uint8_t x = state->a;
				state->a = ((x & 1) << 7) | (x >> 1);
				state->cc.cy = (1 == (x&1));
			}
			break;
        
        // LDAX case
        case 0x1a:  // LDAX D
            {
			uint16_t offset = (state->d << 8) | state->e;
			state->a = state->memory[offset];
			}
			break;
        
        // INX case
        case 0x13: handle_INX(&state->d, &state->e); break; // INX D
        case 0x23: handle_INX(&state->h, &state->l); break; // INX H

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

        // MOV cases
        case 0x56: handleMOVwithMemory(&state->d, state, 0); break; // MOV D, M
        case 0x5e: handleMOVwithMemory(&state->e, state, 0); break; // MOV E, M
        case 0x66: handleMOVwithMemory(&state->h, state, 0); break; // MOV H, M
        case 0x6f: state->l = state->a; break;      // MOV L, A
        case 0x7a: state->a  = state->d;  break;	// MOV A, D
		case 0x7b: state->a  = state->e;  break;	// MOV A, E
		case 0x7c: state->a  = state->h;  break;	// MOV A, H
        case 0x77: handleMOVwithMemory(&state->a, state, 1); break; // MOV A, M

        // Add more cases...

        // Otherwise, treat as unimplemented instruction
        default:
            unimplemented_instruction(state);
            break;
        
        
    }
    // Print Tab
    printf("\t");
    
    // Print Register Values and Flags
    printf("A $%02x B $%02x c $%02x D $%02x E $%02x H $%02x L $%02x SP %04x Flags: %c%c%c%c%c\n",
       state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->sp,
       state->cc.z ? 'Z' : '.', state->cc.s ? 'S' : '.', state->cc.p ? 'P' : '.', 
       state->cc.cy ? 'C' : '.', state->cc.ac ? 'A' : '.');

    return 0;
};