/**
 * @file pycrc_stdout
 * Functions and types for CRC checks.
 *
 * Generated on Sat Nov 12 07:25:42 2016,
 * by pycrc v0.9, https://pycrc.org
 * using the configuration:
 *    Width         = 16
 *    Poly          = 0x1021
 *    Xor_In        = 0x0000
 *    ReflectIn     = False
 *    Xor_Out       = 0x0000
 *    ReflectOut    = False
 *    Algorithm     = bit-by-bit-fast
 *
 * Modified by Michael Muhlbaier
 * 
 * @defgroup crc16 CRC16 Module
 *
 * In system.h use "#define CRC16_FAST" for a fast implementation at the expense
 * more program memory.
 *
 * Use "#define CRC16_SMALL" for a small implementation that takes longer to run.
 *
 * Otherwise a balanced approach will be used by default.
 *
 * Implements the XModem CRC16 standard.
 * @{
 *****************************************************************************/
#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>
#include <stdlib.h>

/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 16 bits.
 *****************************************************************************/
typedef uint_fast16_t crc_t;

/**
 * Calculate the initial crc value.
 *
 * @return     The initial crc value.
 *****************************************************************************/
static inline crc_t CRC16_Init(void)
{
    return 0x0000;
}

/**
 * Update the crc value with new data.
 *
 * @param crc      The current crc value.
 * @param data     Pointer to a buffer of \a data_len bytes.
 * @param data_len Number of bytes in the \a data buffer.
 * @return         The updated crc value.
 *****************************************************************************/
crc_t CRC16_Update(crc_t crc, const void *data, size_t data_len);

/**
 * Calculate the final crc value.
 *
 * @param crc  The current crc value.
 * @return     The final crc value.
 *****************************************************************************/
static inline crc_t CRC16_Finalize(crc_t crc)
{
    return crc ^ 0x0000;
}

 /** @} */
 
 #endif      /* _CRC16_H_ */
