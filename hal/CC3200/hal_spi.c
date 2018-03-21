#include <stdint.h>
#include "project_settings.h"
#include "hal_spi.h"

#include "spi.h"

#include "driverlib/pin.h"
#include "driverlib/spi.h"
#include "driverlib/prcm.h"
#include "hw_memmap.h"

void hal_SPI_ISR(void);

void hal_SPI_Init(spi_settings_t* settings){

	// Configure PIN_05 for SPI0 GSPI_CLK
	PinTypeSPI(PIN_05, PIN_MODE_7);

    // Configure PIN_07 for SPI0 GSPI_MOSI
    PinTypeSPI(PIN_07, PIN_MODE_7);

#ifdef USE_SPI_MISO_ALT
    	// Configure PIN_53 for SPI0 GSPI_CS
		PinTypeSPI(PIN_53, PIN_MODE_9);
#else
		// Configure PIN_06 for SPI0 GSPI_MISO
		PinTypeSPI(PIN_06, PIN_MODE_7);
#endif

    if(!settings->hal_settings.three_pin){
#ifdef USE_SPI_CS_ALT
    	// Configure PIN_50 for SPI0 GSPI_CS
		PinTypeSPI(PIN_50, PIN_MODE_9);
#else
        // Configure PIN_08 for SPI0 GSPI_CS
        PinTypeSPI(PIN_08, PIN_MODE_7);
#endif
    }

	PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

	// Disable
	SPIDisable(GSPI_BASE);

	SPIReset(GSPI_BASE);

	SPIConfigSetExpClk(	GSPI_BASE,
						PRCMPeripheralClockGet(PRCM_GSPI),
						settings->bit_rate,
						SPI_MODE_MASTER,
						settings->mode,
						(
							(settings->hal_settings.sw_cs_control << 24) |
							(settings->hal_settings.three_pin << 25) |
							(settings->hal_settings.cs_active_low << 6) |
							((settings->hal_settings.word_length << 10) |SPI_WL_8) |
							(settings->hal_settings.turbo_on << 19)
						));

	// Clear the RX interrupt
	SPIIntClear(GSPI_BASE, SPI_INT_RX_OVRFLOW|SPI_INT_RX_FULL);

    // Register Interrupt Handler
    SPIIntRegister(GSPI_BASE, hal_SPI_ISR);

    // Enable Interrupts
    SPIIntEnable(GSPI_BASE, SPI_INT_RX_FULL);

	SPIEnable(GSPI_BASE);
}

void hal_SPI_ISR(void){
	SPI_ISR(0);
}

