#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "project_settings.h"
#include "hal_i2c.h"

typedef enum state_t {
    IDLE = 0,
    WRITE_NEXT,
    WRITE_FINISH,
    START_READ,
    READ_NEXT,
    READ_FINISH,
} state_t;

state_t state[4];

void I2C0_IntHandler(void);
void I2C1_IntHandler(void);
void I2C2_IntHandler(void);
void I2C3_IntHandler(void);

void hal_I2C_Init(i2c_settings_t* settings){

	switch(settings->channel){
		case I2C0:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
			GPIOPinConfigure(GPIO_PB2_I2C0SCL);
			GPIOPinConfigure(GPIO_PB3_I2C0SDA);
			GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
			GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

			I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
			I2CIntRegister(I2C0_BASE, I2C0_IntHandler);
			I2CMasterIntEnable(I2C0_BASE);
			break;
		case I2C1:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
			GPIOPinConfigure(GPIO_PA6_I2C1SCL);
			GPIOPinConfigure(GPIO_PA7_I2C1SDA);
			GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
			GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

			I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);
			I2CIntRegister(I2C1_BASE, I2C1_IntHandler);
			I2CMasterIntEnable(I2C1_BASE);
			break;
		case I2C2:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
			GPIOPinConfigure(GPIO_PE4_I2C2SCL);
			GPIOPinConfigure(GPIO_PE5_I2C2SDA);
			GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
			GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

			I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);
			I2CIntRegister(I2C2_BASE, I2C2_IntHandler);
			I2CMasterIntEnable(I2C2_BASE);
			break;
		case I2C3:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
			GPIOPinConfigure(GPIO_PD0_I2C3SCL);
			GPIOPinConfigure(GPIO_PD1_I2C3SDA);
			GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
			GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

			I2CMasterInitExpClk(I2C3_BASE, SysCtlClockGet(), false);
			I2CIntRegister(I2C3_BASE, I2C3_IntHandler);
			I2CMasterIntEnable(I2C3_BASE);
			break;
	}
}

void hal_I2C_SendStart(uint8_t channel){
	switch(channel){
		case I2C0:
			if(I2C_GetTxSize(channel)){ // If there is data to write
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C0_BASE, I2C_GetSlaveAddress(channel), false);
				// Put the byte in the transmit fifo
				I2CMasterDataPut(I2C0_BASE, I2C_GetTxByte(channel));
				if(I2C_GetTxSize(channel) == 0){ //If there is only one byte to write
					if(!I2C_GetRxSize(channel)){ // If there is nothing to read
						state[channel] = WRITE_FINISH;
					    I2CMasterControl(I2C0_BASE, I2C_MCS_START |  I2C_MCS_RUN | I2C_MCS_STOP);
					} else {
						state[channel] = START_READ;
						I2CMasterControl(I2C0_BASE, I2C_MCS_START |  I2C_MCS_RUN);
					}
				} else {// If there are multiple bytes to write
					state[channel] = WRITE_NEXT;
					I2CMasterControl(I2C0_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				}
			} else if (I2C_GetRxSize(channel)) { // If there is only data to read
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C0_BASE, I2C_GetSlaveAddress(channel), true);
				if(I2C_GetRxSize(channel) == 1){ // If there is only 1 byte of data to read
					state[channel] = READ_FINISH;
					I2CMasterControl(I2C0_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				} else { // If there is more than 1 byte to read
					state[channel] = READ_NEXT;
					I2CMasterControl(I2C0_BASE, I2C_MCS_ACK | I2C_MCS_START |  I2C_MCS_RUN);
				}
			}
			break;
		case I2C1:
			if(I2C_GetTxSize(channel)){ // If there is data to write
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C1_BASE, I2C_GetSlaveAddress(channel), false);
				// Put the byte in the transmit fifo
				I2CMasterDataPut(I2C1_BASE, I2C_GetTxByte(channel));
				if(I2C_GetTxSize(channel) == 0){ //If there is only one byte to write
					if(!I2C_GetRxSize(channel)){ // If there is nothing to read
						state[channel] = WRITE_FINISH;
						I2CMasterControl(I2C1_BASE, I2C_MCS_START |  I2C_MCS_RUN | I2C_MCS_STOP);
					} else {
						state[channel] = START_READ;
						I2CMasterControl(I2C1_BASE, I2C_MCS_START |  I2C_MCS_RUN);
					}
				} else {// If there are multiple bytes to write
					state[channel] = WRITE_NEXT;
					I2CMasterControl(I2C1_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				}
			} else if (I2C_GetRxSize(channel)) { // If there is only data to read
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C1_BASE, I2C_GetSlaveAddress(channel), true);
				if(I2C_GetRxSize(channel) == 1){ // If there is only 1 byte of data to read
					state[channel] = READ_FINISH;
					I2CMasterControl(I2C1_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				} else { // If there is more than 1 byte to read
					state[channel] = READ_NEXT;
					I2CMasterControl(I2C1_BASE, I2C_MCS_ACK | I2C_MCS_START |  I2C_MCS_RUN);
				}
			}
			break;
		case I2C2:
			if(I2C_GetTxSize(channel)){ // If there is data to write
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C2_BASE, I2C_GetSlaveAddress(channel), false);
				// Put the byte in the transmit fifo
				I2CMasterDataPut(I2C2_BASE, I2C_GetTxByte(channel));
				if(I2C_GetTxSize(channel) == 0){ //If there is only one byte to write
					if(!I2C_GetRxSize(channel)){ // If there is nothing to read
						state[channel] = WRITE_FINISH;
						I2CMasterControl(I2C2_BASE, I2C_MCS_START |  I2C_MCS_RUN | I2C_MCS_STOP);
					} else {
						state[channel] = START_READ;
						I2CMasterControl(I2C2_BASE, I2C_MCS_START |  I2C_MCS_RUN);
					}
				} else {// If there are multiple bytes to write
					state[channel] = WRITE_NEXT;
					I2CMasterControl(I2C2_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				}
			} else if (I2C_GetRxSize(channel)) { // If there is only data to read
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C2_BASE, I2C_GetSlaveAddress(channel), true);
				if(I2C_GetRxSize(channel) == 1){ // If there is only 1 byte of data to read
					state[channel] = READ_FINISH;
					I2CMasterControl(I2C2_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				} else { // If there is more than 1 byte to read
					state[channel] = READ_NEXT;
					I2CMasterControl(I2C2_BASE, I2C_MCS_ACK | I2C_MCS_START |  I2C_MCS_RUN);
				}
			}
			break;
		case I2C3:
			if(I2C_GetTxSize(channel)){ // If there is data to write
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C3_BASE, I2C_GetSlaveAddress(channel), false);
				// Put the byte in the transmit fifo
				I2CMasterDataPut(I2C3_BASE, I2C_GetTxByte(channel));
				if(I2C_GetTxSize(channel) == 0){ //If there is only one byte to write
					if(!I2C_GetRxSize(channel)){ // If there is nothing to read
						state[channel] = WRITE_FINISH;
						I2CMasterControl(I2C3_BASE, I2C_MCS_START |  I2C_MCS_RUN | I2C_MCS_STOP);
					} else {
						state[channel] = START_READ;
						I2CMasterControl(I2C3_BASE, I2C_MCS_START |  I2C_MCS_RUN);
					}
				} else {// If there are multiple bytes to write
					state[channel] = WRITE_NEXT;
					I2CMasterControl(I2C3_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				}
			} else if (I2C_GetRxSize(channel)) { // If there is only data to read
				// Set the address and write
				I2CMasterSlaveAddrSet(I2C3_BASE, I2C_GetSlaveAddress(channel), true);
				if(I2C_GetRxSize(channel) == 1){ // If there is only 1 byte of data to read
					state[channel] = READ_FINISH;
					I2CMasterControl(I2C3_BASE, I2C_MCS_START |  I2C_MCS_RUN);
				} else { // If there is more than 1 byte to read
					state[channel] = READ_NEXT;
					I2CMasterControl(I2C3_BASE, I2C_MCS_ACK | I2C_MCS_START |  I2C_MCS_RUN);
				}
			}
			break;
	}
}

void I2C0_IntHandler(void) {

	// Clear the I2C interrupt.
    I2CMasterIntClear(I2C0_BASE);

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE){
    	state[I2C0] = IDLE;
    	I2C_TransactionFail(I2C0);
    	return;
    }

    switch(state[I2C0])
    {
        case WRITE_NEXT:
            // Write the next byte to the data register.
            I2CMasterDataPut(I2C0_BASE, I2C_GetTxByte(I2C0));
            if(I2C_GetTxSize(I2C0) == 0){ //If this is the last byte
				if(!I2C_GetRxSize(I2C0)){ // If there is nothing to read
					state[I2C0] = WRITE_FINISH;
					I2CMasterControl(I2C0_BASE, I2C_MCS_RUN | I2C_MCS_STOP);
				} else {
					state[I2C0] = START_READ;
					I2CMasterControl(I2C0_BASE, I2C_MCS_RUN);
				}
			} else {// If there are more bytes to write
				state[I2C0] = WRITE_NEXT;
				I2CMasterControl(I2C0_BASE, I2C_MCS_RUN);
			};
            break;
        case WRITE_FINISH:
        	state[I2C0] = IDLE;
			I2C_TransactionSuccess(I2C0);
            break;
        case START_READ:
            // Put the I2C master into receive mode.
            I2CMasterSlaveAddrSet(I2C0_BASE, I2C_GetSlaveAddress(I2C0), true);
            if(I2C_GetRxSize(I2C0) == 1) { // If there is only one byte to read
            	// Repeated Start followed by receive with NACK from master to end read
            	state[I2C0] = READ_FINISH;
				I2CMasterControl(I2C0_BASE, I2C_MCS_START |  I2C_MCS_RUN);
            } else {
            	state[I2C0] = READ_NEXT;
            	I2CMasterControl(I2C0_BASE, I2C_MCS_START | I2C_MCS_ACK | I2C_MCS_RUN);
            }
            break;
        case READ_NEXT:
            // Read the received character.
        	I2C_Rx_Handler(I2C0, I2CMasterDataGet(I2C0_BASE));
        	if(I2C_GetRxSize(I2C0) == 1) { // If there is only one byte left to read
				// Repeated Start followed by receive with NACK from master to end read
        		state[I2C0] = READ_FINISH;
				I2CMasterControl(I2C0_BASE, I2C_MCS_RUN);
			} else {
				state[I2C0] = READ_NEXT;
				I2CMasterControl(I2C0_BASE, I2C_MCS_ACK | I2C_MCS_RUN);
			}
            break;
        case READ_FINISH:
            // Read the received character.
        	I2C_Rx_Handler(I2C0, I2CMasterDataGet(I2C0_BASE));
        	state[I2C0] = IDLE;
            // Finish the read.
            I2CMasterControl(I2C0_BASE, I2C_MCS_STOP);
            I2C_TransactionSuccess(I2C0);
            break;
    }
}

void I2C1_IntHandler(void) {

	// Clear the I2C interrupt.
    I2CMasterIntClear(I2C1_BASE);

    if(I2CMasterErr(I2C1_BASE) != I2C_MASTER_ERR_NONE){
    	state[I2C1] = IDLE;
    	I2C_TransactionFail(I2C1);
    	return;
    }

    switch(state[I2C1])
    {
        case WRITE_NEXT:
            // Write the next byte to the data register.
            I2CMasterDataPut(I2C1_BASE, I2C_GetTxByte(I2C1));
            if(I2C_GetTxSize(I2C1) == 0){ //If this is the last byte
				if(!I2C_GetRxSize(I2C1)){ // If there is nothing to read
					state[I2C1] = WRITE_FINISH;
					I2CMasterControl(I2C1_BASE, I2C_MCS_RUN | I2C_MCS_STOP);
				} else {
					state[I2C1] = START_READ;
					I2CMasterControl(I2C1_BASE, I2C_MCS_RUN);
				}
			} else {// If there are more bytes to write
				state[I2C1] = WRITE_NEXT;
				I2CMasterControl(I2C1_BASE, I2C_MCS_RUN);
			};
            break;
        case WRITE_FINISH:
        	state[I2C1] = IDLE;
			I2C_TransactionSuccess(I2C1);
            break;
        case START_READ:
            // Put the I2C master into receive mode.
            I2CMasterSlaveAddrSet(I2C1_BASE, I2C_GetSlaveAddress(I2C1), true);
            if(I2C_GetRxSize(I2C1) == 1) { // If there is only one byte to read
            	// Repeated Start followed by receive with NACK from master to end read
            	state[I2C1] = READ_FINISH;
				I2CMasterControl(I2C1_BASE, I2C_MCS_START |  I2C_MCS_RUN);
            } else {
            	state[I2C1] = READ_NEXT;
            	I2CMasterControl(I2C1_BASE, I2C_MCS_START | I2C_MCS_ACK | I2C_MCS_RUN);
            }
            break;
        case READ_NEXT:
            // Read the received character.
        	I2C_Rx_Handler(I2C1, I2CMasterDataGet(I2C1_BASE));
        	if(I2C_GetRxSize(I2C1) == 1) { // If there is only one byte left to read
				// Repeated Start followed by receive with NACK from master to end read
        		state[I2C1] = READ_FINISH;
				I2CMasterControl(I2C1_BASE, I2C_MCS_RUN);
			} else {
				state[I2C1] = READ_NEXT;
				I2CMasterControl(I2C1_BASE, I2C_MCS_ACK | I2C_MCS_RUN);
			}
            break;
        case READ_FINISH:
            // Read the received character.
        	I2C_Rx_Handler(I2C1, I2CMasterDataGet(I2C1_BASE));
        	state[I2C1] = IDLE;
            // Finish the read.
            I2CMasterControl(I2C1_BASE, I2C_MCS_STOP);
            I2C_TransactionSuccess(I2C1);
            break;
    }
}

void I2C2_IntHandler(void) {

	// Clear the I2C interrupt.
    I2CMasterIntClear(I2C2_BASE);

    if(I2CMasterErr(I2C2_BASE) != I2C_MASTER_ERR_NONE){
    	state[I2C2] = IDLE;
    	I2C_TransactionFail(I2C2);
    	return;
    }

    switch(state[I2C2])
    {
        case WRITE_NEXT:
            // Write the next byte to the data register.
            I2CMasterDataPut(I2C2_BASE, I2C_GetTxByte(I2C2));
            if(I2C_GetTxSize(I2C2) == 0){ //If this is the last byte
				if(!I2C_GetRxSize(I2C2)){ // If there is nothing to read
					state[I2C2] = WRITE_FINISH;
					I2CMasterControl(I2C2_BASE, I2C_MCS_RUN | I2C_MCS_STOP);
				} else {
					state[I2C2] = START_READ;
					I2CMasterControl(I2C2_BASE, I2C_MCS_RUN);
				}
			} else {// If there are more bytes to write
				state[I2C2] = WRITE_NEXT;
				I2CMasterControl(I2C2_BASE, I2C_MCS_RUN);
			};
            break;
        case WRITE_FINISH:
        	state[I2C2] = IDLE;
			I2C_TransactionSuccess(I2C2);
            break;
        case START_READ:
            // Put the I2C master into receive mode.
            I2CMasterSlaveAddrSet(I2C2_BASE, I2C_GetSlaveAddress(I2C2), true);
            if(I2C_GetRxSize(I2C2) == 1) { // If there is only one byte to read
            	// Repeated Start followed by receive with NACK from master to end read
            	state[I2C2] = READ_FINISH;
				I2CMasterControl(I2C2_BASE, I2C_MCS_START |  I2C_MCS_RUN);
            } else {
            	state[I2C2] = READ_NEXT;
            	I2CMasterControl(I2C2_BASE, I2C_MCS_START | I2C_MCS_ACK | I2C_MCS_RUN);
            }
            break;
        case READ_NEXT:
            // Read the received character.
        	I2C_Rx_Handler(I2C2, I2CMasterDataGet(I2C2_BASE));
        	if(I2C_GetRxSize(I2C2) == 1) { // If there is only one byte left to read
				// Repeated Start followed by receive with NACK from master to end read
        		state[I2C2] = READ_FINISH;
				I2CMasterControl(I2C2_BASE, I2C_MCS_RUN);
			} else {
				state[I2C2] = READ_NEXT;
				I2CMasterControl(I2C2_BASE, I2C_MCS_ACK | I2C_MCS_RUN);
			}
            break;
        case READ_FINISH:
            // Read the received character.
        	I2C_Rx_Handler(I2C2, I2CMasterDataGet(I2C2_BASE));
        	state[I2C2] = IDLE;
            // Finish the read.
            I2CMasterControl(I2C2_BASE, I2C_MCS_STOP);
            I2C_TransactionSuccess(I2C2);
            break;
    }
}

void I2C3_IntHandler(void) {

	// Clear the I2C interrupt.
    I2CMasterIntClear(I2C3_BASE);

    if(I2CMasterErr(I2C3_BASE) != I2C_MASTER_ERR_NONE){
    	state[I2C3] = IDLE;
    	I2C_TransactionFail(I2C3);
    	return;
    }

    switch(state[I2C3])
    {
        case WRITE_NEXT:
            // Write the next byte to the data register.
            I2CMasterDataPut(I2C3_BASE, I2C_GetTxByte(I2C3));
            if(I2C_GetTxSize(I2C3) == 0){ //If this is the last byte
				if(!I2C_GetRxSize(I2C3)){ // If there is nothing to read
					state[I2C3] = WRITE_FINISH;
					I2CMasterControl(I2C3_BASE, I2C_MCS_RUN | I2C_MCS_STOP);
				} else {
					state[I2C3] = START_READ;
					I2CMasterControl(I2C3_BASE, I2C_MCS_RUN);
				}
			} else {// If there are more bytes to write
				state[I2C3] = WRITE_NEXT;
				I2CMasterControl(I2C3_BASE, I2C_MCS_RUN);
			};
            break;
        case WRITE_FINISH:
        	state[I2C3] = IDLE;
			I2C_TransactionSuccess(I2C3);
            break;
        case START_READ:
            // Put the I2C master into receive mode.
            I2CMasterSlaveAddrSet(I2C3_BASE, I2C_GetSlaveAddress(I2C3), true);
            if(I2C_GetRxSize(I2C3) == 1) { // If there is only one byte to read
            	// Repeated Start followed by receive with NACK from master to end read
            	state[I2C3] = READ_FINISH;
				I2CMasterControl(I2C3_BASE, I2C_MCS_START |  I2C_MCS_RUN);
            } else {
            	state[I2C3] = READ_NEXT;
            	I2CMasterControl(I2C3_BASE, I2C_MCS_START | I2C_MCS_ACK | I2C_MCS_RUN);
            }
            break;
        case READ_NEXT:
            // Read the received character.
        	I2C_Rx_Handler(I2C3, I2CMasterDataGet(I2C3_BASE));
        	if(I2C_GetRxSize(I2C3) == 1) { // If there is only one byte left to read
				// Repeated Start followed by receive with NACK from master to end read
        		state[I2C3] = READ_FINISH;
				I2CMasterControl(I2C3_BASE, I2C_MCS_RUN);
			} else {
				state[I2C3] = READ_NEXT;
				I2CMasterControl(I2C3_BASE, I2C_MCS_ACK | I2C_MCS_RUN);
			}
            break;
        case READ_FINISH:
            // Read the received character.
        	I2C_Rx_Handler(I2C3, I2CMasterDataGet(I2C3_BASE));
        	state[I2C3] = IDLE;
            // Finish the read.
            I2CMasterControl(I2C3_BASE, I2C_MCS_STOP);
            I2C_TransactionSuccess(I2C3);
            break;
    }
}
