/*   Copyright 2015 Michael Muhlbaier and Anthony Merlino

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   Sections of this module were heavily influenced by the Arduino library for Nordic Semiconductor NRF24L01+
   found at https://github.com/akupila/NRF24
*/

/** @defgroup nRF24 nRF24L01+ Module
 * 
 * This module provides an interface with the nRF24L01+ built on the SPI module.
 * 
 * 
 * @{
 */

/** TODO: Make this library work with big endian devices
 *
 */

#ifndef _NRF24_H_
#define _NRF24_H_

#include <stdint.h>

typedef struct nrf24_radio_t {
    void (*ce)(uint8_t); ///< Set Chip Enable Function Pointer
    void (*csn)(uint8_t); ///< Set SPI Ship Select Function Pointer
    void (*AckReceived)(void); // should be called when in PTX mode and TX_DS flag is set
    void (*AckPayloadReceived)(uint8_t * data, uint8_t length);
    void (*MaxRetriesHit)(void);
    void (*ReceivedPayload)(uint8_t * data, uint8_t length);
    void (*AckPayloadSent)(void);
    uint8_t state; ///< state of radio
    uint8_t spi_channel;
    struct settings {
    	uint8_t prim_rx : 1;
    	uint8_t aw : 2;
    	uint8_t unused : 5;
    } settings;
    uint64_t rx_address_p0;
    uint64_t last_tx_address;
    uint8_t last_rx_pipe;
} nrf24_t;


typedef enum {
	NRF24_PA_LOW = 0,		// -18dBm	/	7.0mA transmit
	NRF24_PA_MID,			// -12dBm	/	7.5mA transmit
	NRF24_PA_HIGH,			// -6dBm	/	9mA transmit
	NRF24_PA_MAX			//  0dBm	/	11.3mA transmit
} nrf24_pa_level_e;

typedef enum {
	NRF24_250KBPS = 0,
	NRF24_1MBPS,
	NRF24_2MBPS
} nrf24_datarate_e;

typedef enum {
	NRF24_NO_CRC = 0,
	NRF24_CRC_8BIT,
	NRF24_CRC_16BIT
} nrf24_crc_mode_e;

/** Initialize nRF24L01+ module with default settings
 * 
 * SPI channel should already be initialized before calling nRF24_Init().
 * 
 * Default Settings:
 * - Max Retries 15
 * - Power Amplification Max
 * - Data Rate 2Mbps
 * - 16-bit CRC
 * - Channel 76
 * - Dynamic Payload Length Feature
 * - Ack Payload Feature
 * - Dynamic Ack Payload Feature (allow empty acks)
 * - Auto Ack Enabled on All Pipes
 * - Dynamic Payload Enabled for All Pipes
 * - 5-byte Address Width
 * 
 * Example Usage:
@code
 nrf24_t RF1; // make sure this isn't on the stack
 //... 
 RF1.ce = RF1_CE; // where RF1_CE is a function to set ChipEnable
 RF1.csn = RF1_CSN; // where RF1_CSN is a function to set ~ChipSelect
 RF1.spi_channel = 2;
 RF1.ReceivedPayload = RF1_RxPayloadHandler;
 RF1.AckPayloadReceived = RF1_AckPayloadHandler;
 RF1.AckReceived = RF1_AckReceivedHandler;
 RF1.MaxRetriesHit = RF1_MaxRetriesHandler;

 nRF24_Init(&RF1);
@endcode
 * @param nrf_object Pointer to nfr24_t object.
 */
void nRF24_Init(nrf24_t * nrf_object);

/** Open a RX pipe for a specific address
 * 
 * Note: for pipes 2-5 only the LSB of the address is used
 * 
 * @param nrf_object Pointer to nfr24_t object.
 * @param pipenum Pipe index to use (0-5)
 * @param address address to receive from
 */
void nRF24_OpenRxPipe(nrf24_t * nrf_object, uint8_t pipenum, uint64_t address);

/** Open a TX pipe for a specific address
 * 
 * @param nrf_object Pointer to nfr24_t object.
 * @param address address to transmit on
 */
void nRF24_OpenTxPipe(nrf24_t * nrf_object, uint64_t address);

/** Use nRF24_ISR() in the interrupt service routine that detects high to low
 * changes for the IRQ pin.
 */
void nRF24_EventHandler(nrf24_t * nrf_object);
#define nRF24_ISR(nrf_object) Task_Queue( (task_t)nRF24_EventHandler, nrf_object)

/** Start listening on primary RX node */
void nRF24_StartListening(nrf24_t * nrf_object);

