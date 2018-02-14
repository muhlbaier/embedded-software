#include <msp432.h>
#include <stdint.h>

#include "spi.h"
#include "hal_spi.h"

void hal_SPI_Init(spi_settings_t* settings)
{
    // Configuration flow for USCI modules on MSP432P401R
    // 1: Hold reset
    // 2: Configure control registers (CTLW0, BRW)
    // 3: Configure ports
    // 4: Release reset
    // 5: Clear and enable interrupts

    // Hold reset
	hal_SPI_Disable(settings->channel);

	//Configure control registers
	EUSCI_A_SPI_Type* moduleA = 0;
	EUSCI_B_SPI_Type* moduleB = 0;
	switch (settings->channel) {
        case SPI_A0: moduleA = EUSCI_A0_SPI; break;
        case SPI_A1: moduleA = EUSCI_A1_SPI; break;
        case SPI_A2: moduleA = EUSCI_A2_SPI; break;
        case SPI_A3: moduleA = EUSCI_A3_SPI; break;
        case SPI_B0: moduleB = EUSCI_B0_SPI; break;
        case SPI_B1: moduleB = EUSCI_B1_SPI; break;
        case SPI_B2: moduleB = EUSCI_B2_SPI; break;
        case SPI_B3: moduleB = EUSCI_B3_SPI; break;
        default: return;
	}

	if (moduleA) {
	    moduleA->CTLW0 &= ~0xFFC2; //Clear all settings bits except for R and reserved bits
	    moduleA->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK; //Set clock source bits for SMCLK
	    moduleA->CTLW0 |= EUSCI_A_CTLW0_SYNC; //Set to Synchronous mode
	    moduleA->CTLW0 |= EUSCI_A_CTLW0_MODE_0; //Set to 3-wire SPI mode
	    moduleA->CTLW0 |= EUSCI_A_CTLW0_MST; //Set to master mode
	    moduleA->CTLW0 |= (settings->hal_settings.char7bit) ? (EUSCI_A_CTLW0_SEVENBIT) : 0; //Set data mode 7 or 8 bits
	    moduleA->CTLW0 |= (settings->hal_settings.msb_first) ? (EUSCI_A_CTLW0_MSB) : 0; //Set first bit MSB/LSB
	    moduleA->CTLW0 |= (settings->mode & 2) ? (EUSCI_A_CTLW0_CKPL) : 0; //Set clock polarity when resting
	    moduleA->CTLW0 |= (settings->mode & 1) ? (EUSCI_A_CTLW0_CKPH) : 0; //Set clock capture phase
	    moduleA->BRW = FCPU / settings->bit_rate; //Configure bit rate
	} else if (moduleB) {
	    moduleB->CTLW0 &= ~0xFFC2; //Clear all settings bits except for R and reserved bits
        moduleB->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; //Set clock source bits for SMCLK
        moduleB->CTLW0 |= EUSCI_B_CTLW0_SYNC; //Set to Synchronous mode
        moduleB->CTLW0 |= EUSCI_B_CTLW0_MODE_0; //Set to 3-wire SPI mode
        moduleB->CTLW0 |= EUSCI_B_CTLW0_MST; //Set to master mode
        moduleB->CTLW0 |= (settings->hal_settings.char7bit) ? (EUSCI_B_CTLW0_SEVENBIT) : 0; //Set data mode 7 or 8 bits
        moduleB->CTLW0 |= (settings->hal_settings.msb_first) ? (EUSCI_B_CTLW0_MSB) : 0; //Set first bit MSB/LSB
        moduleB->CTLW0 |= (settings->mode & 2) ? (EUSCI_B_CTLW0_CKPL) : 0; //Set clock polarity when resting
        moduleB->CTLW0 |= (settings->mode & 1) ? (EUSCI_B_CTLW0_CKPH) : 0; //Set clock capture phase
        moduleB->BRW = FCPU / settings->bit_rate; //Configure bit rate
	} else
	    return;

	//Configure ports
	switch (settings->channel) {
        case SPI_A0:
            P1SEL0 |=  0x0E; //XXXX 111X
            P1SEL1 &= ~0x0E; //XXXX 000X
            //UCA0_CLK:  P1.1 (HW Pin 4) (SEL 01)
            //UCA0_MOSI: P1.3 (HW Pin 6) (SEL 01)
            //UCA0_MISO: P1.2 (HW Pin 5) (SEL 01)
            break;
        case SPI_A1:
            P2SEL0 |=  0x0E; //XXXX 111X
            P2SEL1 &= ~0x0E; //XXXX 000X
            //UCA1_CLK:  P2.1 (HW Pin 17) (SEL 01)
            //UCA1_MOSI: P2.3 (HW Pin 19) (SEL 01)
            //UCA1_MISO: P2.2 (HW Pin 18) (SEL 01)
            break;
        case SPI_A2:
            P3SEL0 |=  0x0E; //XXXX 111X
            P3SEL1 &= ~0x0E; //XXXX 000X
            //UCA2_CLK:  P3.1 (HW Pin 33) (SEL 01)
            //UCA2_MOSI: P3.3 (HW Pin 35) (SEL 01)
            //UCA2_MISO: P3.2 (HW Pin 34) (SEL 01)
            break;
        case SPI_A3:
            P9SEL0 |=  0x0E; //XXXX 111X
            P9SEL1 &= ~0x0E; //XXXX 000X
            //UCA3_CLK:  P9.5 (HW Pin 97) (SEL 01)
            //UCA3_MOSI: P9.7 (HW Pin 99) (SEL 01)
            //UCA3_MISO: P9.6 (HW Pin 98) (SEL 01)
            break;
        case SPI_B0:
            P1SEL0 |=  0xE0; //111X XXXX
            P1SEL1 &= ~0xE0; //000X XXXX
            //UCB0_CLK:  P1.5 (HW Pin  9) (SEL 01)
            //UCB0_MOSI: P1.6 (HW Pin 10) (SEL 01)
            //UCB0_MISO: P1.7 (HW Pin 11) (SEL 01)
            break;
        case SPI_B1:
            P6SEL0 |=  0x38; //XX11 1XXX
            P6SEL1 &= ~0x38; //XX00 0XXX
            //UCB1_CLK:  P6.3 (HW Pin 77) (SEL 01)
            //UCB1_MOSI: P6.4 (HW Pin 78) (SEL 01)
            //UCB1_MISO: P6.5 (HW Pin 79) (SEL 01)
            break;
        case SPI_B2:
            P3SEL0 |=  0xE0; //111X XXXX
            P3SEL1 &= ~0xE0; //000X XXXX
            //UCB2_CLK:  P3.5 (HW Pin 37) (SEL 01)
            //UCB2_MOSI: P3.6 (HW Pin 38) (SEL 01)
            //UCB2_MISO: P3.7 (HW Pin 39) (SEL 01)
            break;
        case SPI_B3:
            P10SEL0 |=  0x0E; //XXXX 111X
            P10SEL1 &= ~0x0E; //XXXX 000X
            //UCB3_CLK:  P10.1 (HW Pin 1) (SEL 01)
            //UCB3_MOSI: P10.2 (HW Pin 2) (SEL 01)
            //UCB3_MISO: P10.3 (HW Pin 3) (SEL 01)
            break;
    }

	//Release reset
	hal_SPI_Enable(settings->channel);

	//Clear/Enable inturrupts
	hal_SPI_ClearRxIF(settings->channel);
	hal_SPI_EnableRxInterrupt(settings->channel);
	switch (settings->channel) { //Register NVIC flag
        case SPI_A0: NVIC_EnableIRQ(EUSCIA0_IRQn); break;
        case SPI_A1: NVIC_EnableIRQ(EUSCIA1_IRQn); break;
        case SPI_A2: NVIC_EnableIRQ(EUSCIA2_IRQn); break;
        case SPI_A3: NVIC_EnableIRQ(EUSCIA3_IRQn); break;
        case SPI_B0: NVIC_EnableIRQ(EUSCIB0_IRQn); break;
        case SPI_B1: NVIC_EnableIRQ(EUSCIB1_IRQn); break;
        case SPI_B2: NVIC_EnableIRQ(EUSCIB2_IRQn); break;
        case SPI_B3: NVIC_EnableIRQ(EUSCIB3_IRQn); break;
	}
}

