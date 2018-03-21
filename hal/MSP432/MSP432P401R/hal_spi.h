/** 
 * @defgroup hal_spi_msp432p401r SPI Hardware Abstraction Layer for MSP432P401R
 * @ingroup spi
 * @file hal_spi.h
 *
 * @author Autumn Brown
 *
 * Created on February 25, 2017
 *
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>
#include "project_settings.h"

/**
 * @brief Structure for additional hardware specific SPI settings
 *
 */
typedef struct hal_spi_settings_t {
	uint8_t char7bit : 1; ///< enables 7bit word mode (as oppose to 8bit)
	uint8_t msb_first : 1; ///< enables MSB-first serial transmission
} hal_spi_settings_t;

// This must be included after hal_spi_settings_t so that spi.h can find that definition
#include "spi.h"

#ifdef USE_SPI_A0
#define USE_SPI0
#endif

#ifdef USE_SPI_A1
#define USE_SPI1
#endif

#ifdef USE_SPI_A2
#define USE_SPI2
#endif

#ifdef USE_SPI_A3
#define USE_SPI3
#endif

#ifdef USE_SPI_B0
#define USE_SPI4
#endif

#ifdef USE_SPI_B1
#define USE_SPI5
#endif

#ifdef USE_SPI_B2
#define USE_SPI6
#endif

#ifdef USE_SPI_B3
#define USE_SPI7
#endif

/**
 * Channel for SPI module A0
 *
 * CLK:  P1.1 (HW Pin 4)
 * MOSI: P1.3 (HW Pin 6)
 * MISO: P1.2 (HW Pin 5)
 */
#define SPI_A0 0

/**
 * Channel for SPI module A1
 *
 * CLK:  P2.1 (HW Pin 17)
 * MOSI: P2.3 (HW Pin 18)
 * MISO: P2.2 (HW Pin 19)
 */
#define SPI_A1 1

/**
 * Channel for SPI module A2
 *
 * CLK:  P3.1 (HW Pin 33)
 * MOSI: P3.3 (HW Pin 35)
 * MISO: P3.2 (HW Pin 34)
 */
#define SPI_A2 2

/**
 * Channel for SPI module A3
 *
 * CLK:  P9.5 (HW Pin 97)
 * MOSI: P9.7 (HW Pin 99)
 * MISO: P9.6 (HW Pin 98)
 */
#define SPI_A3 3

/**
 * Channel for SPI module B0
 *
 * CLK:  P1.5 (HW Pin  9)
 * MOSI: P1.6 (HW Pin 10)
 * MISO: P1.7 (HW Pin 11)
 */
#define SPI_B0 4

/**
 * CLK:  P6.3 (HW Pin 77)
 * MOSI: P6.4 (HW Pin 78)
 * MISO: P6.5 (HW Pin 79)
 */
#define SPI_B1 5

/**
 * Channel for SPI module B2
 *
 * CLK:  P3.5 (HW Pin 37)
 * MOSI: P3.6 (HW Pin 38)
 * MISO: P3.7 (HW Pin 39)
 */
#define SPI_B2 6

/**
 * Channel for SPI module B3
 *
 * CLK:  P10.1 (HW Pin 1)
 * MOSI: P10.2 (HW Pin 2)
 * MISO: P10.3 (HW Pin 3)
 */
#define SPI_B3 7

///@}
#endif //_HAL_SPI_H_
