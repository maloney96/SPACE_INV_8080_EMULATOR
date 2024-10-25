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
        const char *mov_cases[] = {
            "MOV B,B", "MOV B,C", "MOV B,D", "MOV B,E", "MOV B,H", "MOV B,L", "MOV B,M", "MOV B,A",
            "MOV C,B", "MOV C,C", "MOV C,D", "MOV C,E", "MOV C,H", "MOV C,L", "MOV C,M", "MOV C,A",
            "MOV D,B", "MOV D,C", "MOV D,D", "MOV D,E", "MOV D,H", "MOV D,L", "MOV D,M", "MOV D,A",
            "MOV E,B", "MOV E,C", "MOV E,D", "MOV E,E", "MOV E,H", "MOV E,L", "MOV E,M", "MOV E,A",
            "MOV H,B", "MOV H,C", "MOV H,D", "MOV H,E", "MOV H,H", "MOV H,L", "MOV H,M", "MOV H,A",
            "MOV L,B", "MOV L,C", "MOV L,D", "MOV L,E", "MOV L,H", "MOV L,L", "MOV L,M", "MOV L,A",
            "MOV M,B", "MOV M,C", "MOV M,D", "MOV M,E", "MOV M,H", "MOV M,L", "HLT",    "MOV M,A",
            "MOV A,B", "MOV A,C", "MOV A,D", "MOV A,E", "MOV A,H", "MOV A,L", "MOV A,M", "MOV A,A"
        };

        const char *add_cases[] = {
            "ADD B", "ADD C", "ADD D", "ADD E", "ADD H", "ADD L", "ADD M", "ADD A",
        };

        const char *adc_cases[] = {
            "ADC B", "ADC C", "ADC D", "ADC E", "ADC H", "ADC L", "ADC M", "ADC A"
        };

        const char *sub_cases[] = {
            "SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB M", "SUB A",
        };

        const char *sbb_cases[] = {
            "SBB B", "SBB C", "SBB D", "SBB E", "SBB H", "SBB L", "SBB M", "SBB A"
        };

        const char *ana_cases[] = {
           "ANA B", "ANA C", "ANA D", "ANA E", "ANA H", "ANA L", "ANA M", "ANA A",
        };

        const char *xra_cases[] = {
            "XRA B", "XRA C", "XRA D", "XRA E", "XRA H", "XRA L", "XRA M", "XRA A"
        };

        const char *ora_cases[] = {
            "ORA B", "ORA C", "ORA D", "ORA E", "ORA H", "ORA L", "ORA M", "ORA A"
        };

        const char *cmp_cases[] = {
            "CMP B", "CMP C", "CMP D", "CMP E", "CMP H", "CMP L", "CMP M", "CMP A"
        };

        // No operation cases
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

        // LXI cases
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
            printf("LXI %s,#$%02x%02x", 
            (*code == 0x01) ? "B" : (*code == 0x11) ? "D" : (*code == 0x21) ? "H" : "SP",
            code[2],
            code[1]);
            opbytes = 3;
            break;
        
        // STAX cases
        case 0x02:
        case 0x12:
            printf("STAX %s", 
            (*code == 0x02) ? "B" : "D");
            break;
        
        // INX cases
        case 0x03:
        case 0x13:
        case 0x23:
        case 0x33:
            printf("INX %s", 
            (*code == 0x03) ? "B" : (*code == 0x13) ? "D" : (*code == 0x23) ? "H" : "SP");
            break;
        
        // INR cases
        case 0x04:
        case 0x0c:
        case 0x14:
        case 0x1c:
        case 0x24:
        case 0x2c:
        case 0x34:
        case 0x3c:
            printf("INR %s", 
            (*code == 0x04) ? "B" : (*code == 0x0c) ? "C" : (*code == 0x14) ? "D" : (*code == 0x1c) ? "E" : 
            (*code == 0x24) ? "H" : (*code == 0x2c) ? "L" : (*code == 0x34) ? "M" : "SP");
            break;
        
        // DCR cases
        case 0x05:
        case 0x0d:
        case 0x15:
        case 0x1d:
        case 0x25:
        case 0x2d:
        case 0x35:
        case 0x3d:
            printf("DCR %s", 
            (*code == 0x05) ? "B" : (*code == 0x0d) ? "C" : (*code == 0x15) ? "D" : (*code == 0x1d) ? "E" : 
            (*code == 0x25) ? "H" : (*code == 0x2d) ? "L" : (*code == 0x35) ? "M" : "SP");
            break;
        
        // MVI cases
        case 0x06:
        case 0x0e:
        case 0x16:
        case 0x1e:
        case 0x26:
        case 0x2e:
        case 0x36:
        case 0x3e:
            printf("MVI %s,#$%02x",
            (*code == 0x06) ? "B" : (*code == 0x0e) ? "C" : (*code == 0x16) ? "D" : (*code == 0x1e) ? "E" : 
            (*code == 0x26) ? "H" : (*code == 0x2e) ? "L" : (*code == 0x36) ? "M" : "SP",
            code[1]);
            opbytes = 2;
            break;
        
        // DAD cases
        case 0x09:
        case 0x19:
        case 0x29:
        case 0x39:
            printf("DAD %s", 
            (*code == 0x09) ? "B" : (*code == 0x19) ? "D" : (*code == 0x29) ? "H" : "SP");
            break;
        
        // LDAX cases
        case 0x0a:
        case 0x1a:
            printf("LDAX %s", 
            (*code == 0x0a) ? "B" : "D");
            break;

        // DCX cases
        case 0x0b:
        case 0x1b:
        case 0x2b:
        case 0x3b:
            printf("DCX %s", 
            (*code == 0x0b) ? "B" : (*code == 0x1b) ? "D" : (*code == 0x2b) ? "H" : "SP");
            break;

        // One-off cases between 0x00 - 0x40:
        case 0x22: printf("SHLD $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x32: printf("STA  $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x07: printf("RLC"); break;
        case 0x17: printf("RAL"); break;
        case 0x27: printf("DAA"); break;
        case 0x37: printf("STC"); break;
        case 0x0f: printf("RRC"); break;
        case 0x1f: printf("RAR"); break;
        case 0x2a: printf("LHLD $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x2f: printf("CMA"); break;
        case 0x3a: printf("LDA $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x3f: printf("CMC"); break;

        // MOV cases
        case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4a:
		case 0x4b:
		case 0x4c:
		case 0x4d:
		case 0x4e:
		case 0x4f:
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x56:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5a:
		case 0x5b:
		case 0x5c:
		case 0x5d:
		case 0x5e:
		case 0x5f:
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
		case 0x6c:
		case 0x6d:
		case 0x6e:
		case 0x6f:
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x79:
		case 0x7a:
		case 0x7b:
		case 0x7c:
		case 0x7d:
		case 0x7e:
		case 0x7f:
            printf("%s", mov_cases[*code - 0x40]);
            break;

        // ADD cases
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
            printf("%s", add_cases[*code - 0x80]);
            break;
        
        // ADC cases
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
            printf("%s", adc_cases[*code - 0x88]);
            break;
        
        // SUB cases
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
            printf("%s", sub_cases[*code - 0x90]);
            break;
        
        // SBB cases 
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
            printf("%s", sbb_cases[*code - 0x98]);
            break;
        
        // ANA cases
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
            printf("%s", ana_cases[*code - 0xa0]);
            break;

        // XRA cases
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
            printf("%s", xra_cases[*code - 0xa8]);
            break;

        // ORA cases
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
            printf("%s", ora_cases[*code - 0xb0]);
            break;

        // CMP cases
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
            printf("%s", cmp_cases[*code - 0xb8]);
            break; 
        
        // POP cases
        case 0xc1:
        case 0xd1:
        case 0xe1:
        case 0xf1:
            printf("POP %s", 
            (*code == 0xc1) ? "B" : (*code == 0xd1) ? "D" : (*code == 0xe1) ? "H" : "SP");
            break;
        
        // PUSH cases
        case 0xc5:
        case 0xd5:
        case 0xe5:
        case 0xf5:
            printf("PUSH %s", 
            (*code == 0xc5) ? "B" : (*code == 0xd5) ? "D" : (*code == 0xe5) ? "H" : "SP");
            break;
        
        // RST cases
        case 0xc7:
        case 0xcf:
        case 0xd7:
        case 0xdf:
        case 0xe7:
        case 0xef:
        case 0xf7:
        case 0xff:
            printf("RST %s",
            (*code == 0xc7) ? "0" : (*code == 0xcf) ? "1" : (*code == 0xd7) ? "2" : (*code == 0xdf) ? "3" : 
            (*code == 0xe7) ? "4" : (*code == 0xef) ? "5" : (*code == 0xf7) ? "6" : "7");
            break;
        
        // Cases printing code[2], code[1] and setting opbytes to 3
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xca:
        case 0xcb:
        case 0xcc:
        case 0xcd:
        case 0xd2:
        case 0xd4:
        case 0xda:
        case 0xdc:
        case 0xdd:
        case 0xe2:
        case 0xe4:
        case 0xea:
        case 0xec:
        case 0xed:
        case 0xf2:
        case 0xf4:
        case 0xfa:
        case 0xfc:
        case 0xfd:
            printf("%s $%02x%02x", 
            (*code == 0xc2) ? "JNZ" : (*code == 0xc3) ? "JMP" : (*code == 0xc4) ? "CNZ" : (*code == 0xca) ? "JZ" :
            (*code == 0xcb) ? "JMP" : (*code == 0xcc) ? "CZ" : (*code == 0xcd) ? "CALL" : (*code == 0xd2) ? "JNC" :
            (*code == 0xd4) ? "CNC" : (*code == 0xda) ? "JC" : (*code == 0xdc) ? "CC" : (*code == 0xdd) ? "CALL" :
            (*code == 0xe2) ? "JPO" : (*code == 0xe4) ? "CPO" : (*code == 0xea) ? "JPE" : (*code == 0xec) ? "CPE" :
            (*code == 0xed) ? "CALL" : (*code == 0xf2) ? "JP" : (*code == 0xf4) ? "CP" : (*code == 0xfa) ? "JM" :
            (*code == 0xfc) ? "CM" : "CALL",
            code[2], code[1]);
            opbytes = 3;
            break;
        
        // Cases printing code[1] and setting opbytes to 2
        case 0xc6:
        case 0xce:
        case 0xd3:
        case 0xd6:
        case 0xdb:
        case 0xde:
        case 0xe6:
        case 0xee:
        case 0xf6:
        case 0xfe:
            printf("%s $%02x", 
            (*code == 0xc6) ? "ADI" : (*code == 0xce) ? "ACI" : (*code == 0xd3) ? "OUT" : (*code == 0xd6) ? "SUI" :
            (*code == 0xdb) ? "IN" : (*code == 0xde) ? "SBI" : (*code == 0xe6) ? "ANI" : (*code == 0xee) ? "XRI" :
            (*code == 0xf6) ? "ORI" : "CPI",
            code[1]);
            opbytes = 2;
            break;

        // One-off cases between 0xc0 - 0xff:
        case 0xc0: printf("RNZ"); break;
        case 0xc8: printf("RZ"); break;
		case 0xc9: printf("RET"); break;
        case 0xd0: printf("RNC"); break;
        case 0xd8: printf("RC");  break;
		case 0xd9: printf("RET"); break;
        case 0xe0: printf("RPO"); break;
        case 0xe3: printf("XTHL"); break;
        case 0xe8: printf("RPE"); break;
		case 0xe9: printf("PCHL"); break;
        case 0xeb: printf("XCHG"); break;
        case 0xf0: printf("RP");  break;
        case 0xf3: printf("DI");  break;
        case 0xf8: printf("RM");  break;
		case 0xf9: printf("SPHL"); break;
        case 0xfb: printf("EI");  break;
    }

    return opbytes;
};
