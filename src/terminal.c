#include "terminal.h"

#include "uart.h"
#include "macros.h"

// compliments of Jehandad
void Terminal_CharXY(uint8_t channel, char c, char x, char y) {
	UART_printf(channel,"%c[%d;%dH%c",ASCII_ESC,y+1,x+1,c);
}

void Terminal_CursorXY(uint8_t channel, char x, char y) {
	UART_printf(channel,"%c[%d;%dH",ASCII_ESC,y+1,x+1);
}

void Terminal_HideCursor(uint8_t channel) {
    UART_printf(channel, "%c[?25l", ASCII_ESC);
}

void Terminal_ShowCursor(uint8_t channel) {
    UART_printf(channel, "%c[?25h", ASCII_ESC);
}

void Terminal_ClearScreen(uint8_t channel){
	UART_printf(channel, "\f");
}

void Terminal_SetColor(uint8_t channel, enum term_color color) {
    UART_printf(channel, "%c[%dm", ASCII_ESC, color);
}
