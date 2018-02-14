/** 
 * @defgroup hal_spi_pic24fj256ga110 SPI Hardware Abstraction Layer for PIC24FJ256GA110
 * @ingroup hal_spi
 *
 * @author Michael Muhlbaier
 *
 * SPI channels 1, 2, and 3 available and supported.
 *
 * @warning Only 8-bit mode supported
 * 
 * @warning the PIC24FJ256GA110 does not support all bit_rate settings. It has
 * two prescale dividers:
 * Primary -> 1:1, 4:1, 16:1, 64:1
 * Secondary -> 1:1, 2:1, 3:1, 4:1, 5:1, 6:1, 7:1, 8:1
 * Thus the minimum bit_rate is PERIPHERAL_CLOCK / 512 and the max is
 * PERIPHERAL_CLOCK and other values will be rounded down to the nearest
 * divider settings.
 * 
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>

#define SPI_1 1 ///< index for SPI module 1
#define SPI_2 2 ///< index for SPI module 2
#define SPI_3 3 ///< index for SPI module 3

/// spi settings structure
typedef struct {
    uint8_t mode16 : 1; ///< 1 = 16bit mode (or 32bit mode), 0 = 8bit mode or 32bit mode (see mode32)
    uint8_t mode32 : 1; ///< 1 = 32bit mode, 0 = 8bit mode or 16bit mode (see mode 16)
    uint8_t sample_phase : 1; ///< sample phase: 1 = sample at end of data output, 0 = sample in middle
} hal_spi_settings_t;

///@}
#endif //_HAL_SPI_H_
