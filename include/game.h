#ifndef _GAME_H_
#define _GAME_H_

#include "project_settings.h"

#ifndef USE_MODULE_BUFFER_PRINTF
#error "This module requires optional UART functionality provided by buffer_printf. Please declare USE_MODULE_BUFFER_PRINTF in project_settings.h"
#endif

#include "terminal.h"

#ifdef USE_MODULE_GAME_CONTROLLER
#include "game_controller.h"
#endif

/** @brief Game Management / Helper Module
 *
 * @defgroup game Game Management / Helper Module
 *
 * This module helps manage multiple game modules running on a single system as
 * well as implementing several helper functions and structures.
 *
 * Command list:
 * - $game help (print help info)
 * - $game player1 {name} [color] [skill] (set player1's name [optional color and skill])
 * - $game player2 {name} [color] [skill] (set player2's name [optional color and skill])
 * name can be up to 15 characters, color can be red, green, yellow, blue, magenta, cyan
 * white, skill can be 0 to 9.
 * - $game {game_name} play (play game_name)
 * - $game {game_name} help (show help for game_name)
 * - $game {game_name} score (show highscores for game_name)
 * - $game {game_name} var1 var2... (call callback for game_name with argc, argv)
 * this can be used to implement changing settings or other more advanced
 * features that are not supported by default.
 *
 * Notes for developing a game using this game management module:
 * - Create a function to initiate playing the game (i.e. Play()). The Play
 * function will typically initialize the states, score, screen, etc. then
 * register a receiver using Game_RegisterPlayer1Receiver() or
 * Game_RegisterPlayer2Receiver() and it can shedule tasks to run the game etc.
 * When the game is over you should be sure to ungregister the receiver and
 * remove any tasks you still have scheduled.
 * - Create a function to print instructions and other information about your game (i.e. Help())
 * - If you use generic names like Play() or Help() or anything else that someone
 * else might use then declare the function to be static so it is limited in
 * scope to your game file "static void Play(void);"
 * - Create a [game_name]_Init() function which in most cases will be your only
 * public function and should be called from main,
 * - [game_name]_Init() should at a minimum call Game_Register() and can optionally
 * call Game_RegisterCallback() and/or Game_RegisterHighscoreCallback().
 *
 * @author Michael Muhlbaier
 * @todo MM add "$game player1 ..." and "$game player2 ..." command support
 * @todo MM implement transparent character support
 *
 * @{
 */

/** @brief Char Object
 *
 * Reusable char object type to track a single char's position and status.
 *
 * @warning JK 10/10
 */
typedef struct {
	char c; ///< Displayed character ("sprite")
	char x; ///< x position (increasing from left to right)
	char y; ///< y position (increasing from top to bottom)
	char status; ///< Status of object. Usage depends on implementation.
} char_object_t;

/** @struct player_info_t
 *
 * @brief Player info structure to hold player settings
 *
 * Holds values for the players name, a color setting for the player, and a skill level
 *
 * @warning JL 10/10
 */
typedef struct {
    char name[16]; ///< name of player
    enum term_color color; ///< color setting for player
    uint8_t skill; ///< 0-9 skill level of player
}player_info_t;

#ifndef TRANSPARENT_CHAR
#define TRANSPARENT_CHAR '`' ///< transparent character for drawing tiles
#endif

/** @brief Game_Register registers a game by adding it to the games[] array
 *
 * Game_Register will take input pointers and copy it onto the corresponding variable 
 * pointers in game_t struct. Will also initialize num_players, score, and callback.
 *
 * @param name - Pointer to start of character array containing name of game
 * @param description - Pointer to start of character array containing description of game
 * @param play - Pointer to function that initiates playing the game
 * @param help - Pointer to function that displays help info
 * @return returns 0xFF if games[] array is at MAX_GAMES length
 * Otherwise returns index of registered game in games[] array
 *
 */
uint8_t Game_Register(char * name, char * description,
    void (*play)(void),
    void (*help)(void));

/**
 * This function takes in an id for the game along with the callback,
 * and saves it in the array of games. The index of the array the
 * callback is stored in is the game_id, so this number must be less
 * than the size of the games array.
 *
 * @param game_id ID of the game. Also used as the array index.
 * @param callback The callback pointer
 *
 * @warning JR 10/10
 */
void Game_RegisterCallback(uint8_t game_id,
    void (*callback)(int argc, char *argv[]));

