#include <stdio.h>
#include "mem_utils.h"
#include <stdint.h>
#include "../inputmanager/debugwrapper.h"

void print_ram_contents(uint8_t *ram, size_t size, const char *filename) {
    FILE *output = stdout;
    if (filename != NULL) {
        output = fopen(filename, "w");
        if (output == NULL) {
            perror("Failed to open file");
            return;
        }
    }

    qdebug_log("Memory map:\n");
    
    // Loop through the memory and print it based on address ranges
    for (size_t i = 0; i < size; i++) {
        // Print section headers at specific addresses
        if (i == 0x0000) fprintf(output, "$0000-$07ff: invaders.h\n");
        else if (i == 0x0800) fprintf(output, "\n\n$0800-$0fff: invaders.g\n");
        else if (i == 0x1000) fprintf(output, "\n\n$1000-$17ff: invaders.f\n");
        else if (i == 0x1800) fprintf(output, "\n\n$1800-$1fff: invaders.e\n");
        else if (i == 0x2000) fprintf(output, "\n\nRAM\n$2000-$23ff: work RAM\n");
        else if (i == 0x2400) fprintf(output, "\n\n$2400-$3fff: video RAM\n");
        else if (i == 0x4000) fprintf(output, "\n\n$4000-: RAM mirror\n");

        // Print the memory contents in the required format
        if (i % 16 == 0) {
            fprintf(output, "\n%04zx: ", i);  // Print the memory address (hexadecimal)
        }
        fprintf(output, "%02x ", ram[i]);  // Print the memory value in hex
    }

    fprintf(output, "\n");
    fflush(output);  // Ensure the output is flushed to the console or file

    if (filename != NULL) {
        fclose(output);
    }
}
