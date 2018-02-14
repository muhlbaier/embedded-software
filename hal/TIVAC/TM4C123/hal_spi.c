/*
 *  hal_spi.c - SPI HAL For the Tiva-C TM4C123
 *
 *  Created on: Mar 29, 2017
 *      Author: Christopher Green <greenc3@students.rowan.edu>
 */

#include "hal_spi.h"

void SPI_ISR0(void);
void SPI_ISR1(void);
void SPI_ISR2(void);
void SPI_ISR3(void);

/* Some notes about this module
 * 
 * -It uses the Tiva-C SSI modules and a combination of TivaWare and direct register manipulation to achieve an SPI HAL
 * -The SSI modules have an 8-deep TX and RX FIFO that complicated handling data a little bit
 * -The Tiva-C can do a bunch of different interrupt types. We want RX-timeout (RXTO) and TX-fifo more than half empty (TXFO)
 * -SSI1 doesn't seem to work properly at higher speeds, this may be because it's used to drive the onboard LED
 * -I've tested this module using an nRF24 and confirmed that SSI channels 0, 2, and 3 work with the chip
 *
 * -TODO: Add a bit-swap feature to accomplish LSB-first functionality, right now we can only do MSB-first
 */

void hal_SPI_Init(spi_settings_t* settings){
	switch(settings->channel){
		case SSI0:
			// Initialize necessary GPIO
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // SSI0 Peripheral Uses GPIOA
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {} // Wait for it to be ready
			GPIOPinConfigure(GPIO_PA2_SSI0CLK); // PA2 SCK
			GPIOPinConfigure(GPIO_PA3_SSI0FSS);	// PA3 SS
			GPIOPinConfigure(GPIO_PA4_SSI0RX); // PA4 MISO
			GPIOPinConfigure(GPIO_PA5_SSI0TX); // PA5 MOSI
			GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5); // Set pins as SSI Mode
			// Initialize and configure SSI peripheral
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);	// Init SSI0 Module
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0)) {} // Wait for it to be ready
			// Determine and set SPI Mode
			switch(settings->mode){
				case 0:
					SSIConfigSetExpClk(SSI0_BASE, FCPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 1:
					SSIConfigSetExpClk(SSI0_BASE, FCPU, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 2:
					SSIConfigSetExpClk(SSI0_BASE, FCPU, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 3:
					SSIConfigSetExpClk(SSI0_BASE, FCPU, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				default:
					break;
			}
			SSIIntRegister(SSI0_BASE, (void*)(SPI_ISR0)); // Register interrupt for SSI0
			break;
		case SSI1:
			// Initialize necessary GPIO
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // SSI1 Peripheral Uses GPIOF
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {} // Wait for it to be ready
			GPIOPinConfigure(GPIO_PF2_SSI1CLK); // PF2 SCK
			GPIOPinConfigure(GPIO_PF3_SSI1FSS);	// PF3 SS
			GPIOPinConfigure(GPIO_PF0_SSI1RX); // PF0 MISO
			GPIOPinConfigure(GPIO_PF1_SSI1TX); // PF1 MOSI
			GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // Set pins as SSI Mode
			// Initialize and configure SSI peripheral
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);	// Init SSI1 Module
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1)) {} // Wait for it to be ready
			// Determine and set SPI Mode
			switch(settings->mode){
				case 0:
					SSIConfigSetExpClk(SSI1_BASE, FCPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 1:
					SSIConfigSetExpClk(SSI1_BASE, FCPU, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 2:
					SSIConfigSetExpClk(SSI1_BASE, FCPU, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 3:
					SSIConfigSetExpClk(SSI1_BASE, FCPU, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				default:
					break;
			}
			SSIIntRegister(SSI1_BASE, (void*)(SPI_ISR1)); // Register interrupt for SSI1
			break;
		case SSI2:
			// Initialize necessary GPIO
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // SSI2 Peripheral Uses GPIOB
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {} // Wait for it to be ready
			GPIOPinConfigure(GPIO_PB4_SSI2CLK); // PB4 SCK
			GPIOPinConfigure(GPIO_PB5_SSI2FSS);	// PB5 SS
			GPIOPinConfigure(GPIO_PB6_SSI2RX); // PB6 MISO
			GPIOPinConfigure(GPIO_PB7_SSI2TX); // PB7 MOSI
			GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // Set pins as SSI Mode
			// Initialize and configure SSI peripheral
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);	// Init SSI2 Module
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)) {} // Wait for it to be ready
			// Determine and set SPI Mode
			switch(settings->mode){
				case 0:
					SSIConfigSetExpClk(SSI2_BASE, FCPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 1:
					SSIConfigSetExpClk(SSI2_BASE, FCPU, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 2:
					SSIConfigSetExpClk(SSI2_BASE, FCPU, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 3:
					SSIConfigSetExpClk(SSI2_BASE, FCPU, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				default:
					break;
			} 
			SSIIntRegister(SSI2_BASE, (void*)(SPI_ISR2)); // Register interrupt for SSI2
			break;
		case SSI3:
			// Initialize necessary GPIO
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // SSI3 Peripheral Uses GPIOD
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)) {} // Wait for it to be ready
			GPIOPinConfigure(GPIO_PD0_SSI3CLK); // PD0 SCK
			GPIOPinConfigure(GPIO_PD1_SSI3FSS);	// PD1 SS
			GPIOPinConfigure(GPIO_PD2_SSI3RX); // PD2 MISO
			GPIOPinConfigure(GPIO_PD3_SSI3TX); // PD3 MOSI
			GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // Set pins as SSI Mode
			// Initialize and configure SSI peripheral
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);	// Init SSI3 Module
			while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI3)) {} // Wait for it to be ready
			// Determine and set SPI Mode
			switch(settings->mode){
				case 0:
					SSIConfigSetExpClk(SSI3_BASE, FCPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 1:
					SSIConfigSetExpClk(SSI3_BASE, FCPU, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 2:
					SSIConfigSetExpClk(SSI3_BASE, FCPU, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				case 3:
					SSIConfigSetExpClk(SSI3_BASE, FCPU, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, settings->bit_rate, settings->hal_settings.data_width);
					break;
				default:
					break;
			} 
			SSIIntRegister(SSI3_BASE, (void*)(SPI_ISR3)); // Register interrupt for SSI3
			break;
	}
	hal_SPI_Enable(settings->channel);
	hal_SPI_ClearRxIF(settings->channel);
	hal_SPI_EnableRxInterrupt(settings->channel);
}

// Enable SPI channel and re-register ISR.  Re-registering must be done because the disable call will de-register ISR
// Registering an ISR that has already been registered has no effect so this is safe
void hal_SPI_Enable(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIEnable(SSI0_BASE);
			SSIIntRegister(SSI0_BASE, (&SPI_ISR0));
			break;
		case SSI1:
			SSIEnable(SSI1_BASE);
			SSIIntRegister(SSI1_BASE, (&SPI_ISR1));
			break;
		case SSI2:
			SSIEnable(SSI2_BASE);
			SSIIntRegister(SSI2_BASE, (&SPI_ISR2));
			break;
		case SSI3:
			SSIEnable(SSI3_BASE);
			SSIIntRegister(SSI3_BASE, (&SPI_ISR3));
			break;
		default:
			return;
	}
}

// Disable the SPI channel and de-register the ISR
void hal_SPI_Disable(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIDisable(SSI0_BASE);
			SSIIntUnregister(SSI0_BASE);
			break;
		case SSI1:
			SSIDisable(SSI1_BASE);
			SSIIntUnregister(SSI1_BASE);
			break;
		case SSI2:
			SSIDisable(SSI2_BASE);
			SSIIntUnregister(SSI2_BASE);
			break;
		case SSI3:
			SSIDisable(SSI3_BASE);
			SSIIntUnregister(SSI3_BASE);
			break;
		default:
			return;
	}
}