/**
 *
 * @param game_id
 * @param callback
 *
 * @todo Andrew R. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_RegisterHighscoreCallback(uint8_t game_id,
        void(*callback)(void));

/**
 *
 * @param game_id ID of game (returned when Game_Register() is called)
 * @param str pointer to null terminated string
 * @param ... variable argument list to be used with any replacement flags in the string
 * 
 * Similar to LogMsg() except with the addition of the game name to the message.
 *
 * @warning CR 7/10
 *
 */
void Game_Log(uint8_t game_id, char * str, ...);

/**
 *
 * @param game_id
 * @param num
 *
 * @todo Jon W. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_EnableMultiPlayer(uint8_t game_id, uint8_t num);

/** Game_RegisterPlayer1Receiver will register the UART char receiver function for player 1
 *
 * Game_RegisterPlayer1Receiver registers the UART char receiver function
 * for player 1. PLAYER1_UART defines the channel of the UART to be registered.
 *
 * @param rx Pointer to UART char receiver function
 *
 * @warning Make sure to Game_UnregisterPlayer1Receiver() when game play is over
 *
 * @warning JWi 10/10
 */
void Game_RegisterPlayer1Receiver(void(*rx)(uint8_t));

/**
 * @brief Register Player 2 UART Channel
 *
 * Game_RegisterPlayer2Receiver() registers a second UART channel to be used by a second player. Only use if you game
 * allows a for a second player
 *
 * @param rx is a function pointer that calls a function when a character is received in the UART channel
 * 
 * @warning Make sure to Game_UnregisterPlayer2Receiver() when game play is over
 * @warning TG 10/10
 */
void Game_RegisterPlayer2Receiver(void(*rx)(uint8_t));

/**
 * @brief Unregister Player 1 UART Channel receiver
 * 
 * Game_UnregisterPlay1Receiver() unregisters the UART channel associated with player 1. 
 
 * @param rx is a function pointer that calls a function when a character is received in the UART channel
 *
 * @warning TG 10/10
 */
void Game_UnregisterPlayer1Receiver(void(*rx)(uint8_t));

/** @brief Unregister Player 2 UART Channel receiver
 *
 * Game_UnregisterPlayer2Receiver() unregisters the UART channel that has been assigned to Player 2.
 *
 * @param rx is a function pointer that calls the desired function when the UART channel receives a character.
 *
 * @warning MM 9/10
 */
void Game_UnregisterPlayer2Receiver(void(*rx)(uint8_t));

/** @brief Write a character to a coordinate of the terminal
 *
 * Game_CharXY writes a character to the terminal. If there are multiple players it will write to all available terminals
 *
 * @param c is the character you wish to print to the terminal
 * @param x defines where to print the character on the x axis
 * @param y defines where to print the character on the y axis
 *
 * @warning MM 9/10
 */
void Game_CharXY(char c, char x, char y);

/**
 *
 * @param c
 * @param x
 * @param y
 *
 * @todo Anthony M. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_Player1CharXY(char c, char x, char y);

/**
 *
 * @param c
 * @param x
 * @param y
 *
 * @todo Anthony M. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_Player2CharXY(char c, char x, char y);

/**
 *
 * @param str pointer to string to be printed (must be null terminated)
 * @param ... variable argument list to be used with any replacement flags in the string
 *
 * Function: Game_Printf(*str)
 *
 * This function is passed a pointer to a string and prints that string to the terminal using UART_vprintf.
 * The function supports printf style replacement flags.  Can output messages from
 * multiple UART channels if more than one person is playing.
 *
 * @warning TM 9/10
 */
void Game_Printf(char * str, ...);

/**
 *
 * @param str pointer to string to be printed (must be null terminated)
 * @param ... variable argument list to be used with any replacement flags in the string
 *
 * Function: Game_Printf(*str)
 *
 * This function is passed a pointer to a string and prints that string to player 1's terminal using UART_vprintf.
 * The function supports printf style replacement flags.
 *
 * @warning TM 9/10
 */
void Game_Player1Printf(char * str, ...);

/** Sending a Player 2 specific message
 *
 * @param str is a string which is to be printed to player 2's screen.
 * @param ... variable argument list to be used with any replacement flags in the string
 * 
 * @code
 * Game_Player2Printf("Your score is: %d", score);
 * @endcode
 * 
 * This function is passed a pointer to a string and prints that string to player 2's terminal using UART_vprintf.
 * The function supports printf style replacement flags.
 *
 * @warning RT 8/10
 */
