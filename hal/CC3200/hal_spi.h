/** 
 * @defgroup hal_spi_cc3200 SPI Hardware Abstraction Layer for CC3200
 * @ingroup hal_spi
 *
 * @author Anthony Merlino
 *
 * Created on September 9, 2015, 1:31 PM
 *
 * @todo Anthony Merlino document hal_spi_cc3200
 *
 * @{
 */

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>

#include "hw_types.h" // Needed or driverlib/spi.h doesn't know about certain types
#include "hw_memmap.h"
#include "driverlib/spi.h"
#include "inc/hw_mcspi.h"

/** @brief spi settings structure
 *
 * @warning structure had to be above project_settings.h include in order for compiler to find it ???
 */
typedef struct hal_spi_settings_t {
	uint8_t sw_cs_control : 1; /// 0= HW Control, 1= SW Control
	uint8_t three_pin : 1; /// 0= 4-pin mode, 1 = 3-pin mode
	uint8_t cs_active_low : 1; /// 0= Active High, 1= Active Low
	uint8_t word_length : 2; /// 0= 8-bit, 1= 16-bit, 3= 32-bit
	uint8_t turbo_on : 1; /// 0= Turbo Off, 1= Turbo On
}hal_spi_settings_t;

#include "project_settings.h"

#ifdef USE_SPI
#define USE_SPI0
#endif

#define SPI_CH 0 ///< index for General Purpose SPI module

#define hal_SPI_Enable(channel) 			SPIEnable(GSPI_BASE)
#define hal_SPI_Disable(channel)			SPIDisable(GSPI_BASE)
#define hal_SPI_SpaceAvailable(channel)		(HWREG(GSPI_BASE + MCSPI_O_CH0STAT) & MCSPI_CH0STAT_TXS)
#define hal_SPI_DataAvailable(channel)		(HWREG(GSPI_BASE + MCSPI_O_CH0STAT) & MCSPI_CH0STAT_RXS)
#define hal_SPI_ClearRxIF(channel)			SPIIntClear(GSPI_BASE, SPI_INT_RX_OVRFLOW|SPI_INT_RX_FULL)
#define hal_SPI_ClearTxIF(channel)			SPIIntClear(GSPI_BASE, SPI_INT_TX_UDRFLOW |SPI_INT_TX_EMPTY)
#define hal_SPI_EnableRxInterrupt(channel)	SPIIntEnable(GSPI_BASE, SPI_INT_RX_FULL)
#define hal_SPI_EnableTxInterrupt(channel)	SPIIntEnable(GSPI_BASE, SPI_INT_TX_EMPTY)
#define hal_SPI_DisableRxInterrupt(channel) SPIIntDisable(GSPI_BASE, SPI_INT_RX_FULL)
#define hal_SPI_DisableTxInterrupt(channel) SPIIntDisable(GSPI_BASE, SPI_INT_TX_EMPTY)
#define hal_SPI_RxIntStatus(channel)		SPIIntStatus(GSPI_BASE, SPI_INT_RX_FULL)
#define hal_SPI_TxIntStatus(channel)		SPIIntStatus(GSPI_BASE, SPI_INT_TX_EMPTY)
#define hal_SPI_TxByte(channel, data)		(HWREG(GSPI_BASE + MCSPI_O_TX0) = data)
#define hal_SPI_RxByte(channel)				(uint8_t)(HWREG(GSPI_BASE + MCSPI_O_RX0) & 0xFF)

///@}
#endif //_HAL_SPI_H_