/** Disable the nRF24 chip and standby */
void nRF24_Standby(nrf24_t * nrf_object);

void nRF24_WriteReg(nrf24_t * nrf_object, uint8_t reg_address, uint8_t value);
void nRF24_WriteMultReg(nrf24_t * nrf_object, uint8_t reg_address, uint8_t *data_ptr, uint8_t length);
uint8_t nRF24_ReadReg(nrf24_t * nrf_object, uint8_t reg_address);
/** Get the length of the payload received */
uint8_t nRF24_GetPayloadLength(nrf24_t * nrf_object);

/** Send a message from a primary TX node */
void nRF24_Write(nrf24_t * nrf_object, uint8_t * buf, uint8_t length);
/** Read the received message */
void nRF24_Read(nrf24_t * nrf_object, uint8_t * buf, uint8_t length);
/** Send a message in an ack response payload from a primary RX node */
void nRF24_WriteAck(nrf24_t * nrf_object, uint8_t * buf, uint8_t length, uint8_t pipe);

void nRF24_SetPowerAmplificationLevel(nrf24_t * nrf_object, nrf24_pa_level_e level);
void nRF24_SetDataRate(nrf24_t * nrf_object, nrf24_datarate_e rate);
void nRF24_SetCRCMode(nrf24_t * nrf_object, nrf24_crc_mode_e mode);
void nRF24_SetChannel(nrf24_t * nrf_object, uint8_t channel);
void nRF24_SetActive(nrf24_t * nrf_object, uint8_t active);

/**
 * @param delay delay before retransmit attempt, actual delay = 250us + delay*250us where delay is from 0-15
 * @param count retransmit count before triggering max retries interrupt, 0-15 where 0 is no retries and 15 is 15 retries
 */
void nRF24_SetRetries(nrf24_t * nrf_object, uint8_t delay, uint8_t count);
void nRF24_FlushRx(nrf24_t * nrf_object);
void nRF24_FlushTx(nrf24_t * nrf_object);

/** @} */

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Helpers - Memory Macros */
#define RX_ADDR_BASE RX_ADDR_P0

#define AW_5B 0x03 // 5 byte address
#define AW_4B 0x02 // 4 byte address
#define AW_3B 0x01 // 3 byte address

// Bit Mnemonics ----------------------
// CONFIG
#define MASK_RX_DR  0x40
#define MASK_TX_DS  0x20
#define MASK_MAX_RT 0x10
#define EN_CRC      0x08
#define CRCO        0x04
#define PWR_UP      0x02
#define PRIM_RX     0x01
// EN_AA
#define ENAA_P5     0x20
#define ENAA_P4     0x10
#define ENAA_P3     0x08
#define ENAA_P2     0x04
#define ENAA_P1     0x02
#define ENAA_P0     0x01
// EN_RXADDR
#define ERX_P5      0x20
#define ERX_P4      0x10
#define ERX_P3      0x08
#define ERX_P2      0x04
#define ERX_P1      0x02
#define ERX_P0      0x01
// SETUP_AW
#define AW          0x01
// SETUP_RETR
#define ARD         0x10
#define ARC         0x01
// RF_SETUP
#define CONT_WAVE   0x80
#define RF_DR_LOW   0x20
#define PLL_LOCK    0x10
#define RF_DR_HIGH  0x08
#define RF_PA_HIGH  0x04
#define RF_PA_LOW   0x02
// STATUS
#define RX_DR       0x40
#define TX_DS       0x20
#define MAX_RT      0x10
#define RX_P_NO     0x02
#define TX_FULL     0x01
// OBSERVE_TX
#define PLOS_CNT    0x10
#define ARC_CNT     0x01
// FIFO_STATUS
#define TX_REUSE    0x40
#define TX_FULL_FIFO 0x20 // annoyingly this has the same mnemonic as in STATUS
#define TX_EMPTY    0x10
#define RX_FULL     0x02
#define RX_EMPTY    0x01
// DYNPD
#define DPL_P5      0x20
#define DPL_P4      0x10
#define DPL_P3      0x08
#define DPL_P2      0x04
#define DPL_P1      0x02
#define DPL_P0      0x01
// FEATURE
#define EN_DPL      0x04
#define EN_ACK_PAY  0x02
#define EN_DYN_ACK  0x01

// SPI Commands ----------------------
#define REGISTER_MASK       0x1F
#define R_REGISTER          0x00
#define W_REGISTER          0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define REUSE_TX_PL         0xE3
#define ACTIVATE            0x50
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD       0xA8
#define W_TX_PAYLOAD_NO_ACK 0xB0
#define NOP                 0xFF

#endif //_NRF24_H_
