#include "memory.h"
#include <stdio.h>
#include <string.h>
#include "../inputmanager/debugwrapper.h"

mem_block_t *create_mem_block(int size) {
    if (size <= 0) {
        qdebug_log("Invalid memory size requested: %d\n", size);
        return NULL;
    }

    mem_block_t *mem = malloc(sizeof(mem_block_t));
    if (!mem) {
        qdebug_log("Failed to allocate memory for mem_block_t\n");
        return NULL;
    }

    mem->mem = malloc(sizeof(uint8_t) * size);
    if (!mem->mem) {
        qdebug_log("Failed to allocate memory block of size: %d\n", size);
        free(mem);  // Free the structure if memory allocation fails
        return NULL;
    }

    mem->size = size;
    return mem;
}

int load_rom(mem_block_t *mem, const char *file_name) {
    if (!mem || !mem->mem || mem->size <= 0) {
        qdebug_log("Invalid memory block passed to load_rom\n");
        return -1;
    }

    // Open the ROM file
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        qdebug_log("Could not open ROM file: %s\n", file_name);
        return -1;
    }

    // Get the size of the ROM file
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Ensure the ROM fits in the memory block
    if (fsize > mem->size) {
        qdebug_log("ROM size (%zu bytes) exceeds available memory size (%d bytes)\n", fsize, mem->size);
        fclose(f);
        return -1;
    }

    // Read the ROM data into memory
    fread(mem->mem, 1, fsize, f);

    // Close the ROM file
    fclose(f);

    qdebug_log("ROM loaded successfully: %s\n", file_name);
    return 0;
}

void delete_mem_block(mem_block_t *block) {
    if (block) {
        if (block->mem) {
            free(block->mem);  // Free the allocated memory
            block->mem = NULL; // Nullify the pointer
        }
        free(block);  // Free the structure
        block = NULL; // Nullify the structure pointer
    }
}
