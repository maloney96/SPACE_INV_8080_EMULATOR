# SPACE_INV_8080_EMULATOR

### Disassembler

1. Read current instructions from opcode buffer.

2. Input current memory address for reference.

3. Decode instruction based on opcode. 

### Emulator

1. Fetch the current instruction from memory.

2. Disassemble instructions for debugging.

3. Increment program counter and execute instructions. Return error if instruction unimplemented.

### Memory

1. Place all invaders source files into the invaders folder..
cd into the invaders folder and run the following..
cat invaders.h invaders.g invaders.f invaders.e > invaders.rom

2. cd .. into SPACE_INV_8080_EMULATOR and run make command

3. run ./emulator

4. Memory MAP with contents dumped via txt file "RAM_DUMP.txt" 

Memory map:
	ROM
	$0000-$07ff:	invaders.h
	$0800-$0fff:	invaders.g
	$1000-$17ff:	invaders.f
	$1800-$1fff:	invaders.e
	
	RAM
	$2000-$23ff:	work RAM
	$2400-$3fff:	video RAM
	
	$4000-:		RAM mirror
