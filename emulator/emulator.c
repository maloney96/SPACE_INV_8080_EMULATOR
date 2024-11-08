#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"
#include "../disassembler/disassembler.h"

void unimplemented_instruction(state_8080cpu *state) {
    state->pc--; // Undo PC increment

    // Display error message along with the disassembled instruction
    fprintf(stderr, "Error: No instruction implemented at address %04x: ", state->pc);
    disassemble_opcode(state->memory, state->pc);  // Show the problematic instruction
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
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

void logic_flags_A(state_8080cpu *state) {
    state->cc.cy = state->cc.ac = 0;
    state->cc.z = (state->a == 0);
    state->cc.s = (0x80 == (state->a & 0x80));
    state->cc.p = parity(state->a, 8);
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

        // MOV cases - MOV DESTINATION, SOURCE
        case 0x56: handle_MOVwithMemory(&state->d, state, 0); break; // MOV D, M
        case 0x5e: handle_MOVwithMemory(&state->e, state, 0); break; // MOV E, M
        case 0x66: handle_MOVwithMemory(&state->h, state, 0); break; // MOV H, M
        case 0x6f: state->l = state->a; break;      				 // MOV L, A
        case 0x77: handle_MOVwithMemory(&state->a, state, 1); break; // MOV M, A
        case 0x7a: state->a = state->d; break;	    				 // MOV A, D
        case 0x7b: state->a = state->e; break;	    				 // MOV A, E
        case 0x7c: state->a = state->h; break;	    				 // MOV A, H
        case 0x7e: handle_MOVwithMemory(&state->a, state, 0); break; // MOV A, M

        // ANA case
        case 0xa7:                                                  // ANA A
            state->a = state->a & state->a;
            logic_flags_A(state);
            break;
        
        // XRA case
        case 0xaf:                                                  // XRA A
            state->a = state->a ^ state->a;
            logic_flags_A(state);
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
        
        // JNZ case
        case 0xc2:
			if (0 == state->cc.z)
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


        // JNC case
        case 0xd2:
            if (0 == state->cc.cy)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            break;

        // JMP case
        case 0xc3: state->pc = (opcode[2] << 8) | opcode[1]; break;

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

        // ANI case
        case 0xe6:
            {
                state->a = state->a & opcode[1];
                logic_flags_A(state);
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

    return 0;
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
