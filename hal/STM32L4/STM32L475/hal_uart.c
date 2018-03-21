#include <stdint.h>
#include <stdbool.h>
#include "hal_uart.h"
#include "uart.h"
#include "stm32l4xx.h"
#include "project_settings.h"
//#include "pio.h"

#define BAUD(b) ((FCPU + (b/2))/(b))

void hal_UART_Init(uint8_t channel, uint32_t baud) {
	USART_TypeDef *uart_ptr;
	GPIO_InitTypeDef gpio_init_structure;
	gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_structure.Pull = GPIO_NOPULL;
	switch(channel) {
		case 1:
			uart_ptr = USART1;
			__HAL_RCC_USART1_CLK_ENABLE();
			// configure UART TX pin for alternate function
			gpio_init_structure.Pin = UART1_TX_PIN;
			gpio_init_structure.Alternate = UART1_TX_ALT_FUNCTION;
			HAL_GPIO_Init(UART1_TX_GPIO_PORT, &gpio_init_structure);
			// configure UART RX pin for alternate function
			gpio_init_structure.Pin = UART1_RX_PIN;
			gpio_init_structure.Alternate = UART1_RX_ALT_FUNCTION;
			HAL_GPIO_Init(UART1_RX_GPIO_PORT, &gpio_init_structure);
			break;
		case 2:
			uart_ptr = USART2;
			__HAL_RCC_USART2_CLK_ENABLE();
			break;
		case 3:
			uart_ptr = USART3;
			__HAL_RCC_USART3_CLK_ENABLE();
			break;
		case 4:
			uart_ptr = UART4;
			__HAL_RCC_UART4_CLK_ENABLE();
			break;
		case 5:
			uart_ptr = UART5;
			__HAL_RCC_UART5_CLK_ENABLE();
			break;
		default:
			return;
	}
	// enable clock for port B
	/// @todo add logic to determine which clock to use
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// disable UART and set everything to default
	uart_ptr->CR1 = 0; //&= ~USART_CR1_UE;
	uart_ptr->CR2 = 0;
	uart_ptr->CR3 = 0;
	uart_ptr->BRR = BAUD(baud);

	hal_UART_EnableRxInterrupt(channel);
	hal_UART_RxByte(channel);
    hal_UART_Enable(channel);
	hal_UART_EnableInterrupts(channel);
}

void hal_UART_EnableInterrupts(uint8_t channel) {
	switch(channel) {
		case 1:
			NVIC_DisableIRQ(USART1_IRQn);
			NVIC_ClearPendingIRQ(USART1_IRQn);
			NVIC_SetPriority(USART1_IRQn, 1); //Priority is 1
			NVIC_EnableIRQ(USART1_IRQn);
			break;
		case 2:
			NVIC_DisableIRQ(USART2_IRQn);
			NVIC_ClearPendingIRQ(USART2_IRQn);
			NVIC_SetPriority(USART2_IRQn, 1); //Priority is 1
			NVIC_EnableIRQ(USART2_IRQn);
			break;
		case 3:
			NVIC_DisableIRQ(USART3_IRQn);
			NVIC_ClearPendingIRQ(USART3_IRQn);
			NVIC_SetPriority(USART3_IRQn, 1); //Priority is 1
			NVIC_EnableIRQ(USART3_IRQn);
			break;
		case 4:
			NVIC_DisableIRQ(UART4_IRQn);
			NVIC_ClearPendingIRQ(UART4_IRQn);
			NVIC_SetPriority(UART4_IRQn, 1); //Priority is 1
			NVIC_EnableIRQ(UART4_IRQn);
			break;
		case 5:
			NVIC_DisableIRQ(UART5_IRQn);
			NVIC_ClearPendingIRQ(UART5_IRQn);
			NVIC_SetPriority(UART5_IRQn, 1); //Priority is 1
			NVIC_EnableIRQ(UART5_IRQn);
			break;
		default:
			break;
	}
}

void hal_UART_Enable(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; break;
		case 2: USART2->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; break;
		case 3: USART3->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; break;
		case 4: UART4->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; break;
		case 5: UART5->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; break;
		default: break;
	}
}

