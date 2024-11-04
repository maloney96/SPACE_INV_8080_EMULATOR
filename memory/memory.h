

#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

// Define a struct to represent a memory block
typedef struct {
    int size;
    uint8_t *mem;
} mem_block_t;

// Function declarations
mem_block_t *create_mem_block(int size);
void load_rom(mem_block_t *mem, const char *file_name);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_H
