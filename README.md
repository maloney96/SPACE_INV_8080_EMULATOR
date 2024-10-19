# SPACE_INV_8080_EMULATOR

### Memory

Rom is created by combining all invaders files within the /invaders folder
cat invaders.h invaders.g invaders.f invaders.e > invaders.rom

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