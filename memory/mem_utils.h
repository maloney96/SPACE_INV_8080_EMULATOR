#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <stddef.h>
#include <stdint.h>

// Function declarations
void print_ram_contents(uint8_t *ram, size_t size, const char *filename);

#endif // MEM_UTILS_H

#ifdef __cplusplus
}
#endif