void hal_UART_Disable(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 &= ~(USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); break;
		case 2: USART2->CR1 &= ~(USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); break;
		case 3: USART3->CR1 &= ~(USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); break;
		case 4: UART4->CR1 &= ~(USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); break;
		case 5: UART5->CR1 &= ~(USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); break;
		default: break;
	}
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 |= USART_CR1_RXNEIE; break;
		case 2: USART2->CR1 |= USART_CR1_RXNEIE; break;
		case 3: USART3->CR1 |= USART_CR1_RXNEIE; break;
		case 4: UART4->CR1 |= USART_CR1_RXNEIE; break;
		case 5: UART5->CR1 |= USART_CR1_RXNEIE; break;
		default: break;
	}
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 |= USART_CR1_TXEIE; break;
		case 2: USART2->CR1 |= USART_CR1_TXEIE; break;
		case 3: USART3->CR1 |= USART_CR1_TXEIE; break;
		case 4: UART4->CR1 |= USART_CR1_TXEIE; break;
		case 5: UART5->CR1 |= USART_CR1_TXEIE; break;
		default: break;
	}
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 &= ~USART_CR1_RXNEIE; break;
		case 2: USART2->CR1 &= ~USART_CR1_RXNEIE; break;
		case 3: USART3->CR1 &= ~USART_CR1_RXNEIE; break;
		case 4: UART4->CR1 &= ~USART_CR1_RXNEIE; break;
		case 5: UART5->CR1 &= ~USART_CR1_RXNEIE; break;
		default: break;
	}
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
	switch(channel) {
		case 1: USART1->CR1 &= ~USART_CR1_TXEIE; break;
		case 2: USART2->CR1 &= ~USART_CR1_TXEIE; break;
		case 3: USART3->CR1 &= ~USART_CR1_TXEIE; break;
		case 4: UART4->CR1 &= ~USART_CR1_TXEIE; break;
		case 5: UART5->CR1 &= ~USART_CR1_TXEIE; break;
		default: break;
	}
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
	switch(channel) {
		case 1: USART1->TDR = c; break;
		case 2: USART2->TDR = c; break;
		case 3: USART3->TDR = c; break;
		case 4: UART4->TDR = c; break;
		case 5: UART5->TDR = c; break;
		default: break;
	}
}

uint8_t hal_UART_RxByte(uint8_t channel) {
	switch(channel) {
		case 1: return USART1->RDR;
		case 2: return USART2->RDR;
		case 3: return USART3->RDR;
		case 4: return UART4->RDR;
		case 5: return UART5->RDR;
		default: return 0;
	}
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
	switch(channel) {
		case 1: return (USART1->ISR & USART_ISR_RXNE) ? 1 : 0;
		case 2: return (USART2->ISR & USART_ISR_RXNE) ? 1 : 0;
		case 3: return (USART3->ISR & USART_ISR_RXNE) ? 1 : 0;
		case 4: return (UART4->ISR & USART_ISR_RXNE) ? 1 : 0;
		case 5: return (UART5->ISR & USART_ISR_RXNE) ? 1 : 0;
		default: return 0;
	}
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
	switch(channel) {
		case 1: return (USART1->ISR & USART_ISR_TXE) ? 1 : 0;
		case 2: return (USART2->ISR & USART_ISR_TXE) ? 1 : 0;
		case 3: return (USART3->ISR & USART_ISR_TXE) ? 1 : 0;
		case 4: return (UART4->ISR & USART_ISR_TXE) ? 1 : 0;
		case 5: return (UART5->ISR & USART_ISR_TXE) ? 1 : 0;
		default: return 0;
	}
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
	switch(channel) {
		case 1: return (USART1->CR1 & USART_CR1_TXEIE) ? 1 : 0;
		case 2: return (USART2->CR1 & USART_CR1_TXEIE) ? 1 : 0;
		case 3: return (USART3->CR1 & USART_CR1_TXEIE) ? 1 : 0;
		case 4: return (UART4->CR1 & USART_CR1_TXEIE) ? 1 : 0;
		case 5: return (UART5->CR1 & USART_CR1_TXEIE) ? 1 : 0;
		default: return 0;
	}
}

#ifdef USE_UART1
void USART1_IRQHandler(void) {
    UART_Rx_Handler(1);
    UART_Tx_Handler(1);
}
#endif
#ifdef USE_UART2
void USART2_IRQHandler(void) {
    UART_Rx_Handler(2);
    UART_Tx_Handler(2);
}
#endif
#ifdef USE_UART3
void USART3_IRQHandler(void) {
    UART_Rx_Handler(3);
    UART_Tx_Handler(3);
}
#endif
#ifdef USE_UART4
void UART4_IRQHandler(void) {
    UART_Rx_Handler(4);
    UART_Tx_Handler(4);
}
#endif
#ifdef USE_UART5
void USART5_IRQHandler(void) {
    UART_Rx_Handler(5);
    UART_Tx_Handler(5);
}
#endif