void hal_SPI_Enable(uint8_t channel)
{
	switch (channel) {
		case SPI_A0:
			EUSCI_A0_SPI->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
			break;
		case SPI_A1:
		    EUSCI_A1_SPI->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
			break;
		case SPI_A2:
            EUSCI_A2_SPI->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
            break;
		default:
			return;
	}
}

void hal_SPI_Disable(uint8_t channel)
{
	switch (channel) {
        case SPI_A0:
            EUSCI_A0_SPI->CTLW0 |= EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->CTLW0 |= EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->CTLW0 |= EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->CTLW0 |= EUSCI_A_CTLW0_SWRST;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->CTLW0 |= EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->CTLW0 |= EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->CTLW0 |= EUSCI_B_CTLW0_SWRST;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->CTLW0 |= EUSCI_B_CTLW0_SWRST;
            break;
		default:
			return;
	}
}

void hal_SPI_EnableRxInterrupt(uint8_t channel)
{
	switch (channel) {
		case SPI_A0:
			EUSCI_A0_SPI->IE |= EUSCI_A_IE_RXIE;
			break;
		case SPI_A1:
            EUSCI_A1_SPI->IE |= EUSCI_A_IE_RXIE;
            break;
		case SPI_A2:
            EUSCI_A2_SPI->IE |= EUSCI_A_IE_RXIE;
            break;
		case SPI_A3:
            EUSCI_A3_SPI->IE |= EUSCI_A_IE_RXIE;
            break;
		case SPI_B0:
            EUSCI_B0_SPI->IE |= EUSCI_B_IE_RXIE;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IE |= EUSCI_B_IE_RXIE;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IE |= EUSCI_B_IE_RXIE;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IE |= EUSCI_B_IE_RXIE;
            break;
		default:
			return;
	}
}

