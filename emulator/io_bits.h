//
// Created by Colin Cummins on 10/27/24.
//

#ifndef IO_BITS_H
#define IO_BITS_H

// Port 0 Bits are defined in hardware but not used

// Input Port 1
#define CREDIT  0b00000001
#define P2START 0b00000010
#define P1START 0b00000100
#define UNUSED  0b00001000 //Unused bit
#define P1SHOT  0b00010000
#define P1LEFT  0b00100000
#define P1RIGHT 0b01000000
// Port 1 Bit 7 not connected

// Input Port 2
#define DIPSW3  0b00000001
#define DIPSW5  0b00000010
#define TILT    0b00000100
#define DIPSW6  0b00001000
#define P2SHOT  0b00010000
#define P2LEFT  0b00100000
#define P2RIGHT 0b01000000
#define CINFO   0b10000000

// Output Port 2 instructs shifter how many bits to shift

// Output port 3
// Sounds
#define UFO         0b00000001 // bit 0=UFO (repeats)        SX0 0.raw
#define SHOTS       0b00000010 // bit 1=Shot                 SX1 1.raw
#define PLAYER_DIE  0b00000100 // bit 2=Flash (player die)   SX2 2.raw
#define INVADER_DIE 0b00001000 // bit 3=Invader die          SX3 3.raw
#define EXTEND_PLAY 0b00010000 // bit 4=Extended play        SX4
#define AMP_ON      0b00100000 // bit 5= AMP enable          SX5
// Port 3 Bit 6 not connected
// Port 3 Bit 7 not connected

//Output port 4
//Port 4: (discrete sounds)
//bit 0-7 shift data (LSB on 1st write, MSB on 2nd)


// Output port 5
// Sounds and misc
#define FLEET1      0b00000001 // bit 0=Fleet movement 1     SX6 4.raw
#define FLEET2      0b00000010 // bit 1=Fleet movement 2     SX7 5.raw
#define FLEET3      0b00000100 // bit 2=Fleet movement 3     SX8 6.raw
#define FLEET4      0b00001000 // bit 3=Fleet movement 4     SX9 7.raw
#define UFO_HIT     0b00010000 // bit 4=UFO Hit              SX10 8.raw
#define COCKTAIL    0b00100000 // bit 5=Cocktail Mode - flip screen
// Port 5 Bit 6 not connected
// Port 5 Bit 7 not connected

//Output port 6
//Watchdog - read or write to reset


#endif //IO_BITS_H
