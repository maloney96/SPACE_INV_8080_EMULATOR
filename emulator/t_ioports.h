//
// Created by Colin Cummins on 10/26/24.
//


#ifndef IOPORTS_H
#define IOPORTS_H

#include <stdint.h>
#include <stdlib.h>

// Define a struct to represent the io ports
typedef struct t_ioports {
    //The first two inputs have register bits that are always 'on'
    uint8_t read00 {0b00001110}; //INP0
    uint8_t read01 {0b00001000}; //INP1
    uint8_t read02; //INP2
    uint8_t read03; //SFHT_IN reads result from dedicated bit shift hardware
    uint8_t write02; //SHFT_AMT how many bits to shift in bit shift hardware
    uint8_t write03; //SOUND1 sound bits
    uint8_t write04; //SHFT_DATA writes new value into lower register of bit shift hardware
    uint8_t write05; //SOUND2 which sounds to have sound chip play, also option to rotate for cocktail mode
    uint8_t write06; //WATCHDOG
} t_ioports;

#endif // IOPORTS_H