void hal_SPI_EnableTxInterrupt(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            EUSCI_A0_SPI->IE |= EUSCI_A_IE_TXIE;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->IE |= EUSCI_A_IE_TXIE;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->IE |= EUSCI_A_IE_TXIE;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->IE |= EUSCI_A_IE_TXIE;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->IE |= EUSCI_B_IE_TXIE;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IE |= EUSCI_B_IE_TXIE;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IE |= EUSCI_B_IE_TXIE;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IE |= EUSCI_B_IE_TXIE;
            break;
		default:
			return;
	}
}

void hal_SPI_DisableRxInterrupt(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            EUSCI_A0_SPI->IE &= ~EUSCI_A_IE_RXIE;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->IE &= ~EUSCI_A_IE_RXIE;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->IE &= ~EUSCI_A_IE_RXIE;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->IE &= ~EUSCI_A_IE_RXIE;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->IE &= ~EUSCI_B_IE_RXIE;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IE &= ~EUSCI_B_IE_RXIE;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IE &= ~EUSCI_B_IE_RXIE;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IE &= ~EUSCI_B_IE_RXIE;
            break;
		default:
			return;
	}
}

void hal_SPI_DisableTxInterrupt(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            EUSCI_A0_SPI->IE &= ~EUSCI_A_IE_TXIE;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->IE &= ~EUSCI_A_IE_TXIE;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->IE &= ~EUSCI_A_IE_TXIE;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->IE &= ~EUSCI_A_IE_TXIE;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->IE &= ~EUSCI_B_IE_TXIE;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IE &= ~EUSCI_B_IE_TXIE;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IE &= ~EUSCI_B_IE_TXIE;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IE &= ~EUSCI_B_IE_TXIE;
            break;
		default:
			return;
	}
}

uint8_t hal_SPI_TxIntStatus(uint8_t channel)
{
	switch (channel) {
		case SPI_A0:
			return (EUSCI_A0_SPI->IFG & EUSCI_A_IFG_TXIFG) != 0;
		case SPI_A1:
			return (EUSCI_A1_SPI->IFG & EUSCI_A_IFG_TXIFG) != 0;
		case SPI_A2:
		    return (EUSCI_A2_SPI->IFG & EUSCI_A_IFG_TXIFG) != 0;
		case SPI_A3:
		    return (EUSCI_A3_SPI->IFG & EUSCI_A_IFG_TXIFG) != 0;
		case SPI_B0:
            return (EUSCI_B0_SPI->IFG & EUSCI_B_IFG_TXIFG) != 0;
        case SPI_B1:
            return (EUSCI_B1_SPI->IFG & EUSCI_B_IFG_TXIFG) != 0;
        case SPI_B2:
            return (EUSCI_B2_SPI->IFG & EUSCI_B_IFG_TXIFG) != 0;
        case SPI_B3:
            return (EUSCI_B3_SPI->IFG & EUSCI_B_IFG_TXIFG) != 0;
		default:
			return 0;
	}
}

