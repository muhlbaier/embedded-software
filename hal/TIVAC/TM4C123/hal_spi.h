/*
 *  hal_spi.h - SPI HAL For the Tiva-C TM4C123
 *
 *  Created on: Mar 29, 2017
 *      Author: Christopher Green <greenc3@students.rowan.edu>
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "project_settings.h"
#include "driverlib/ssi.h"
#include "inc/hw_ssi.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

// HAL-specific parameters for the Tiva-C
typedef struct hal_spi_settings_t {
	uint8_t data_width : 5; // Tiva-C supports data widths between 4 and 16
	uint8_t msb_first : 1; // msb_first - You MUST do this ordering swap in software, Tiva-C doesn't have hardware functionality
}hal_spi_settings_t;

// This must be included after hal_spi_settings_t so that spi.h can find that definition
#include "spi.h"

// Tiva-C T4MC123 has 4 SSI Modules - SSI0-SSI3
#ifdef USE_SSI0
#define USE_SPI0
#endif

#ifdef USE_SSI1
#define USE_SPI1
#endif

#ifdef USE_SSI2
#define USE_SPI2
#endif

#ifdef USE_SSI3
#define USE_SPI3
#endif

#define SSI0 0 // index for SPI module 0
#define SSI1 1 // index for SPI module 1
#define SSI2 2 // index for SPI module 2
#define SSI3 3 // index for SPI module 3

#endif
