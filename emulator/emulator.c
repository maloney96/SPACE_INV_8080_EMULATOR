#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"
#include "disassembler/disassembler.h"

void unimplemented_instruction(state_8080cpu* state) {
    state->pc--; // Undo PC increment

    // Display error message along with the disassembled instruction
    fprintf(stderr, "Error: No instruction implemented at address %04x: ", state->pc);
    disassemble_opcode(state->memory, state->pc);  // Show the problematic instruction
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
};

void handle_LXI(uint8_t* high, uint8_t* low, uint8_t* opcode, state_8080cpu* state) {
    *low = opcode[1];
    *high = opcode[2];
    state->pc += 2;
};

void handle_MOVwithMemory(uint8_t* reg, state_8080cpu* state, int direction) {
    uint16_t offset = (state->h << 8) | state-> l;
    direction == 0 ? (*reg = state->memory[offset]) : (state->memory[offset] = *reg);
};

int emulate_8080cpu(state_8080cpu* state) {
    int cycles = 4;
	unsigned char *opcode = &state->memory[state->pc];

	disassemble_opcode(state->memory, state->pc);
	
	state->pc+=1;	

    switch (*opcode) {
        // NOP
        case 0x00: break;
        
        // LXI cases
        case 0x01: handleLXI(&state->b, &state->c, opcode, state); break; // LXI B, word
        case 0x11: handleLXI(&state->d, &state->e, opcode, state); break; // LXI D, word
        case 0x21: handleLXI(&state->h, &state->l, opcode, state); break; // LXI H, word
        case 0x31: // LXI SP, word
            state->sp = (opcode[2] << 8) | opcode[1]; 
            state->pc += 2; 
            break;

        // MOV cases
        case 0x56: handleMOVwithMemory(&state->d, state, 0); break; // MOV D, M
        case 0x5e: handleMOVwithMemory(&state->e, state, 0); break; // MOV E, M
        case 0x66: handleMOVwithMemory(&state->h, state, 0); break; // MOV H, M
        case 0x6f: state->l = state->a; break;      // MOV L,A
        case 0x7a: state->a  = state->d;  break;	// MOV D,A
		case 0x7b: state->a  = state->e;  break;	// MOV E,A
		case 0x7c: state->a  = state->h;  break;	// MOV H,A
        case 0x77: handleMOVwithMemory(&state->a, state, 1); break; // MOV M, A

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