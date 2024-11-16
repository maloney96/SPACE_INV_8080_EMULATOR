//
// Created by Colin Cummins on 10/20/24.
// Defines default input mapping for emulator
//
// Updated by Ian McCubbin, 10/22/24
//  - RENAMED FIRE_BUTTON to FIRE
//  - RENAMED LEFT_JOYSTICK to LEFT
//  - RENAMED RIGHT_JOYSTICK to RIGHT
//  - ADDED EXIT_GAME CHARACTER (as ESCape button)
//  - ADDED COMMENTS TO DEFINITIONS TO TRANSLATE TO KEYBOARD
//
// Updated by Ian McCubbin, 10/29/24
//  - REMAPPED LEFT, RIGHT, INSERT_COIN TO MORE UNIVERSAL LAYOUT
//  - RENAMED FIRE_BUTTON to DEFAULT_FIRE
//  - RENAMED LEFT_JOYSTICK to DEFAULT_LEFT
//  - RENAMED RIGHT_JOYSTICK to DEFAULT_RIGHT
//  - RENAMED P1_BUTTON to DEFAULT_P1
//  - RENAMED P2_BUTTON to DEFAULT_P2
//  - RENAMED INSERT_COIN to DEFAULT_INSERT_COIN
//  - RENAMED EXIT_GAME to DEFAULT_EXIT


#ifndef KEYMAP_H
#define KEYMAP_H

// Basic control inputs from keyboard. The player facing controls on a cabinet.
#define DEFAULT_FIRE Qt::Key_Space              // SPACEBAR
#define DEFAULT_LEFT Qt::Key_A                  // 'A'
#define DEFAULT_RIGHT Qt::Key_D                 // 'D'
#define DEFAULT_P1_BUTTON Qt::Key_1             // '1'
#define DEFAULT_P2_BUTTON Qt::Key_2             // '2'
#define DEFAULT_INSERT_COIN Qt::Key_Shift       // SHIFT 'SHIFT'
#define DEFAULT_EXIT Qt::Key_Escape             // ESCAPE 'ESC'

// Game settings. Equivalent to internal dipswitches on a cabinet.
// We are using ascii codes for keys not usually on a keyboard

//Set number of lives
#define LIVES_3 0x0a3
#define LIVES_4 0x0a4
#define LIVES_5 0x0a5
#define LIVES_6 0x0a6

//Set score threshold for extra life
#define BONUS_LIFE_1000 0x0a7
#define BONUS_LIFE_1500 0x0a8

#endif //KEYMAP_H
