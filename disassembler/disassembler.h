#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <stddef.h>
#include <stdint.h>

typedef struct condition_codes {    
    uint8_t    z:1;    // Zero flag
    uint8_t    s:1;    // Sign flag
    uint8_t    p:1;    // Parity flag
    uint8_t    cy:1;   // Carry flag
    uint8_t    ac:1;   // Auxiliary carry flag
    uint8_t    pad:3;  // Padding bits to make the struct 1 byte
} condition_codes;    

typedef struct state_8080cpu {    
    uint8_t    a;           // Accumulator
    uint8_t    b;           // B Register
    uint8_t    c;           // C Register
    uint8_t    d;           // D Register
    uint8_t    e;           // E Register
    uint8_t    h;           // H Register
    uint8_t    l;           // L Register
    uint16_t   sp;          // Stack Pointer (16-bit)
    uint16_t   pc;          // Program Counter (16-bit)
    uint8_t    *memory;     // Pointer to 64KB memory block    
    condition_codes cc;     // Condition Codes (status flags)
    uint8_t     int_enable; // Interrupt Enable/Disable flag
} state_8080cpu;

int disassemble_opcode(unsigned char *opcodebuffer, int pc);

#endif // DISASSEMBLER_H