uint8_t hal_SPI_IsTxIntEnabled(uint8_t channel)
{
	switch (channel) {
		case SPI_A0:
			return (EUSCI_A0_SPI->IE & EUSCI_A_IE_TXIE) != 0;
		case SPI_A1:
            return (EUSCI_A1_SPI->IE & EUSCI_A_IE_TXIE) != 0;
		case SPI_A2:
            return (EUSCI_A2_SPI->IE & EUSCI_A_IE_TXIE) != 0;
		case SPI_A3:
            return (EUSCI_A3_SPI->IE & EUSCI_A_IE_TXIE) != 0;
		case SPI_B0:
            return (EUSCI_B0_SPI->IE & EUSCI_B_IE_TXIE) != 0;
        case SPI_B1:
            return (EUSCI_B1_SPI->IE & EUSCI_B_IE_TXIE) != 0;
        case SPI_B2:
            return (EUSCI_B2_SPI->IE & EUSCI_B_IE_TXIE) != 0;
        case SPI_B3:
            return (EUSCI_B3_SPI->IE & EUSCI_B_IE_TXIE) != 0;
		default:
			return 0;
	}
}

uint8_t hal_SPI_RxIntStatus(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            return (EUSCI_A0_SPI->IFG & EUSCI_A_IFG_RXIFG) != 0;
        case SPI_A1:
            return (EUSCI_A1_SPI->IFG & EUSCI_A_IFG_RXIFG) != 0;
        case SPI_A2:
            return (EUSCI_A2_SPI->IFG & EUSCI_A_IFG_RXIFG) != 0;
        case SPI_A3:
            return (EUSCI_A3_SPI->IFG & EUSCI_A_IFG_RXIFG) != 0;
        case SPI_B0:
            return (EUSCI_B0_SPI->IFG & EUSCI_B_IFG_RXIFG) != 0;
        case SPI_B1:
            return (EUSCI_B1_SPI->IFG & EUSCI_B_IFG_RXIFG) != 0;
        case SPI_B2:
            return (EUSCI_B2_SPI->IFG & EUSCI_B_IFG_RXIFG) != 0;
        case SPI_B3:
            return (EUSCI_B3_SPI->IFG & EUSCI_B_IFG_RXIFG) != 0;
		default:
			return 0;
	}
}

void hal_SPI_TxByte(uint8_t channel, uint8_t b)
{
	switch (channel) {
		case SPI_A0:
			EUSCI_A0_SPI->TXBUF = b;
			break;
		case SPI_A1:
            EUSCI_A1_SPI->TXBUF = b;
			break;
		case SPI_A2:
            EUSCI_A2_SPI->TXBUF = b;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->TXBUF = b;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->TXBUF = b;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->TXBUF = b;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->TXBUF = b;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->TXBUF = b;
            break;
		default:
			return;
	}
}

uint8_t hal_SPI_RxByte(uint8_t channel)
{
	switch (channel) {
        case SPI_A0:
            return EUSCI_A0_SPI->RXBUF;
        case SPI_A1:
            return EUSCI_A1_SPI->RXBUF;
        case SPI_A2:
            return EUSCI_A2_SPI->RXBUF;
        case SPI_A3:
            return EUSCI_A3_SPI->RXBUF;
        case SPI_B0:
            return EUSCI_B0_SPI->RXBUF;
        case SPI_B1:
            return EUSCI_B1_SPI->RXBUF;
        case SPI_B2:
            return EUSCI_B2_SPI->RXBUF;
        case SPI_B3:
            return EUSCI_B3_SPI->RXBUF;
		default:
			return 0;
	}
}

void hal_SPI_ClearRxIF(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            EUSCI_A0_SPI->IFG &= ~EUSCI_A_IFG_RXIFG;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->IFG &= ~EUSCI_A_IFG_RXIFG;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->IFG &= ~EUSCI_A_IFG_RXIFG;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->IFG &= ~EUSCI_A_IFG_RXIFG;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->IFG &= ~EUSCI_B_IFG_RXIFG;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IFG &= ~EUSCI_B_IFG_RXIFG;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IFG &= ~EUSCI_B_IFG_RXIFG;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IFG &= ~EUSCI_B_IFG_RXIFG;
            break;
		default:
			return;
	}
}

void hal_SPI_ClearTxIF(uint8_t channel)
{
	switch (channel) {
	    case SPI_A0:
            EUSCI_A0_SPI->IFG &= ~EUSCI_A_IFG_TXIFG;
            break;
        case SPI_A1:
            EUSCI_A1_SPI->IFG &= ~EUSCI_A_IFG_TXIFG;
            break;
        case SPI_A2:
            EUSCI_A2_SPI->IFG &= ~EUSCI_A_IFG_TXIFG;
            break;
        case SPI_A3:
            EUSCI_A3_SPI->IFG &= ~EUSCI_A_IFG_TXIFG;
            break;
        case SPI_B0:
            EUSCI_B0_SPI->IFG &= ~EUSCI_B_IFG_TXIFG;
            break;
        case SPI_B1:
            EUSCI_B1_SPI->IFG &= ~EUSCI_B_IFG_TXIFG;
            break;
        case SPI_B2:
            EUSCI_B2_SPI->IFG &= ~EUSCI_B_IFG_TXIFG;
            break;
        case SPI_B3:
            EUSCI_B3_SPI->IFG &= ~EUSCI_B_IFG_TXIFG;
            break;
		default:
			return;
	}
}

