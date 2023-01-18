#include <stdint.h>
#include <stdbool.h>

#include "library.h"
#include "project_settings.h"
#include "mcuid.h"
#include "debug.h"
#include "hal_eeprom.h"
#include "flash.h"

//#include "pin_mux.h"
//#include "clock_config.h"
//#include "board.h"
#include "fsl_common.h"
#include "fsl_flexspi.h"
//#include "app.h"


//#include "fsl_iomuxc.h"
//#include "pin_mux.h"

#include "fsl_cache.h"
#include "flexspi_nor_flash_ops.h"

// IS25LP064A-JBLE
// page size:		256B
// sector size:		4kB, 32kB, 64kB


uint32_t FLASH_Init(void)
{
	hal_EEPROM_Init();
	
	return 0;
}

uint32_t FLASH_EraseSector(uint32_t sector_address, uint32_t sector_size)
{
	//return 0;
	//debugMSG(DNOTIFY, "ERASE ADDR: %x\n",sector_address);
	// assuming 4096 sector size
	uint32_t result = 1;
    
    uint32_t address = sector_address - (FLASH_MEMORY_ADDRESS);
    result = flexspi_nor_flash_erase_sector(EXAMPLE_FLEXSPI, address);
    DCACHE_InvalidateByRange(sector_address, FLASH_SECTOR_SIZE);
    
    
    if(result == (kStatus_Success))
    {
		result = 0;
	}
    
    return result;
}

uint32_t FLASH_Program(uint32_t page_address, uint32_t length, uint8_t * data)
{
	//return 0;
	//debugMSG(DNOTIFY, "WRITE ADDR: %x L: %d\n",page_address, length);
	//uint32_t k;
	//for(k=0;k<length;k++)
	//{
		//if((k%16) == 0)
		//{
			//debugMSG(DAPPEND, "\n%x  ",page_address+k);
		//}
		//else
		//{
			//debugMSG(DAPPEND, "%xb ",data[k]);
		//}
	//}
	//debugMSG(DAPPEND, "\n");
	
    uint32_t result = 1;
    uint32_t address = page_address - (FLASH_MEMORY_ADDRESS);
    //length = length/4;
    
    __disable_irq();
    result = flexspi_nor_flash_program(EXAMPLE_FLEXSPI, address, (uint32_t *)data, length);
    __enable_irq();
    DCACHE_InvalidateByRange(page_address, FLASH_SECTOR_SIZE);
    
    
    if(result == (kStatus_Success))
    {
		result = 0;
	}
	
    return result;
}
