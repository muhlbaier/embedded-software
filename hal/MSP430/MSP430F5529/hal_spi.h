/** 
 * @defgroup hal_spi_msp430f5529 SPI Hardware Abstraction Layer for MSP430F5529
 * @ingroup hal_spi
 *
 * @author Anthony Merlino
 *
 * Created on February 26, 2015, 1:31 PM
 *
 * @todo Anthony Merlino document hal_spi_msp430f5529
 *
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>
#include "project_settings.h"

/** @brief spi settings structure
 *
 * @warning structure had to be above project_settings.h include in order for compiler to find it ???
 */
typedef struct hal_spi_settings_t {
	uint8_t char7bit : 1; ///< char7bit
	uint8_t msb_first : 1; ///< msb_first
}hal_spi_settings_t;

// This must be included after hal_spi_settings_t so that spi.h can find that definition
#include "spi.h"

#ifdef USE_SPI_A0
#define USE_SPI0
#endif

#ifdef USE_SPI_A1
#define USE_SPI1
#endif

#ifdef USE_SPI_B0
#define USE_SPI2
#endif

#ifdef USE_SPI_B1
#define USE_SPI3
#endif

#define SPI_A0 0 ///< index for SPI module A0
#define SPI_A1 1 ///< index for SPI module A1
#define SPI_B0 2 ///< index for SPI module B0
#define SPI_B1 3 ///< index for SPI module B1

///@}
#endif //_HAL_SPI_H_
