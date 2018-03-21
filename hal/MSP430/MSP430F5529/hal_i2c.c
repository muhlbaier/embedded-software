#include <msp430.h>
#include <stdint.h>
#include "project_settings.h"
#include "i2c.h"

typedef enum state_t {
    IDLE = 0,
    WRITING,
    READING,
    STOPPING_SUCCESS,
    STOPPING_FAIL,
    ACK,
    NACK
} state_t;

state_t state[2];

void hal_I2C_Init(i2c_settings_t* settings){
	hal_I2C_Disable(settings->channel);
	switch(settings->channel){
		case I2C_B0:
			P3SEL |= BIT0 + BIT1;
			UCB0CTL0 |= UCMODE_3 + UCSYNC + UCMST;
			UCB0CTL0 |= (settings->address_length) ? (UCSLA10) : 0;
			UCB0CTL1 |= UCSSEL_2;
			// Configure bit rate
			UCB0BR0 = FCPU / settings->bit_rate;
			UCB0BR1 = FCPU / settings->bit_rate / 256;
			break;
		case I2C_B1:
#warning "fix the i2c pins for this channel"
			UCB1CTL0 |= UCMODE_3 + UCSYNC;;
			UCB1CTL0 |= (settings->address_length) ? (UCSLA10) : 0;
			UCB1CTL1 |= UCSSEL_2;
			// Configure bit rate
			UCB1BR0 = FCPU / settings->bit_rate;
			UCB1BR1 = FCPU / settings->bit_rate / 256;
			break;
		default:
			return;
	}
	hal_I2C_Enable(settings->channel);
	hal_I2C_EnableInterrupts(settings->channel);
}

void hal_I2C_Enable(uint8_t channel){
	switch(channel){
		case I2C_B0:
			UCB0CTL1 &= ~(UCSWRST);
			break;
		case I2C_B1:
			UCB1CTL1 &= ~(UCSWRST);
			break;
		default:
			return;
	}
}

void hal_I2C_Disable(uint8_t channel){
	switch(channel){
		case I2C_B0:
			UCB0CTL1 |= UCSWRST;
			break;
		case I2C_B1:
			UCB1CTL1 |= UCSWRST;
			break;
		default:
			return;
	}
}

void hal_I2C_EnableInterrupts(uint8_t channel){
	switch(channel){
		case I2C_B0:
			UCB0IE |= UCRXIE + UCTXIE + UCNACKIE;
			break;
		case I2C_B1:
			UCB1IE |= UCRXIE + UCTXIE + UCNACKIE;
			break;
		default:
			return;
	}
}

void hal_I2C_DisableInterrupts(uint8_t channel){
	switch(channel){
		case I2C_B0:
			UCB0IE = 0;
			break;
		case I2C_B1:
			UCB1IE = 0;
			break;
		default:
			return;
	}
}

void hal_I2C_SendStart(uint8_t channel){
	switch(channel){
		case I2C_B0:
#warning "This needs to change to support 10-bit addresses"
			UCB0I2CSA = I2C_GetSlaveAddress(channel);
			// If there is data to write
			if(I2C_GetTxSize(channel)){
				state[I2C_B0] = WRITING;
				// Set to transmitter mode and set start bit
				UCB0CTL1 |= UCTR + UCTXSTT;
			} else if(I2C_GetRxSize(channel)) {	// If this is a receive only command
				// Set to receiver mode
				UCB0CTL1 &= ~UCTR;
				state[I2C_B0] = READING;
				/* With the MSP430 I2C module, the stop bit flag must be set
				 * prior to the reception of the last byte in the transaction.
				 * If the STP flag is not set until after reception of the last byte,
				 * than the STP flag would need to be set prior to reading the byte
				 * from the buffer.  Otherwise, an extra byte would be received.
				 *
				 * Therefore, if there is only a single byte to read, we have to poll
				 * the STT flag so that we know as soon as the start condition has been
				 * set so we can apply a stop condition
				 */
				if(I2C_GetRxSize(I2C_B0) == 1){
					// Put a start condition on the bus
					UCB0CTL1 |= UCTXSTT;
					// Wait for the start flag to be asserted
					while(UCB0CTL1 & UCTXSTT);
					// Put a stop bit on the bus
					UCB0CTL1 |= UCTXSTP;
				} else {
					// Put a start condition on the bus
					UCB0CTL1 |= UCTXSTT;
				}
			} else {
				// Say the transaction succeeded since there was nothing to do
				I2C_TransactionSuccess(I2C_B0);
			}
			break;
		case I2C_B1:

			break;
		default:
			return;
	}
}

#pragma vector=USCI_B0_VECTOR
__interrupt void _I2C_B0_ISR(void){
	switch(UCB0IV){
		case 0x04: // Nack
			//If we received a NACK, the transaction has failed
			state[I2C_B0] = IDLE;
			I2C_TransactionFail(I2C_B0);
			break;
		case 0x0A: //Rx
			if(state[I2C_B0] == READING){
				I2C_Rx_Handler(I2C_B0, UCB0RXBUF);
				if(I2C_GetRxSize(I2C_B0) == 1){
					// Generate a stop bit
					UCB0CTL1 |= UCTXSTP;
				}
				if(I2C_GetRxSize(I2C_B0) == 0){
					state[I2C_B0] = IDLE;
					I2C_TransactionSuccess(I2C_B0);
				}
			}
			break;
		case 0x0C: //Tx
			if(state[I2C_B0] == WRITING){
				if(I2C_GetTxSize(I2C_B0)){
					UCB0TXBUF = I2C_GetTxByte(I2C_B0);
				} else {
					if(I2C_GetRxSize(I2C_B0)){
						state[I2C_B0] = READING;
						// Put the module in receive mode
						UCB0CTL1 &= ~UCTR;
						if(I2C_GetRxSize(I2C_B0) == 1){
							// Put a repeated start condition on the bus
							UCB0CTL1 |= UCTXSTT;
							while(UCB0CTL1 & UCTXSTT);
							// Generate a stop bit
							UCB0CTL1 |= UCTXSTP;
						} else {
							// Put a repeated start condition on the bus
							UCB0CTL1 |= UCTXSTT;
						}
					} else {
						// Generate STOP bit
						UCB0CTL1 |= UCTXSTP;
						state[I2C_B0] = IDLE;
						I2C_TransactionSuccess(I2C_B0);
						return;
					}
				}
			}
			break;
	}
}

#pragma vector=USCI_B1_VECTOR
__interrupt void _I2CB1_ISR(void){

}
