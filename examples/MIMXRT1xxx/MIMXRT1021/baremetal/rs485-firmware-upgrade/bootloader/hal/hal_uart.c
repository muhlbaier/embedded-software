#include <stdint.h>
#include <stdbool.h>
#include "hal_uart.h"
#include "uart.h"
#include "project_settings.h"
//#include "MIMXRT1021.h"
#include "fsl_clock.h"

#include "debug.h"

#define UART_WAIT4TC_COUNT		40000

void hal_UART_Init(uint8_t channel, uint32_t baud) 
{
	uint32_t uart_clock[9] = 
	{
		0,
		kCLOCK_Lpuart1,
		kCLOCK_Lpuart2,
		kCLOCK_Lpuart3,
		kCLOCK_Lpuart4,
		kCLOCK_Lpuart5,
		kCLOCK_Lpuart6,
		kCLOCK_Lpuart7,
		kCLOCK_Lpuart8
	};
	
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	// enable clock;
	CLOCK_EnableClock(uart_clock[channel]);
	
	switch(channel) {
		case 1:
			IOMUXC->SW_MUX_CTL_PAD[UART1_TX_kIOMUXC_SW_MUX_CTL_PAD_GPIO] = UART1_TX_kIOMUXC_SW_MUX_CTL_PAD_GPIO_MUX_MODE;
			IOMUXC->SW_PAD_CTL_PAD[UART1_TX_kIOMUXC_SW_PAD_CTL_PAD_GPIO] = UART1_TX_kIOMUXC_SW_PAD_CTL_PAD_GPIO_FIELDS;
			IOMUXC->SW_MUX_CTL_PAD[UART1_RX_kIOMUXC_SW_MUX_CTL_PAD_GPIO] = UART1_RX_kIOMUXC_SW_MUX_CTL_PAD_GPIO_MUX_MODE;
			IOMUXC->SW_PAD_CTL_PAD[UART1_RX_kIOMUXC_SW_PAD_CTL_PAD_GPIO] = UART1_RX_kIOMUXC_SW_PAD_CTL_PAD_GPIO_FIELDS;
		
			break;
		case 2:
			IOMUXC->SW_MUX_CTL_PAD[UART2_TX_kIOMUXC_SW_MUX_CTL_PAD_GPIO] = UART2_TX_kIOMUXC_SW_MUX_CTL_PAD_GPIO_MUX_MODE;
			IOMUXC->SW_PAD_CTL_PAD[UART2_TX_kIOMUXC_SW_PAD_CTL_PAD_GPIO] = UART2_TX_kIOMUXC_SW_PAD_CTL_PAD_GPIO_FIELDS;
			IOMUXC->SW_MUX_CTL_PAD[UART2_RX_kIOMUXC_SW_MUX_CTL_PAD_GPIO] = UART2_RX_kIOMUXC_SW_MUX_CTL_PAD_GPIO_MUX_MODE;
			IOMUXC->SW_PAD_CTL_PAD[UART2_RX_kIOMUXC_SW_PAD_CTL_PAD_GPIO] = UART2_RX_kIOMUXC_SW_PAD_CTL_PAD_GPIO_FIELDS;
		
			
			break;
		case 3:
			
			break;
		case 4:
			
			break;
		case 5:
			
			break;
		case 6:
			
			break;
		case 7:
			
			break;
		case 8:
			
			break;
		default:
			return;
	}
		
	
	// reset peripheral
	LPUARTx->GLOBAL = (LPUART_GLOBAL_RST_MASK);
	LPUARTx->GLOBAL &= ~(LPUART_GLOBAL_RST_MASK);
	// disable input trigger
	LPUARTx->PINCFG = 0x00000000;
	
	LPUARTx->STAT = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
	
	LPUARTx->FIFO = 0;
	// oversampling by 16, one stop bit
	baud = (80000000/16)/baud;
	LPUARTx->BAUD = baud|(15<<24);
	LPUARTx->CTRL = 0;

	hal_UART_EnableRxInterrupt(channel);
	hal_UART_RxByte(channel);
	
    hal_UART_Enable(channel);
	hal_UART_EnableInterrupts(channel);
}

