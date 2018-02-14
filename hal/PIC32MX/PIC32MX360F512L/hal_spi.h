/** 
 * @defgroup hal_spi_pic32mx695f512h SPI Hardware Abstraction Layer for PIC32MX695F512H
 * @ingroup hal_spi
 *
 * @author Michael Muhlbaier
 *
 * SPI channels 2, 3, 4, available and supported.
 *
 * @warning Only 8-bit mode supported
 * 
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>

#define SPI_1 0 ///< index for SPI module 2
#define SPI_2 1 ///< index for SPI module 2

/// spi settings structure
typedef struct {
    uint8_t mode16 : 1; ///< 1 = 16bit mode (or 32bit mode), 0 = 8bit mode or 32bit mode (see mode32)
    uint8_t mode32 : 1; ///< 1 = 32bit mode, 0 = 8bit mode or 16bit mode (see mode 16)
    uint8_t sample_phase : 1; ///< sample phase: 1 = sample at end of data output, 0 = sample in middle
} hal_spi_settings_t;

///@}
#endif //_HAL_SPI_H_
