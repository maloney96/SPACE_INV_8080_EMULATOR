#ifndef KEYMAP_H
#define KEYMAP_H

/**
 * @file keymap.h
 * @brief Defines default input mappings for the emulator.
 *
 * This header file establishes the default key bindings for various emulator controls,
 * including player actions and game settings. It maps specific keyboard keys to emulator
 * functions, facilitating user interaction.
 *
 * @author Colin Cummins
 * @date Created on 10/20/24
 *
 * @author Ian McCubbin
 * @date Updated on 10/22/24
 * - Renamed FIRE_BUTTON to FIRE
 * - Renamed LEFT_JOYSTICK to LEFT
 * - Renamed RIGHT_JOYSTICK to RIGHT
 * - Added EXIT_GAME character (mapped to ESC key)
 * - Added comments to definitions to translate to keyboard
 *
 * @date Updated on 10/29/24
 * - Remapped LEFT, RIGHT, INSERT_COIN to a more universal layout
 * - Renamed FIRE_BUTTON to DEFAULT_FIRE
 * - Renamed LEFT_JOYSTICK to DEFAULT_LEFT
 * - Renamed RIGHT_JOYSTICK to DEFAULT_RIGHT
 * - Renamed P1_BUTTON to DEFAULT_P1
 * - Renamed P2_BUTTON to DEFAULT_P2
 * - Renamed INSERT_COIN to DEFAULT_INSERT_COIN
 * - Renamed EXIT_GAME to DEFAULT_EXIT
 */

/**
 * @def DEFAULT_FIRE
 * @brief Default key mapping for the fire action.
 *
 * Mapped to the SPACEBAR key.
 */
#define DEFAULT_FIRE Qt::Key_Space              // SPACEBAR

/**
 * @def DEFAULT_LEFT
 * @brief Default key mapping for moving left.
 *
 * Mapped to the 'A' key.
 */
#define DEFAULT_LEFT Qt::Key_A                  // 'A'

/**
 * @def DEFAULT_RIGHT
 * @brief Default key mapping for moving right.
 *
 * Mapped to the 'D' key.
 */
#define DEFAULT_RIGHT Qt::Key_D                 // 'D'

/**
 * @def DEFAULT_P1_BUTTON
 * @brief Default key mapping for Player 1's action button.
 *
 * Mapped to the '1' key.
 */
#define DEFAULT_P1_BUTTON Qt::Key_1             // '1'

/**
 * @def DEFAULT_P2_BUTTON
 * @brief Default key mapping for Player 2's action button.
 *
 * Mapped to the '2' key.
 */
#define DEFAULT_P2_BUTTON Qt::Key_2             // '2'

/**
 * @def DEFAULT_INSERT_COIN
 * @brief Default key mapping for inserting a coin.
 *
 * Mapped to the SHIFT key.
 */
#define DEFAULT_INSERT_COIN Qt::Key_Shift       // SHIFT

/**
 * @def DEFAULT_EXIT
 * @brief Default key mapping for exiting the game.
 *
 * Mapped to the ESCAPE key.
 */
#define DEFAULT_EXIT Qt::Key_Escape             // ESCAPE

// Some other default settings to store:

/** @def DEFAULT_EXTRA_LIVES
 *  @brief Default number of extra lives
 */
#define DEFAULT_EXTRA_LIVES 3

/** @def DEFAULT_EXTRA_LIFE_SCORE
 *  @brief Default score for an extra life
 */
#define DEFAULT_EXTRA_LIFE_AT 1000

#endif // KEYMAP_H