// Enable an interrupt to be triggered on RX time-out (triggered 32 SPI clock cycles after the last received bit)
void hal_SPI_EnableRxInterrupt(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntEnable(SSI0_BASE, SSI_RXTO);
			break;
		case SSI1:
			SSIIntEnable(SSI1_BASE, SSI_RXTO);
			break;
		case SSI2:
			SSIIntEnable(SSI2_BASE, SSI_RXTO);
			break;
		case SSI3:
			SSIIntEnable(SSI3_BASE, SSI_RXTO);
			break;
		default:
			return;
	}
}

// Enable an interrupt to be triggered when the transmit FIFO is more than half empty
void hal_SPI_EnableTxInterrupt(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntEnable(SSI0_BASE, SSI_TXFF);  
			break;
		case SSI1:
			SSIIntEnable(SSI1_BASE, SSI_TXFF);
			break;
		case SSI2:
			SSIIntEnable(SSI2_BASE, SSI_TXFF);
			break;
		case SSI3:
			SSIIntEnable(SSI3_BASE, SSI_TXFF);
			break;
		default:
			return;
	}
}

void hal_SPI_DisableRxInterrupt(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntDisable(SSI0_BASE, SSI_RXTO); 
			break;
		case SSI1:
			SSIIntDisable(SSI1_BASE, SSI_RXTO);
			break;
		case SSI2:
			SSIIntDisable(SSI2_BASE, SSI_RXTO);
			break;
		case SSI3:
			SSIIntDisable(SSI3_BASE, SSI_RXTO);
			break;
		default:
			return;
	}
}

