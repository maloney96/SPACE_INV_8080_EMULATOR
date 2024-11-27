# Space Invaders ROM Emulator

### Installation Instructions

1. Clone the repo to your local system.
2. Install QT Creator (https://www.qt.io/download-qt-installer-oss). The team used QT Creator version 14.02 at the time of development.
3. Obtain copy of Space Invaders ROM (four separate files), combine into a single file (cat invaders.h invaders.g invaders.f invaders.e > invaders.rom), and place combined ROM into root directory of cloned project. 
4. Open QT Creator, click "Open Project", select CMakeLists.txt in root of directory.
5. Hit Play button in bottom left corner of UI.
6. Once launched, hit Play Game to launch game. Otherwise, hit Settings to configure game controls or Instructions to see brief overview of game.
7. Enjoy!

### Discussion of Main Files

### Disassembler

Note - Disassembler file is primarily for debugging. Lines involving disassembler within Emulator files are commented out to optimize performance. Only uncomment them to see which instructions are implemented at what timing in the emulator.

1. Reads current instructions from opcode buffer.
2. Inputs current memory address for reference.
3. Decodes instruction based on opcode. 

### Emulator

1. Fetches the current instruction from memory.
2. Disassembles instructions for debugging.
3. Increments program counter and execute instructions. Return error if instruction unimplemented.

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