void Game_Player2Printf(char * str, ...);

/** @brief Hide the cursor for player 1 (and player2 if a two player game) 
 * 
 * Hide the cursor and, if echo is on, turn echo off.
 */
void Game_HideCursor(void);

/** @brief Show the cursor for player 1 (and player2 if a two player game) 
 *
 * Show the cursor and, if echo was on, turn echo back on.
 */
void Game_ShowCursor(void);

/**
 *
 * This function sets the color for the game and can be either the foreground or the background depending on the user input.
 * If there is a second channel, it will also set the corresponding color specifications to the second UART channel.
 * 
 * @param color is one of the elements within the enumeration defined in terminal.h.  
 *
 * @warning RT 10/10
 */
void Game_SetColor(enum term_color color);

/**
 * Convenience function for writing a tile (2D array of characters) to the screen at a specified X, Y coordinate.
 * This function maps characters in the beginning of the array to the top of the screen and the end to a lower portion of the screen.
 * A null terminated array signals the end of the array.
 * 
 * @code
 * 
 * @endcode
 * 
 * @param tile An array of character pointers with null termination
 * @param x The horizontal location to begin drawing the tile
 * @param y The vertical location to begin drawing the tile
 *
 *
 * @warning GL 10/10
 */
void Game_DrawTile(char *tile[], char x, char y);

/**
 * Draws the border of a rectangle using the following character codes: 186, 187, 188, 200, 201, 205
 * @param x_min The smallest horizontal location to begin drawing.
 * @param y_min The smallest vertical direction to begin drawing.
 * @param x_max The largest horizontal location to begin drawing.
 * @param y_max The largest vertical direction to begin drawing.
 * @warning Remember that the coordinate system starts in the upper left corner and is positive down the screen for the y axis.
 *
 * @warning GL 10/10
 */
void Game_DrawRect(char x_min, char y_min, char x_max, char y_max);

/**
 * @warning AA 10/10
 *
 * This function can be used to fill up a rectangular area with a passed character
 *
 * @param c - Some character
 * @param x_min - minimum value of x
 * @param y_min - minimum value of y
 * @param x_max - maximum value of x
 * @param y_max - maximum value of y
 * Example code:
 * @code
 *
 * #include <stdio.h>
 * #include <stdlib.h>
 * .
 * .
 * .
 * int main()
 * {
 * .
 * .
 * .
 * Game_FillRect('C', 1, 1, 10, 10)
 * .
 * .
 * .
 *  return 0;
 * }
 *
 *@endcode
 *
 */
void Game_FillRect(char c, char x_min, char y_min, char x_max, char y_max);

/**
 * @todo Gerald V. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_ScrollDown(void);

/**
 * @todo Gerald V. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_ScrollUp(void);

/**
 * @todo Atharva A. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Game_ClearScreen(void);

/** Game_IsTransmitting is used to determine whether the game is currently transmitting data to UART
 * This allows the user to check that the game screen is fully updated before allowing the game to advances forward 
 * 
 * @return 1 if the UART for the specified player is being updated, 0 if no values are being written to UART
 *
 * @warning DB 9/10
 */
uint8_t Game_IsTransmitting(void);

/**
 * @warning KC 8/10
 * 
 * This is used to print out an "\a" within the game. The "\a" is used to represent a bell that sounds an alarm
 */
void Game_Bell(void);

/** @brief let the game management module know that your game is over.
 * 
 * This function must be called after your game is over so the game management
 * module can allow users to play again.
 */
void Game_GameOver(void);

#ifdef USE_MODULE_GAME_NFR24
#include "nrf24.h"

typedef struct {
    uint8_t type;
    uint8_t index;
    controller_buttons_t controller[4];
    uint8_t user_data[22];
}game_network_payload_t;

void Game_RegisterInputCallback(void(*fn)(game_network_payload_t *));

void Game_RegisterHostPacketizer(uint8_t(*fn)(uint8_t *));

void Game_Join(uint8_t channel);

void Game_Host(uint8_t channel);

void Game_NetworkInit(nrf24_t * nrf_ptr);

#ifndef RF_ADDRESS
#define RF_ADDRESS 0x6C6C6C7800
#endif

#define GAME_NRF_TX_PERIOD 40

#endif

///@}
#endif // _GAME_H_
