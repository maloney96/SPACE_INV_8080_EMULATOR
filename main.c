#include <stdio.h>
#include <stdlib.h>
#include "memory/memory.h"
#include "memory/mem_utils.h"

#define MEMORY_SIZE 0x10000 // 64KB total memory

mem_block_t *ram;

int main() {

    // TBD INIT 8080 CPU and GUI

    ram = create_mem_block(MEMORY_SIZE); // Allocate memory for the RAM

    // Load the Space Invaders ROM
    load_rom(ram, "invaders/invaders.rom");

    // Print the contents of the RAM
    printf("RAM contents after loading ROM:\n");
    print_ram_contents(ram->mem, MEMORY_SIZE, "RAM_DUMP.txt"); // Print the mem contents for QA

    // Continue with emulator logic...

    return 0;
}