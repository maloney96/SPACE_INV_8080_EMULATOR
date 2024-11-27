#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

// Define a struct to represent a memory block
typedef struct {
    int size;        // Size of the memory block
    uint8_t *mem;    // Pointer to the allocated memory
} mem_block_t;

// Function declarations
mem_block_t *create_mem_block(int size); // Allocate a memory block
int load_rom(mem_block_t *mem, const char *file_name); // Load ROM file into memory block
void delete_mem_block(mem_block_t *block); // Free memory block

#ifdef __cplusplus
}
#endif

#endif // MEMORY_H
