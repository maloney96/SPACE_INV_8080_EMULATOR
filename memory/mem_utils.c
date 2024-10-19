#include <stdio.h>
#include "mem_utils.h"
#include <stdint.h>

// void print_ram_contents(uint8_t *ram, size_t size) {
//     for (size_t i = 0; i < size; i++) {
//         if (i % 16 == 0) {
//             printf("\n%04zx: ", i);
//         }
//         printf("%02x ", ram[i]);
//     }
//     printf("\n");
//     fflush(stdout); // Ensure the output is flushed to the console
// }

void print_ram_contents(uint8_t *ram, size_t size) {
    printf("Memory map:\n");
    
    // Loop through the memory and print it based on address ranges
    for (size_t i = 0; i < size; i++) {
        // Print section headers at specific addresses
        if (i == 0x0000) printf("$0000-$07ff: invaders.h\n");
        else if (i == 0x0800) printf("\n\n$0800-$0fff: invaders.g\n");
        else if (i == 0x1000) printf("\n\n$1000-$17ff: invaders.f\n");
        else if (i == 0x1800) printf("\n\n$1800-$1fff: invaders.e\n");
        else if (i == 0x2000) printf("\n\nRAM\n$2000-$23ff: work RAM\n");
        else if (i == 0x2400) printf("\n\n$2400-$3fff: video RAM\n");
        else if (i == 0x4000) printf("\n\n$4000-: RAM mirror\n");

        // Print the memory contents in the required format
        if (i % 16 == 0) {
            printf("\n%04zx: ", i);  // Print the memory address (hexadecimal)
        }
        printf("%02x ", ram[i]);  // Print the memory value in hex
    }

    printf("\n");
    fflush(stdout);  // Ensure the output is flushed to the console
}
