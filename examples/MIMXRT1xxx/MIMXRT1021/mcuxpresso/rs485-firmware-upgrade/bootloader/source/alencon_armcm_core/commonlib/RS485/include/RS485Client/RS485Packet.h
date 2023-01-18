/**
*	@file 	RS485Packet.h
*	@brief
*	@date
*	@author	James Ethridge
*
*
* Copyright (c) 2015, Alencon Systems
* All rights reserved.
*/


/** PackageName
*	@addtogroup PackageName
*	@{
*/

/** RS485Packet
*	@addtogroup RS485Packet
*	@{
*/
/* ***** Inclusion Gaurd ****** */
#ifndef __RS485Packet_H__
#define __RS485Packet_H__

/* ***** Dependencies ****** */
#include <stdint.h>

/** RS485Packet_Public_Macros
*	@{
*/
#define RS485_DATA_NBYTES 128 //TODO - why can't I make this smaller?
#define RS485_PACKET_PREAMBLE 0xC0
#define RS485_PACKET_FOOTER   0x0A55

#define RS485_PACKET {0}
/** RS485Packet_Public_Macros
*	@}
*/

/** RS485Packet_Public_Types
*	@{
*/

typedef struct rs485_payload_t{
    uint8_t  length;
    uint8_t  control;
    uint8_t  destination;
    uint8_t  databuf[RS485_DATA_NBYTES];
}rs485_payload_t;

typedef struct rs485_packet_t{
    uint8_t preamble;
    rs485_payload_t payload;
    uint16_t crc;
    uint16_t footer;
}rs485_packet_t;



/** RS485Packet_Public_Types
*	@}
*/

/** RS485Packet_Public_Functions
*	@{
*/
void RS485Packet_init(rs485_packet_t *packet, uint8_t destination, uint8_t control, const void *data, uint8_t size);
uint16_t RS485Packet_calc_crc(rs485_packet_t *packet);
/** RS485Packet_Public_Functions
*	@}
*/

#endif

/** RS485Packet
*	@}
*/

/** PackageName
*	@}
*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */
