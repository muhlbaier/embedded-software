#include <stdint.h>
#include <stdbool.h>
#include "project_settings.h"
#include "nrf24.h"
#include "spi.h"
#include "timing.h"

void nRF24_Init(nrf24_t * nrf_object){

	// Set the CSN high to start
	nrf_object->csn(1);

	// Set the CE low, power down mode
	nrf_object->ce(0);

	// wait for 'power on reset'
	DelayMs(5);

	// Some initial values - Longest at first so that there are less issues
	nRF24_SetRetries(nrf_object, 15, 15);

	// Set maximum output power
	nRF24_SetPowerAmplificationLevel(nrf_object, NRF24_PA_MAX);

	// Must match on both ends
	nRF24_SetDataRate(nrf_object, NRF24_2MBPS);

	// Must match on both ends
	nRF24_SetCRCMode(nrf_object, NRF24_CRC_16BIT);

	// Must match on both ends
	// When using 2MBPS datarate the channels should be at least 2MHz apart
	nRF24_SetChannel(nrf_object, 76);

	// Activate features - otherwise we can't modify the FEATURES registry
	// 0x73 is a magic number from the datasheet
//	nRF24_WriteReg(nrf_object, ACTIVATE, 0x73);

	// to keep things simple we only support dynamic payloads so enable this feature with payloads
	// allow no-ack payloads too for broadcast
	// and of course payloads in ACK packets
	nRF24_WriteReg(nrf_object, FEATURE, EN_DPL | EN_ACK_PAY | EN_DYN_ACK);

	nRF24_ReadReg(nrf_object, FEATURE);

	// enable auto ack on all pipes, required for dynamic payloads
	nRF24_WriteReg(nrf_object, EN_AA, ENAA_P0 | ENAA_P1 | ENAA_P2 | ENAA_P3 | ENAA_P4 | ENAA_P5);

	// enable dynamic payload on all pipes
	nRF24_WriteReg(nrf_object, DYNPD, DPL_P0 | DPL_P1 | DPL_P2 | DPL_P3 | DPL_P4 | DPL_P5);

	// set address width to 5 bytes
	nRF24_WriteReg(nrf_object, SETUP_AW, 0x3);

	// clear interrupt flags
	nRF24_WriteReg(nrf_object, STATUS, nRF24_ReadReg(nrf_object, STATUS) | RX_DR | TX_DS | MAX_RT);

	// save power until needed
	nRF24_SetActive(nrf_object, false);

	// Clear any pending data
	nRF24_FlushRx(nrf_object);
	nRF24_FlushTx(nrf_object);

	// Assume default settings
	nrf_object->settings.aw = AW_5B;
}

void nRF24_StartListening(nrf24_t * nrf_object) {
	nRF24_Standby(nrf_object);
	nRF24_WriteReg(nrf_object, CONFIG, nRF24_ReadReg(nrf_object, CONFIG) | PWR_UP | PRIM_RX);
	nrf_object->settings.prim_rx = 1;
	nRF24_WriteReg(nrf_object, STATUS, RX_DR | TX_DS | MAX_RT);

	// Restore the pipe0 address, if exists
	if (nrf_object->rx_address_p0) {
		nRF24_WriteMultReg(nrf_object, RX_ADDR_P0, (uint8_t *)&nrf_object->rx_address_p0, nrf_object->settings.aw + 2);
	}

	nrf_object->ce(1);

	// wait for the radio to come up (130us actually only needed)
    DelayUs(130);
	//DelayMs(1);
}

void nRF24_Write(nrf24_t * nrf_object, uint8_t * buf, uint8_t length){
	// Transmitter power-up
	nRF24_WriteReg(nrf_object, CONFIG, (nRF24_ReadReg(nrf_object, CONFIG) | PWR_UP ) & ~PRIM_RX );
	nrf_object->settings.prim_rx = 0;
	// Only needs to be 150uS
    DelayUs(150);
	//DelayMs(1);

	// Send the payload
	nRF24_WriteMultReg(nrf_object, W_TX_PAYLOAD, buf, length);

	nrf_object->ce(1);
	// Only needs to be 15us
	//DelayMs(1);
    DelayUs(15);
	nrf_object->ce(0);
}

void nRF24_WriteAck(nrf24_t * nrf_object, uint8_t * buf, uint8_t length, uint8_t pipe){
	spi_transaction_t transaction;
	uint8_t i;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = length + 1;
	transaction.readLength = 0;
	transaction.data[0] = W_ACK_PAYLOAD | (pipe & 0x07);
	for(i = 1; i <= length; i++){
		transaction.data[i] = *buf++;
	}
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
}

void nRF24_Standby(nrf24_t * nrf_object) {
	nrf_object->ce(0);
	// Flush buffers
	nRF24_FlushRx(nrf_object);
	nRF24_FlushTx(nrf_object);
}

void nRF24_SetPowerAmplificationLevel(nrf24_t * nrf_object, nrf24_pa_level_e level){
	uint8_t rfSetup = nRF24_ReadReg(nrf_object, RF_SETUP);

	// clear previous bits
	rfSetup &= ~(0x6); // low
	rfSetup |= (level << 1) & 0x6;
	// store new config
	nRF24_WriteReg(nrf_object, RF_SETUP, rfSetup);
}

void nRF24_SetRetries(nrf24_t * nrf_object, uint8_t delay, uint8_t count){
	nRF24_WriteReg(nrf_object, SETUP_RETR, (delay & 0x0F) << 4 | (count & 0x0F));
}

void nRF24_SetDataRate(nrf24_t * nrf_object, nrf24_datarate_e rate){
	uint8_t rfSetup = nRF24_ReadReg(nrf_object, RF_SETUP);

	// note: this setup works for NRF24L01+ only (not without +)

	// clear previous. this results in 1mbps data rate
	rfSetup &= ~(RF_DR_HIGH | RF_DR_LOW);

	if (rate == NRF24_250KBPS)
	{
		rfSetup |= RF_DR_LOW;
	} else if (rate == NRF24_2MBPS)
	{
		rfSetup |= RF_DR_HIGH;
	}
	nRF24_WriteReg(nrf_object, RF_SETUP, rfSetup);
}

void nRF24_SetCRCMode(nrf24_t * nrf_object, nrf24_crc_mode_e mode){
	uint8_t config = nRF24_ReadReg(nrf_object, CONFIG);
	config &= ~EN_CRC;
	config &= ~CRCO;
	if (mode == NRF24_NO_CRC) return;
	config |= EN_CRC;
	if (mode == NRF24_CRC_16BIT) config |= CRCO;
	nRF24_WriteReg(nrf_object, CONFIG, config);
}

void nRF24_SetChannel(nrf24_t * nrf_object, uint8_t channel){
	nRF24_WriteReg(nrf_object, RF_CH, channel & 0x7F);
}

void nRF24_OpenRxPipe(nrf24_t * nrf_object, uint8_t pipeNum, uint64_t address){

	// Make sure the pipe number is valid
	if(pipeNum > 5) return;

	// If the pipe to be setup is 0, we need to back this up so that the
	// pipes can be automatically restored after a Tx
	if (pipeNum == 0) {
		nrf_object->rx_address_p0 = address;
	}

	// If the pipeNum is 2-5, the address can only be the LSB.  The upper bytes are assumed
	// to be the same as the pipe 1 address
	if(pipeNum >= 2){
		nRF24_WriteReg(nrf_object, RX_ADDR_BASE + pipeNum, (uint8_t)(address));
	} else {
		nRF24_WriteMultReg(nrf_object, RX_ADDR_BASE + pipeNum, (uint8_t *)&address, nrf_object->settings.aw + 2);
	}

	nRF24_WriteReg(nrf_object, EN_RXADDR, nRF24_ReadReg(nrf_object, EN_RXADDR) | (1 << pipeNum));
}

void nRF24_OpenTxPipe(nrf24_t * nrf_object, uint64_t address) {
	nRF24_Standby(nrf_object);
	nRF24_WriteMultReg(nrf_object, RX_ADDR_P0, (uint8_t *)&address, nrf_object->settings.aw + 2);
	nRF24_WriteMultReg(nrf_object, TX_ADDR, (uint8_t *)&address, nrf_object->settings.aw + 2);

	/** TODO: We may need to add this back in.  I am unsure how using dynamic payload effects the use
	 * of the payload length registers
	 *
	 * \code
	 *	nRF24_WriteReg(nrf_object, RX_PW_P0, min(payload_size,max_payload_size));
	 *	\endcode
	 */
}

void nRF24_EventHandler(nrf24_t * nrf_object){
	uint8_t payload_length;
	spi_transaction_t transaction;
	// Read the status register to figure out the cause of the interrupt
	uint8_t status = nRF24_ReadReg(nrf_object, STATUS);
	volatile uint8_t temp_status;
	temp_status = status;
	// Clear the interrupts
	nRF24_WriteReg(nrf_object, STATUS, status | RX_DR | MAX_RT | TX_DS);

	if(nrf_object->settings.prim_rx) { // Receiver Mode
		// Routine for handling successful data transmission
		if (status & TX_DS){
			if(nrf_object->AckPayloadSent){
				nrf_object->AckPayloadSent();
			}
		}
		// Routine for handling data received
		while ((temp_status & RX_DR) || ((temp_status & 0x0E) <= 0x0A)){
			// save the global last RX pipe
			nrf_object->last_rx_pipe = (temp_status & 0x0E) >> 1;
			if(nrf_object->last_rx_pipe >= 7) {
				temp_status = 0x0E;
			}else {
				// Find out how long the payload is
				payload_length = nRF24_GetPayloadLength(nrf_object);

				transaction.flags.blocking = 1;
				transaction.cs_control = nrf_object->csn;
				transaction.writeLength = 1;
				transaction.readLength = payload_length;
				transaction.readDelay = 1;
				transaction.data[0] = R_RX_PAYLOAD;
				transaction.flags.channel = nrf_object->spi_channel;

				SPI_Transact(&transaction);

				if(nrf_object->ReceivedPayload){
					nrf_object->ReceivedPayload(&transaction.data[1], payload_length);
				}
				temp_status = nRF24_ReadReg(nrf_object, STATUS);
			}
		}
	} else { // Transmit Mode
		// Routine for handling data received
		while ((temp_status & RX_DR) || ((temp_status & 0x0E) <= 0x0A)){
			// Find out how long the payload is
			payload_length = nRF24_GetPayloadLength(nrf_object);

			transaction.flags.blocking = 1;
			transaction.cs_control = nrf_object->csn;
			transaction.writeLength = 1;
			transaction.readLength = payload_length;
			transaction.readDelay = 1;
			transaction.data[0] = R_RX_PAYLOAD;
			transaction.flags.channel = nrf_object->spi_channel;

			SPI_Transact(&transaction);
			if(nrf_object->AckPayloadReceived) {
				nrf_object->AckPayloadReceived(&transaction.data[1], payload_length);
			}
			temp_status = nRF24_ReadReg(nrf_object, STATUS);
		}
		if ((status & TX_DS) && !(status & RX_DR)){
			// Routine for handling successful data transmission
			if(nrf_object->AckReceived){
				nrf_object->AckReceived();
			}
		}

		// Routine for handling max retries
		if (status & MAX_RT){
			if(nrf_object->MaxRetriesHit){
				nrf_object->MaxRetriesHit();
			}
		}
	}
}

void nRF24_SetActive(nrf24_t * nrf_object, uint8_t active){
	uint8_t config = nRF24_ReadReg(nrf_object, CONFIG);
	config &= ~PWR_UP;
	if (active) config |= PWR_UP;
	nRF24_WriteReg(nrf_object, CONFIG, config);

	// Need to wait for activation. Datasheet says this should be controlled by MCU so let's be good citizens
	// Actually only 150uS is required with external oscillator (likely) but let's be on the safe side and
	// use 1.5mS (2ms) which is the delay when using the internal oscillator
    DelayUs(1500);
	//DelayMs(2);
}

void nRF24_FlushRx(nrf24_t * nrf_object){
	spi_transaction_t transaction;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = 1;
	transaction.readLength = 0;
	transaction.readDelay = 0;
	transaction.data[0] = W_REGISTER | FLUSH_RX;
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
}

void nRF24_FlushTx(nrf24_t * nrf_object){
	spi_transaction_t transaction;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = 1;
	transaction.readLength = 0;
	transaction.readDelay = 0;
	transaction.data[0] = W_REGISTER | FLUSH_TX;
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
}

void nRF24_WriteReg(nrf24_t * nrf_object, uint8_t reg_address, uint8_t value){
	spi_transaction_t transaction;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = 2;
	transaction.readLength = 0;
	transaction.data[0] = W_REGISTER | reg_address;
	transaction.data[1] = value;
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
}

void nRF24_WriteMultReg(nrf24_t * nrf_object, uint8_t reg_address, uint8_t *data_ptr, uint8_t length){
	spi_transaction_t transaction;
	uint8_t i;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = length + 1;
	transaction.readLength = 0;
	transaction.data[0] = W_REGISTER | reg_address;
	for(i = 1; i <= length; i++){
		transaction.data[i] = *data_ptr++;
	}
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
}

uint8_t nRF24_ReadReg(nrf24_t * nrf_object, uint8_t reg_address){
	spi_transaction_t transaction;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = 1;
	transaction.readLength = 1;
	transaction.readDelay = 1;
	transaction.data[0] = R_REGISTER | reg_address;
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
	return transaction.data[1];
}

uint8_t nRF24_GetPayloadLength(nrf24_t * nrf_object){
	spi_transaction_t transaction;

	transaction.flags.blocking = 1;
	transaction.cs_control = nrf_object->csn;
	transaction.writeLength = 1;
	transaction.readLength = 1;
	transaction.readDelay = 1;
	transaction.data[0] = R_RX_PL_WID;
	transaction.flags.channel = nrf_object->spi_channel;

	SPI_Transact(&transaction);
	return transaction.data[1];
}
