/**
 * @defgroup muh_game Muhlbaier's Game
 * @ingroup games
 *
 * This game demonstrates using the Task Management Module
 *
 * Note: the map is drawn with extended ASCII chars, to view it properly
 * change translation in PuTTY:
 * Change Settings -> Window -> Translation
 * Then set the Remote character set to CP866
 *
 * The game is a shooter, there will be enemies coming from the top
 * and if they get to the bottom it is game over. You get up to 5
 * simultaneous shots then you will have to wait for a shot to hit an
 * enemy or go off the screen before you can shoot again. When your
 * indicator at the bottom is '^' it means you are ready to shoot
 * if it is 'o' you are not ready to shoot.
 *
 * When you hit an enemy the bell dings and a massive explosion happens
 * indicated by '*'. The '*' will remain until a shot or enemy pass over it.
 *
 * "$game MUH play" will start the game.
 * "$game MUH help" will print information about the game.
 *
 * Enjoy!
 *
 * @warning This game runs best at 16MHz or higher. See @ref hal_clock
 *
 *  Created on: Mar 29, 2014
 *      Author: Muhlbaier
 *
 *  @{
 */

#ifndef MUH_GAME_H_
#define MUH_GAME_H_

/** MUH_Init must be called before the game can be played
 *
 */
void MuhGame_Init(void);

/** @} */

#endif /* MUH_GAME_H_ */
