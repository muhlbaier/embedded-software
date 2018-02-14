/** 
 * @defgroup hal_spi_pic32mx250f128b SPI Hardware Abstraction Layer for PIC32MX250F128B
 * @ingroup hal_spi
 *
 * @author Michael Muhlbaier
 *
 * SPI channels 1 and 2 available and supported.
 *
 * @warning Only 8-bit mode supported
 * 
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>

#define SPI_1 1 ///< index for SPI module 1
#define SPI_2 2 ///< index for SPI module 2

/// spi settings structure
typedef struct {
    uint8_t mode16 : 1; ///< 1 = 16bit mode (or 32bit mode), 0 = 8bit mode or 32bit mode (see mode32)
    uint8_t mode32 : 1; ///< 1 = 32bit mode, 0 = 8bit mode or 16bit mode (see mode 16)
    uint8_t sample_phase : 1; ///< sample phase: 1 = sample at end of data output, 0 = sample in middle
} hal_spi_settings_t;

///@}
#endif //_HAL_SPI_H_