void hal_SPI_DisableTxInterrupt(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntDisable(SSI0_BASE, SSI_TXFF); 
			break;
		case SSI1:
			SSIIntDisable(SSI1_BASE, SSI_TXFF);
			break;
		case SSI2:
			SSIIntDisable(SSI2_BASE, SSI_TXFF);
			break;
		case SSI3:
			SSIIntDisable(SSI3_BASE, SSI_TXFF);
			break;
		default:
			return;
	}
}

// This function checks if there is a pending TX interrupt
uint8_t hal_SPI_TxIntStatus(uint8_t channel){
	switch(channel){
		case SSI0:
			// Check the SSI Masked Interrupt Status register for a pending interrupt that needs servicing
			return ((HWREG(SSI0_BASE + SSI_O_MIS) & SSI_MIS_TXMIS)>0)?1:0;
		case SSI1:
			return ((HWREG(SSI1_BASE + SSI_O_MIS) & SSI_MIS_TXMIS)>0)?1:0;
		case SSI2:
			return ((HWREG(SSI2_BASE + SSI_O_MIS) & SSI_MIS_TXMIS)>0)?1:0;
		case SSI3:
			return ((HWREG(SSI3_BASE + SSI_O_MIS) & SSI_MIS_TXMIS)>0)?1:0;
		default:
			return 0;
	}
}

// This function checks if the TX interrupt is enabled
uint8_t hal_SPI_IsTxIntEnabled(uint8_t channel){
	switch(channel){
		case SSI0:
			return (SSIIntStatus(SSI0_BASE, SSI_TXFF) >> SSI_TXFF);
		case SSI1:
			return (SSIIntStatus(SSI1_BASE, SSI_TXFF) >> SSI_TXFF);
		case SSI2:
			return (SSIIntStatus(SSI2_BASE, SSI_TXFF) >> SSI_TXFF);
		case SSI3:
			return (SSIIntStatus(SSI3_BASE, SSI_TXFF) >> SSI_TXFF);
		default:
			return 0;
	}
}

// This function checks if there is a pending RX interrupt
uint8_t hal_SPI_RxIntStatus(uint8_t channel){
	switch(channel){
		case SSI0:
			// Check the SSI Masked Interrupt Status register for a pending RX-timeout interrupt that needs servicing
			return ((HWREG(SSI0_BASE + SSI_O_MIS) & SSI_MIS_RTMIS)>0)?1:0;
		case SSI1:
			return ((HWREG(SSI1_BASE + SSI_O_MIS) & SSI_MIS_RTMIS)>0)?1:0;
		case SSI2:
			return ((HWREG(SSI2_BASE + SSI_O_MIS) & SSI_MIS_RTMIS)>0)?1:0;
		case SSI3:
			return ((HWREG(SSI3_BASE + SSI_O_MIS) & SSI_MIS_RTMIS)>0)?1:0;
		default:
			return 0;
	}
}

// Send one byte (or at least push it into the TX FIFO)
void hal_SPI_TxByte(uint8_t channel, uint8_t b){
	switch(channel){
		case SSI0:
			SSIDataPut(SSI0_BASE, (uint32_t)(b));
			break;
		case SSI1:
			SSIDataPut(SSI1_BASE, (uint32_t)(b));
			break;
		case SSI2:
			SSIDataPut(SSI2_BASE, (uint32_t)(b));
			break;
		case SSI3:
			SSIDataPut(SSI3_BASE, (uint32_t)(b));
			break;
		default:
			return;
	}
}

