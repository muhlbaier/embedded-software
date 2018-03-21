#include <stdint.h>
#include <stdbool.h>

#include "hw_memmap.h"
#include "hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/prcm.h"
#include "driverlib/pin.h"
#include "hw_uart.h"

#include "hal_uart.h"
#include "uart.h"
#include "hal_pin_macros.h"
#include "project_settings.h"

void hal_UART_ConfigClock(uint8_t channel, uint32_t baud);

// Provide user warnings for selecting pins
#ifdef USE_UART0
	#ifndef UART0_TX_GPIO
		#error "Must define UART0_TX_GPIO and UART0_RX_GPIO in project_settings.h if you are using UART0. Ex. #define UART0_TX_GPIO 1"
	#endif
	#ifndef UART0_RX_GPIO
		#error "Must define UART0_TX_GPIO and UART0_RX_GPIO in project_settings.h if you are using UART0. Ex. #define UART0_RX_GPIO 2"
	#endif
#endif

#ifdef USE_UART1
	#ifndef UART1_TX_GPIO
		#error "Must define UART1_TX_GPIO and UART1_RX_GPIO in project_settings.h if you are using UART1. Ex. #define UART1_TX_GPIO 1"
	#endif
	#ifndef UART1_RX_GPIO
		#error "Must define UART1_TX_GPIO and UART1_RX_GPIO in project_settings.h if you are using UART1. Ex. #define UART1_RX_GPIO 2"
	#endif
#endif


void hal_UART0_ISR(void);
void hal_UART1_ISR(void);

void hal_UART_Init(uint8_t channel, uint32_t baud) {
	switch(channel) {
#ifdef USE_UART0
		case UART0:
			PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
			PRCMPeripheralClkEnable(GET_GPIO_PORT_PERIPHERAL(UART0_TX_GPIO), PRCM_RUN_MODE_CLK);
			PRCMPeripheralClkEnable(GET_GPIO_PORT_PERIPHERAL(UART0_RX_GPIO), PRCM_RUN_MODE_CLK);
		    PinTypeUART(CAT3(GPIO, UART0_TX_GPIO, _PIN_NUM), CAT3(GPIO, UART0_TX_GPIO, _UART0_TX));
		    PinTypeUART(CAT3(GPIO, UART0_RX_GPIO, _PIN_NUM), CAT3(GPIO, UART0_RX_GPIO, _UART0_RX));
			UARTIntRegister(UART_BASE(channel), hal_UART0_ISR);
			break;
#endif
#ifdef USE_UART1
		case UART1:
			PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);
			PRCMPeripheralClkEnable(GET_GPIO_PORT_PERIPHERAL(UART1_TX_GPIO), PRCM_RUN_MODE_CLK);
			PRCMPeripheralClkEnable(GET_GPIO_PORT_PERIPHERAL(UART1_RX_GPIO), PRCM_RUN_MODE_CLK);
			PinTypeUART(CAT3(GPIO, UART1_TX_GPIO, _PIN_NUM), CAT3(GPIO, UART1_TX_GPIO, _UART1_TX));
			PinTypeUART(CAT3(GPIO, UART1_RX_GPIO, _PIN_NUM), CAT3(GPIO, UART1_RX_GPIO, _UART1_RX));
			UARTIntRegister(UART_BASE(channel), hal_UART1_ISR);
			break;
#endif
		default:
			return;
		}

	// Disable the channel for configuration
	hal_UART_Disable(channel);
	// Configure the clock settings for the baud rate
	hal_UART_ConfigClock(channel, baud);
	// Set parity, data length, and number of stop bits.
    HWREG(UART_BASE(channel) + UART_O_LCRH) = (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    // Clear the flags register.
    HWREG(UART_BASE(channel) + UART_O_FR) = 0;
    // Enable the rx interrupt
	hal_UART_EnableRxInterrupt(channel);
	// Enable error interrupts
	hal_UART_EnableErrorInterrupts(channel);


	// Enable the UART channel
	hal_UART_Enable(channel);
}

void hal_UART_ConfigClock(uint8_t channel, uint32_t baud){
	uint32_t div;

	if((baud * 16) > PRCMPeripheralClockGet(PRCM_UARTA0 + channel)) {
        // Enable high speed mode.
        HWREG(UART_BASE(channel) + UART_O_CTL) |= UART_CTL_HSE;

        // Half the supplied baud rate to compensate for enabling high speed
        // mode.  This allows the following code to be common to both cases.
        baud /= 2;
    }
    else {
        // Disable high speed mode.
        HWREG(UART_BASE(channel) + UART_O_CTL) &= ~(UART_CTL_HSE);
    }

    // Compute the fractional baud rate divider.
    div = (((PRCMPeripheralClockGet(PRCM_UARTA0 + channel) * 8) / baud) + 1) / 2;

    //
    // Set the baud rate.
    //
    HWREG(UART_BASE(channel) + UART_O_IBRD) = div / 64;
    HWREG(UART_BASE(channel) + UART_O_FBRD) = div % 64;
}

inline void hal_UART_ISR(uint8_t channel){
    // Read interrupt status register to determine interrupt cause
    uint32_t status = UARTIntStatus(UART_BASE(channel), true);

	// Frame Error Interrupt
    if(status & UART_RIS_FERIS){
    	UART_Error_Handler(channel, FRAMING_ERROR);
    	hal_UART_ClearFrameErrorIF(channel);
    }

    // Parity Error Interrupt
    if(status & UART_RIS_PERIS){
    	UART_Error_Handler(channel, PARITY_ERROR);
    	hal_UART_ClearParityErrorIF(channel);
    }

    // Break Error Interrupt
    if(status & UART_RIS_BERIS){
    	UART_Error_Handler(channel, BREAK_ERROR);
    	hal_UART_ClearBreakErrorIF(channel);
    }

    // Overrun Error Interrupt
    if(status & UART_RIS_OERIS){
    	UART_Error_Handler(channel, OVERRUN_ERROR);
    	hal_UART_ClearOverrunErrorIF(channel);
    }
	
    // Receive interrupt
    if(status & UART_RIS_RXRIS){
    	UART_Rx_Handler(channel);
		hal_UART_ClearRxIF(channel);
    }

    // Transmit interrupt
    if(status & UART_RIS_TXRIS){
    	UART_Tx_Handler(channel);
    	hal_UART_ClearTxIF(channel);
    }
}

void hal_UART0_ISR(void){
	hal_UART_ISR(UART0);
}

void hal_UART1_ISR(void){
	hal_UART_ISR(UART1);
}