void hal_UART_EnableInterrupts(uint8_t channel) 
{///
	
	uint32_t UARTx_IRQn = 0;
	
	switch(channel) {
		case 1:
			UARTx_IRQn = LPUART1_IRQn;
			break;
		case 2:
			UARTx_IRQn = LPUART2_IRQn;
			break;
		case 3:
			UARTx_IRQn = LPUART3_IRQn;
			break;
		case 4:
			UARTx_IRQn = LPUART4_IRQn;
			break;
		case 5:
			UARTx_IRQn = LPUART5_IRQn;
			break;
		case 6:
			UARTx_IRQn = LPUART6_IRQn;
			break;
		case 7:
			UARTx_IRQn = LPUART7_IRQn;
			break;
		case 8:
			UARTx_IRQn = LPUART8_IRQn;
			break;
		default:
		{
			return;
			break;
		}
	}
	
	NVIC_DisableIRQ(UARTx_IRQn);
	NVIC_ClearPendingIRQ(UARTx_IRQn);
	NVIC_SetPriority(UARTx_IRQn, 1); //Priority is 1
	NVIC_EnableIRQ(UARTx_IRQn);
}

LPUART_Type * UART_GetInstance(uint8_t channel)
{///
	LPUART_Type * LPUARTx = 0;
	
	switch(channel) {
		case 1:
			LPUARTx = LPUART1;
			break;
		case 2:
			LPUARTx = LPUART2;
			break;
		case 3:
			LPUARTx = LPUART3;
			break;
		case 4:
			LPUARTx = LPUART4;
			break;
		case 5:
			LPUARTx = LPUART5;
			break;
		case 6:
			LPUARTx = LPUART6;
			break;
		case 7:
			LPUARTx = LPUART7;
			break;
		case 8:
			LPUARTx = LPUART8;
			break;
		default:
			break;
	}
	
	return LPUARTx;
}

void hal_UART_Enable(uint8_t channel) 
{///
	
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->CTRL |= (LPUART_CTRL_TE_MASK)|(LPUART_CTRL_RE_MASK);
	__DMB();
}

void hal_UART_Disable(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->CTRL &= ~((LPUART_CTRL_TE_MASK)|(LPUART_CTRL_RE_MASK));
}

void hal_UART_EnableRxInterrupt(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->CTRL |= (LPUART_CTRL_RIE_MASK);
}

void hal_UART_EnableTxInterrupt(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	
	LPUARTx->CTRL |= (LPUART_CTRL_TCIE_MASK);
}

void hal_UART_DisableRxInterrupt(uint8_t channel) 
{///	
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->CTRL &= ~(LPUART_CTRL_RIE_MASK);
}

void hal_UART_DisableTxInterrupt(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->CTRL &= ~(LPUART_CTRL_TCIE_MASK);
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return;
	}
	
	LPUARTx->DATA = c;
}

uint8_t hal_UART_RxByte(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return 0;
	}
	
	return LPUARTx->DATA;
}

uint8_t hal_UART_DataAvailable(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return 0;
	}
	
	if(LPUARTx->STAT & LPUART_STAT_RDRF_MASK) 
	{
		return 1;
	}
	
	return 0;
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return 0;
	}
	
	if(LPUARTx->STAT & LPUART_STAT_TC_MASK) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) 
{///
	LPUART_Type * LPUARTx = UART_GetInstance(channel);
	if(LPUARTx == 0)
	{
		return 0;
	}
	
	if(LPUARTx->CTRL & (LPUART_CTRL_TCIE_MASK))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

#ifdef USE_UART1
void LPUART1_IRQHandler(void) {
	
    UART_Rx_Handler(1);
    UART_Tx_Handler(1);
}
#endif
#ifdef USE_UART2
void LPUART2_IRQHandler(void) {

    UART_Rx_Handler(2);
    UART_Tx_Handler(2);
}
#endif
#ifdef USE_UART3
void LPUART3_IRQHandler(void) {
    UART_Rx_Handler(3);
    UART_Tx_Handler(3);
}
#endif
#ifdef USE_UART4
void LPUART4_IRQHandler(void) {
    UART_Rx_Handler(4);
    UART_Tx_Handler(4);
}
#endif
#ifdef USE_UART5
void LPUART5_IRQHandler(void) {
    UART_Rx_Handler(5);
    UART_Tx_Handler(5);
}
#endif
#ifdef USE_UART6
void LPUART6_IRQHandler(void) {
    UART_Rx_Handler(6);
    UART_Tx_Handler(6);
}
#endif
#ifdef USE_UART7
void LPUART7_IRQHandler(void) {
    UART_Rx_Handler(7);
    UART_Tx_Handler(7);
}
#endif
#ifdef USE_UART8
void LPUART8_IRQHandler(void) {
    UART_Rx_Handler(8);
    UART_Tx_Handler(8);
}
#endif