// Get a byte from the RX FIFO
uint8_t hal_SPI_RxByte(uint8_t channel){
	uint32_t received = 0;
	switch(channel){
		case SSI0:
			SSIDataGetNonBlocking(SSI0_BASE, &received); // Non blocking call will just return 0 if no data is available instead of waiting
			break;
		case SSI1:
			SSIDataGetNonBlocking(SSI1_BASE, &received);
			break;
		case SSI2:
			SSIDataGetNonBlocking(SSI2_BASE, &received);
			break;
		case SSI3:
			SSIDataGetNonBlocking(SSI3_BASE, &received);
			break;
		default:
			return 0;
	}
	return (uint8_t)(received);	// Warning: Be aware that if using > 8-bit data, some changes will need to be made to the embedded library
}

void hal_SPI_ClearRxIF(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntClear(SSI0_BASE, SSI_RXFF); 
			break;
		case SSI1:
			SSIIntClear(SSI1_BASE, SSI_RXFF);
			break;
		case SSI2:
			SSIIntClear(SSI2_BASE, SSI_RXFF);
			break;
		case SSI3:
			SSIIntClear(SSI3_BASE, SSI_RXFF);
			break;
		default:
			return;
	}
}

void hal_SPI_ClearTxIF(uint8_t channel){
	switch(channel){
		case SSI0:
			SSIIntClear(SSI0_BASE, SSI_TXFF);  
			break;
		case SSI1:
			SSIIntClear(SSI1_BASE, SSI_TXFF);
			break;
		case SSI2:
			SSIIntClear(SSI2_BASE, SSI_TXFF);
			break;
		case SSI3:
			SSIIntClear(SSI3_BASE, SSI_TXFF);
			break;
		default:
			return;
	}
}

uint8_t hal_SPI_SpaceAvailable(uint8_t channel){
	// This code works directly with the Tiva-C registers, this is only necessary because TivaWare doesn't provide a mechanism to do this
	switch(channel){
		case SSI0:
		    // Checks the "SSI TX FIFO Not Full" bit (bit 1) of SSI Status Register
			return ((HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_TFE)>0)?1:0;
		case SSI1:
			return ((HWREG(SSI1_BASE + SSI_O_SR) & SSI_SR_TFE)>0)?1:0;
		case SSI2:
			return ((HWREG(SSI2_BASE + SSI_O_SR) & SSI_SR_TFE)>0)?1:0;
		case SSI3:
			return ((HWREG(SSI3_BASE + SSI_O_SR) & SSI_SR_TFE)>0)?1:0;
		default:
			return 0;
	}
}

uint8_t hal_SPI_DataAvailable(uint8_t channel){
	// This code works directly with the Tiva-C registers, this is only necessary because TivaWare doesn't provide a mechanism to do this
	switch(channel){
		case SSI0:
			// Checks the "SSI RX FIFO Not Empty" bit (bit 2) of SSI Status Register 
			return ((HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_RNE)>0)?1:0;
		case SSI1:
			return ((HWREG(SSI1_BASE + SSI_O_SR) & SSI_SR_RNE)>0)?1:0;
		case SSI2:
			return ((HWREG(SSI2_BASE + SSI_O_SR) & SSI_SR_RNE)>0)?1:0;
		case SSI3:
			return ((HWREG(SSI3_BASE + SSI_O_SR) & SSI_SR_RNE)>0)?1:0;
		default:
			return 0;
	}
}

// This will return 1 if the receive FIFO has overrun
uint8_t hal_SPI_OverrunError(uint8_t channel) {
	switch(channel){
		case SSI0:
			// Check the raw (unmasked) interrupt status for RF FIFO Overrun
			return ((HWREG(SSI0_BASE + SSI_O_RIS) & SSI_RIS_RORRIS)>0)?1:0;
		case SSI1:
			return ((HWREG(SSI1_BASE + SSI_O_RIS) & SSI_RIS_RORRIS)>0)?1:0;
		case SSI2:
			return ((HWREG(SSI2_BASE + SSI_O_RIS) & SSI_RIS_RORRIS)>0)?1:0;
		case SSI3:
			return ((HWREG(SSI3_BASE + SSI_O_RIS) & SSI_RIS_RORRIS)>0)?1:0;
		default:
			return 0;
	}
}

void SPI_ISR0(void){
	SPI_ISR(SSI0);
}

void SPI_ISR1(void){
	SPI_ISR(SSI1);
}

void SPI_ISR2(void){
	SPI_ISR(SSI2);
}

void SPI_ISR3(void){
	SPI_ISR(SSI3);
}
