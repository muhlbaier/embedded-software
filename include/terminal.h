#ifndef _TERMINAL_H_
#define _TERMINAL_H_
/**
 * @defgroup terminal Terminal Helper Functions
 * @ingroup helpers
 * 
 * @{
 */

#include <stdint.h>

#include "project_settings.h"

#ifndef USE_MODULE_BUFFER_PRINTF
#error "This module requires optional UART functionality provided by buffer_printf. Please declare USE_MODULE_BUFFER_PRINTF in project_settings.h"
#endif

/**
 * This enumeration allows the user to easily set up specific configurations for the characters
 * which are printed to the terminal. The position in this enumeration corresponds to the ANSI Escape Sequences 
 * for color text and formatting. Please reference http://bluesock.org/~willg/dev/ansi.html for specifics on which
 * element in the enumeration is responsible for controlling what attribute within the terminal.
 * 
 * @warning RT 10/10
 * 
 */
enum term_color {
    Reset_all_attributes = 0,
    Bright,
    Dim,
    Underscore,
    Blink,
    Reverse,
    Hidden,
    ForegroundBlack = 30,
    ForegroundRed,
    ForegroundGreen,
    ForegroundYellow,
    ForegroundBlue,
    ForegroundMagenta,
    ForegroundCyan,
    ForegroundWhite,
    BackgroundBlack = 40,
    BackgroundRed,
    BackgroundGreen,
    BackgroundYellow,
    BackgroundBlue,
    BackgroundMagenta,
    BackgroundCyan,
    BackgroundWhite
};

/** Reusable function to push a character to a terminal at a specific location.
 *
 * This function will only work when the buffer is used to communicate to a terminal
 * supporting the used features (e.g. PuTTY)
 *
 * Location 0,0 is the top left corner of the terminal
 *
 * @param channel UART channel
 * @param c char to send
 * @param x zero base x location
 * @param y zero base y location
 */
void Terminal_CharXY(uint8_t channel, char c, char x, char y);

void Terminal_CursorXY(uint8_t channel, char x, char y);

/** Function to turn the cursor off
 *
 * This function will make the cursor within the terminal hidden by utilizing the DECTCEM (DEC text cursor enable mode)
 * escape sequence "%c[?25l". The ASCII_ESC character is needed so the terminal knows that the above sequence is a control sequence. 
 *
 * @param channel UART Channel
 * 
 * @warning RT 10/10
 */
void Terminal_HideCursor(uint8_t channel);

/**
 *
 * @param channel
 *
 * @todo Gerald V. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Terminal_ShowCursor(uint8_t channel);

/**
 *
 * @param channel
 * @param color
 *
 * @todo Gerald V. Document this function (when done change this line to "@todo MM check <your names> documentation"
 */
void Terminal_SetColor(uint8_t channel, enum term_color color);

void Terminal_ClearScreen(uint8_t channel);

///@}
#endif // _TERMINAL_H_