uint8_t hal_SPI_SpaceAvailable(uint8_t channel)
{
	switch (channel) {
		case SPI_A0:
			return EUSCI_A0_SPI->IFG & EUSCI_A_IFG_TXIFG;
		case SPI_A1:
			return EUSCI_A1_SPI->IFG & EUSCI_A_IFG_TXIFG;
		case SPI_A2:
            return EUSCI_A2_SPI->IFG & EUSCI_A_IFG_TXIFG;
        case SPI_A3:
            return EUSCI_A3_SPI->IFG & EUSCI_A_IFG_TXIFG;
        case SPI_B0:
            return EUSCI_B0_SPI->IFG & EUSCI_B_IFG_TXIFG;
        case SPI_B1:
            return EUSCI_B1_SPI->IFG & EUSCI_B_IFG_TXIFG;
        case SPI_B2:
            return EUSCI_B2_SPI->IFG & EUSCI_B_IFG_TXIFG;
        case SPI_B3:
            return EUSCI_B3_SPI->IFG & EUSCI_B_IFG_TXIFG;
		default:
			return 0;
	}
}

uint8_t hal_SPI_DataAvailable(uint8_t channel)
{
	switch(channel) {
	    case SPI_A0:
            return EUSCI_A0_SPI->IFG & EUSCI_A_IFG_RXIFG;
        case SPI_A1:
            return EUSCI_A1_SPI->IFG & EUSCI_A_IFG_RXIFG;
        case SPI_A2:
            return EUSCI_A2_SPI->IFG & EUSCI_A_IFG_RXIFG;
        case SPI_A3:
            return EUSCI_A3_SPI->IFG & EUSCI_A_IFG_RXIFG;
        case SPI_B0:
            return EUSCI_B0_SPI->IFG & EUSCI_B_IFG_RXIFG;
        case SPI_B1:
            return EUSCI_B1_SPI->IFG & EUSCI_B_IFG_RXIFG;
        case SPI_B2:
            return EUSCI_B2_SPI->IFG & EUSCI_B_IFG_RXIFG;
        case SPI_B3:
            return EUSCI_B3_SPI->IFG & EUSCI_B_IFG_RXIFG;
		default:
			return 0;
	}
}

uint8_t hal_SPI_OverrunError(uint8_t channel)
{
    switch(channel) {
        case SPI_A0:
            return (EUSCI_A0_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_A1:
            return (EUSCI_A1_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_A2:
            return (EUSCI_A2_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_A3:
            return (EUSCI_A3_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_B0:
            return (EUSCI_B0_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_B1:
            return (EUSCI_B1_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_B2:
            return (EUSCI_B2_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        case SPI_B3:
            return (EUSCI_B3_SPI->STATW & EUSCI_A_STATW_OE) != 0;
        default:
            return 0;
    }
}

#ifdef USE_SPI_A0
void EUSCIA0_IRQHandler(void) {
	SPI_ISR(SPI_A0);
}
#endif

#ifdef USE_SPI_A1
void EUSCIA1_IRQHandler(void) {
	SPI_ISR(SPI_A1);
}
#endif

#ifdef USE_SPI_A2
void EUSCIA2_IRQHandler(void) {
    SPI_ISR(SPI_A2);
}
#endif

#ifdef USE_SPI_A3
void EUSCIA3_IRQHandler(void) {
    SPI_ISR(SPI_A3);
}
#endif

#ifdef USE_SPI_B0
void EUSCIB0_IRQHandler(void) {
    SPI_ISR(SPI_B0);
}
#endif

#ifdef USE_SPI_B1
void EUSCIB1_IRQHandler(void) {
    SPI_ISR(SPI_B1);
}
#endif

#ifdef USE_SPI_B2
void EUSCIB2_IRQHandler(void) {
    SPI_ISR(SPI_B2);
}
#endif

#ifdef USE_SPI_B3
void EUSCIB3_IRQHandler(void) {
    SPI_ISR(SPI_B3);
}
#endif
