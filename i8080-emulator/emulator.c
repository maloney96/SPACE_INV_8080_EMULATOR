#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"
#include "disassembler.h"

void unimplemented_instruction(state_8080cpu* state) {
    state->pc--; // Undo PC increment

    // Display error message along with the disassembled instruction
    fprintf(stderr, "Error: No instruction implemented at address %04x: ", state->pc);
    disassemble_opcode(state->memory, state->pc);  // Show the problematic instruction
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
};

int emulate_8080cpu(state_8080cpu* state) {
    int cycles = 4;
	unsigned char *opcode = &state->memory[state->pc];

	disassemble_opcode(state->memory, state->pc);
	
	state->pc+=1;	

    switch (*opcode) {
        case 0x00: break; //NOP
        case 0x01:
            state->c = opcode[1];
            state->b = opcode[2];
            state->pc += 2;
            break;
        case 0x02:
        case 0x03:
        case 0x04:
            unimplemented_instruction(state);
            break;
        
        // Add more cases...
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