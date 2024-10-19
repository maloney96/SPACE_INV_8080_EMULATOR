#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdlib.h>

// Define memory map regions
// #define ROM_START 0x0000
// #define ROM_END 0x1FFF
// #define RAM_START 0x2000
// #define RAM_END 0x3FFF

// Define a struct to represent a memory block
typedef struct {
    int size;
    uint8_t *mem;
} mem_block_t;

// Function declarations
mem_block_t *create_mem_block(int size);
void load_rom(mem_block_t *mem, const char *file_name);

#endif // MEMORY_H