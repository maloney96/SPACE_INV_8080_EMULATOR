#include "memory.h"
#include <stdio.h>
#include <string.h>
#include "../inputmanager/debugwrapper.h"

mem_block_t *create_mem_block(int size) {
    mem_block_t *mem = malloc(sizeof(mem_block_t));
    mem->size = size;
    mem->mem = malloc(sizeof(uint8_t) * size);

    return mem;
}

void load_rom(mem_block_t *mem, const char *file_name) {
    // Open file
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        qdebug_log("Could not open ROM: %s\n", file_name);
        exit(1);
    }
    // Get file size
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);  // Get the size of the file
    fseek(f, 0, SEEK_SET);    // Move file pointer back to the beginning

    // Ensure the memory block is large enough
    if (fsize > mem->size) {
        qdebug_log("ROM size exceeds available memory size.\n");
        fclose(f);
        exit(1);
    }

    // Directly read ROM data into memory block
    fread(mem->mem, fsize, 1, f);

    // Close the file
    fclose(f);
}
