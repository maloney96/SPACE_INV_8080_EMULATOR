#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "disassembler.h"

int disassemble_opcode(unsigned char *opcode_buffer, int pc) {
    unsigned char *code = &opcode_buffer[pc];
    int opbytes = 1;
    printf("%04x ", pc);

    switch (*code) {
        case 0x00:
        case 0x08: 
        case 0x10: 
        case 0x18: 
        case 0x20: 
        case 0x28: 
        case 0x30: 
        case 0x38:
            printf("NOP");
            break;

        //add more cases...

        return opbytes;
    }
};
