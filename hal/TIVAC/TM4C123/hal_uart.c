#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "hal_uart.h"
#include "inc/hw_uart.h"
#include "inc/hw_types.h"
#include "project_settings.h"
#include "uart.h"

void hal_UART0_ISR(void);
void hal_UART1_ISR(void);
void hal_UART2_ISR(void);
void hal_UART3_ISR(void);
void hal_UART4_ISR(void);
void hal_UART5_ISR(void);
void hal_UART6_ISR(void);
void hal_UART7_ISR(void);

void hal_UART_Init(uint8_t channel, uint32_t baud) {
	switch(channel) {
		case UART0:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
			GPIOPinConfigure(GPIO_PA0_U0RX);
			GPIOPinConfigure(GPIO_PA1_U0TX);
			GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			UARTIntRegister(UART_BASE(channel), hal_UART0_ISR);
			break;
		case UART1:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
			GPIOPinConfigure(GPIO_PB0_U1RX);
			GPIOPinConfigure(GPIO_PB1_U1TX);
			GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			UARTIntRegister(UART_BASE(channel), hal_UART1_ISR);
			break;
		case UART2:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
			GPIOPinConfigure(GPIO_PD6_U2RX);
			GPIOPinConfigure(GPIO_PD7_U2TX);
			GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
			UARTIntRegister(UART_BASE(channel), hal_UART2_ISR);
			break;
		case UART3:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
			GPIOPinConfigure(GPIO_PC6_U3RX);
			GPIOPinConfigure(GPIO_PC7_U3TX);
			GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
			UARTIntRegister(UART_BASE(channel), hal_UART3_ISR);
			break;
		case UART4:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
			GPIOPinConfigure(GPIO_PC4_U4RX);
			GPIOPinConfigure(GPIO_PC5_U4TX);
			GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
			UARTIntRegister(UART_BASE(channel), hal_UART4_ISR);
			break;
		case UART5:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
			GPIOPinConfigure(GPIO_PE4_U5RX);
			GPIOPinConfigure(GPIO_PE5_U5TX);
			GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
			UARTIntRegister(UART_BASE(channel), hal_UART5_ISR);
			break;
		case UART6:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
			GPIOPinConfigure(GPIO_PD4_U6RX);
			GPIOPinConfigure(GPIO_PD5_U6TX);
			GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
			UARTIntRegister(UART_BASE(channel), hal_UART6_ISR);
			break;
		case UART7:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
			GPIOPinConfigure(GPIO_PE0_U7RX);
			GPIOPinConfigure(GPIO_PE1_U7TX);
			GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			UARTIntRegister(UART_BASE(channel), hal_UART7_ISR);
			break;
		}
	hal_UART_Disable(channel);
	UARTConfigSetExpClk(UART_BASE(channel), FCPU, baud, UART_CONFIG_WLEN_8);
	UARTFIFODisable(UART_BASE(channel));
	UARTRxErrorClear(UART_BASE(channel));
	hal_UART_EnableRxInterrupt(channel);
}

void hal_UART0_ISR(void){
	UART_Rx_Handler(UART0);
	hal_UART_ClearRxIF(UART0);
	UART_Tx_Handler(UART0);
}
void hal_UART1_ISR(void){
	UART_Rx_Handler(UART1);
	hal_UART_ClearRxIF(UART1);
	UART_Tx_Handler(UART1);
}
void hal_UART2_ISR(void){
	UART_Rx_Handler(UART2);
	hal_UART_ClearRxIF(UART2);
	UART_Tx_Handler(UART2);
}
void hal_UART3_ISR(void){
	UART_Rx_Handler(UART3);
	hal_UART_ClearRxIF(UART3);
	UART_Tx_Handler(UART3);
}
void hal_UART4_ISR(void){
	UART_Rx_Handler(UART4);
	hal_UART_ClearRxIF(UART4);
	UART_Tx_Handler(UART4);
}
void hal_UART5_ISR(void){
	UART_Rx_Handler(UART5);
	hal_UART_ClearRxIF(UART5);
	UART_Tx_Handler(UART5);
}
void hal_UART6_ISR(void){
	UART_Rx_Handler(UART6);
	hal_UART_ClearRxIF(UART6);
	UART_Tx_Handler(UART6);
}
void hal_UART7_ISR(void){
	UART_Rx_Handler(UART7);
	hal_UART_ClearRxIF(UART7);
	UART_Tx_Handler(UART7);
}
