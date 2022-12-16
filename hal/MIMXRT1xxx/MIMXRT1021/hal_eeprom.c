#include <stdint.h>
#include <stdbool.h>

//#include "eeprom.h"
//#include "SSD_FTFx.h"
#include "library.h"
#include "project_settings.h"
#include "mcuid.h"
#include "debug.h"
#include "hal_eeprom.h"


#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_common.h"
#include "fsl_flexspi.h"
#include "app.h"


#include "fsl_iomuxc.h"
#include "pin_mux.h"

#include "fsl_cache.h"
#include "flexspi_nor_flash_ops.h"

// IS25LP064A-JBLE
// page size:		256B
// sector size:		4kB, 32kB, 64kB


/// IMPORTANT
/// @warning it is impossible to write to the flash memory when the clock is in HSRUN mode
/// it is up to the user to switch the clock and switch it back if using HSRUN mode

// this implementation is dependent on the SSD functions provided by KSDK 1.3 which are in the following files:
// FlashInit.c
// CopyToRam.c
// FlashProgram.c
// FlashCommandSequence.c
// FlashEraseSector.c
//
// Note these files are copied into the embeddedlibrary for convenience but can be used directly from KSDK 1.3


#if EEPROM_SIZE > 512
#warning "Max eeprom size is 512 16-bit words"
#endif

#define EFLASH_MEMORY_ADDRESS		0x60000000

//// size of array to copy__Launch_Command function to
//// It should be at least equal to actual size of __Launch_Command func
//// User can change this value based on RAM size availability and actual size of __Launch_Command function
//#define LAUNCH_CMD_SIZE           0x100
//#define FTFx_REG_BASE             0x40020000
//#define P_FLASH_BASE              0x00000000
//// Program Flash block information
//#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
//// Data Flash block information
//#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
//// Flex Ram block information
//#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS

//// pointer to flash functions in RAM
//static pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

//// Array to copy __Launch_Command func to RAM.
//static uint16_t ramFunc[LAUNCH_CMD_SIZE/2];

// two pages/blocks of data, 512 32-bit words each
#ifndef EEPROM_DATA1
//#define EEPROM_DATA1 0x0003F000
#define EEPROM_DATA1 0x60010000
#endif
#ifndef EEPROM_DATA2
#define EEPROM_DATA2 0x60011000
#endif
const uint32_t * eeprom_data1 = (uint32_t*)EEPROM_DATA1;
const uint32_t * eeprom_data2 = (uint32_t*)EEPROM_DATA2;
///* requires something like the following added to the scatter file:

/* the following can be used if you don't care where eeprom goes and don't care if it is erased when programming or bootloading
__align(2048) const uint32_t eeprom_data1[512];
__align(2048) const uint32_t eeprom_data2[512];
*/


// internal functions
const uint32_t * GetActivePage(void);
int GetNextLocation(void);

static int _next_location = -1;
static const uint32_t * data_page_ptr;

void hal_EEPROM_Init(void) 
{///
	static uint8_t initialized = 0;
	if(initialized == 0) 
	{
		uint32_t status;
		__disable_irq();	
		flexspi_nor_flash_init(EXAMPLE_FLEXSPI);
		
		flexspi_nor_enable_quad_mode(EXAMPLE_FLEXSPI);
		__enable_irq();
		//debugMSG(0, "QUAD: %x\n",status);
		
    	
//#define EEPROM_UNIT_TEST    	
#ifdef EEPROM_UNIT_TEST
		uint32_t k;
		uint32_t result;
		uint8_t wbuff[HAL_EEPROM_PAGE_SIZE] __attribute__ ((aligned (4)));
		uint8_t rbuff[HAL_EEPROM_PAGE_SIZE];

		uint8_t vendorID = 0;
		__disable_irq();
		status = flexspi_nor_get_vendor_id(EXAMPLE_FLEXSPI, &vendorID);
		__enable_irq();
		debugMSG(0, "VID :%xb\n\n", vendorID);

	

		debugMSG(0, "ERASE sector at ADDR: %x\n",eeprom_data1);
		result = hal_EEPROM_ErasePage(EEPROM_DATA1);
		//DCACHE_InvalidateByRange(EEPROM_DATA1, HAL_EEPROM_PAGE_SIZE);
		debugMSG(0, "completed R:%x\n",result);
		
		uint8_t * ptr = (uint8_t *)eeprom_data1;
		for(k=0;k<(HAL_EEPROM_PAGE_SIZE);k++)
		{
			rbuff[k] = ptr[k];
		}
		for(k=0;k<(HAL_EEPROM_PAGE_SIZE);k++)
		{
			if(rbuff[k] != 0xFF)
			{
				debugMSG(DERROR, "erased error %d -> %xb\n",k,rbuff[k]);
				while(1);
			}
		}
		debugMSG(DNOTIFY, "erase VALID\n\n");
		
		
		
		debugMSG(0, "WRITE at sector ADDR: %x\n",eeprom_data1);
		for(k=0;k<(HAL_EEPROM_PAGE_SIZE);k++)
		{
			wbuff[k] = k;
		}
		result = hal_EEPROM_WritePage((uint32_t)eeprom_data1, wbuff);
		//DCACHE_InvalidateByRange(EEPROM_DATA1, HAL_EEPROM_PAGE_SIZE);
		debugMSG(0, "completed R:%x\n",result);
		
		ptr = (uint8_t *)eeprom_data1;
		for(k=0;k<(HAL_EEPROM_PAGE_SIZE);k++)
		{
			rbuff[k] = ptr[k];
		}
		
		for(k=0;k<(HAL_EEPROM_PAGE_SIZE);k++)
		{
			if(rbuff[k] != wbuff[k])
			{
				debugMSG(DERROR, "write error %d  %xb - %xb\n",k,rbuff[k],wbuff[k]);
				while(1);
			}
		}
		debugMSG(DNOTIFY, "write VALID\n\n");
		
		
		debugMSG(DNOTIFY, "test completed\n");
		while(1);
#endif    	
    	
    	
        // check if both pages are active, if so we need to erase them both:
        if(eeprom_data1[0] != 0xFFFFFFFF && eeprom_data2[0] != 0xFFFFFFFF) 
        {
			//#warning "Make the larger EEPROM of the two active EEPROMs the active one instead of wiping both out if a repack failed during a write."
			status = hal_EEPROM_ErasePage((uint32_t)eeprom_data1);
			if (status != kStatus_Success)
			{
				//PRINTF("\r\n***NOR Flash Erase Sector Failed!***\r\n");
				//ErrorTrap();
			}
            status = hal_EEPROM_ErasePage((uint32_t)eeprom_data2);
			if (status != kStatus_Success)
			{
				//PRINTF("\r\n***NOR Flash Erase Sector Failed!***\r\n");
				//ErrorTrap();
			}   
        }
        // get the active page
        data_page_ptr = GetActivePage();
        // if pointer is empty then neither page has data.
        if(data_page_ptr == 0) {
            /// @todo determine if we should erase the pages
            data_page_ptr = &eeprom_data1[0];
        }
        initialized = 1;
    }
}

uint16_t hal_EEPROM_Read(uint32_t address) 
{///
	int i;
	union32_t u32;
	// make sure the data_page_ptr is good
	if(data_page_ptr != &eeprom_data1[0] && data_page_ptr != &eeprom_data2[0]) return 0xFFFF;
	// find the most recent occurance of the address
	for(i = GetNextLocation()-1; i >= 0; i--) {
		u32.phrase = *(data_page_ptr+i);
		if(address == u32.word[1]) {
			return (u32.word[0] & 0xFFFF);
		}
	}
	
	
	// if address not found then return 0xFFFF
	return 0xFFFF;
}

void hal_EEPROM_Write(uint32_t address, uint16_t data) 
{///
	union32_t u32;

	// make sure the data is different than stored before writing
	if(hal_EEPROM_Read(address) == data) 
	{
		return;
	}
	
	// get end of page
	int i;
	i = GetNextLocation();
	
	// if no space is left in the current page then repack to the other page and erase this page
	if(i >= (FLASH_PAGE_SIZE)) 
	{
		const uint32_t * next_data_page;
		if(data_page_ptr == &eeprom_data1[0]) 
		{
			next_data_page = &eeprom_data2[0];
		}
		else 
		{
			next_data_page = &eeprom_data1[0];
		}
		// make sure the other page is erased
		hal_EEPROM_ErasePage((uint32_t)next_data_page);
		
		
		// move data from this page to the other page (make sure to update the data for this address if it exists aleady)
		// where to write the data
		uint32_t write_location;
		write_location = (uint32_t)next_data_page;
		int j;
		for(i = 0; i < FLASH_PAGE_SIZE; i++) 
		{
			// get the data
			if(i == address) 
			{
				u32.word[0] = data;
				u32.word[1] = address;
			}
			else 
			{		
				for(j = ((FLASH_PAGE_SIZE)-1); j >= 0; j--) 
				{
					u32.phrase = *(data_page_ptr+j);
					if(i == u32.word[1]) 
					{
						break;
					}
					else 
					{
						u32.word[0] = 0xFFFF;
					}
				}
			}
			// write it to the new page if it is non 0xFFFF
			if(u32.word[0] != 0xFFFF) 
			{
				hal_EEPROM_WritePartialPage((uint32_t)(write_location), (uint8_t *)&u32.phrase, 4);
				write_location += 4;
			}
		}
		
		// erase this page
		hal_EEPROM_ErasePage((uint32_t)data_page_ptr);
		
		data_page_ptr = next_data_page;
		_next_location = -1;
	}
	else 
	{
		// write to the page
		u32.word[1] = address;
		u32.word[0] = data;
		
		hal_EEPROM_WritePartialPage((uint32_t)(data_page_ptr + i), (uint8_t *)&u32.phrase, 4);
		
		_next_location = i + 4;
	}
	// else EEPROM SIZE must be 512 and it must be completely full meaning we just repacked and wrote the new date during that process
}

uint32_t hal_EEPROM_ErasePage(uint32_t page_address)
{///
    uint32_t result;
    
    uint32_t address = page_address - (EFLASH_MEMORY_ADDRESS);
    __disable_irq();
    result = flexspi_nor_flash_erase_sector(EXAMPLE_FLEXSPI, address);
    __enable_irq();
    DCACHE_InvalidateByRange(page_address, HAL_EEPROM_PAGE_SIZE);
    
    return result;
}

uint32_t hal_EEPROM_EraseAll(void) 
{///
    uint32_t result;
    uint32_t page_address;
    
    page_address = ((uint32_t)eeprom_data1) - (EFLASH_MEMORY_ADDRESS);
    __disable_irq();
    result = flexspi_nor_flash_erase_sector(EXAMPLE_FLEXSPI, page_address);
    __enable_irq();
    DCACHE_InvalidateByRange((uint32_t)eeprom_data1, HAL_EEPROM_PAGE_SIZE);
    
    page_address = ((uint32_t)eeprom_data2) - (EFLASH_MEMORY_ADDRESS);
    __disable_irq();
    result |= flexspi_nor_flash_erase_sector(EXAMPLE_FLEXSPI, page_address);
    __enable_irq();
    DCACHE_InvalidateByRange((uint32_t)eeprom_data2, HAL_EEPROM_PAGE_SIZE);
      
    return result;
}

uint32_t hal_EEPROM_WritePartialPage(uint32_t page_address, uint8_t * data, uint16_t length) 
{///
    uint32_t result;
    uint32_t address = page_address - (EFLASH_MEMORY_ADDRESS);
    __disable_irq();
    result = flexspi_nor_flash_program(EXAMPLE_FLEXSPI, address, (uint32_t *)data, length);
    __enable_irq();
    DCACHE_InvalidateByRange(page_address, HAL_EEPROM_PAGE_SIZE);
    
    return result;
}

uint32_t hal_EEPROM_WritePage(uint32_t page_address, uint8_t * data) 
{///
    uint32_t result;
    
    uint32_t address = page_address - (EFLASH_MEMORY_ADDRESS);
    __disable_irq();
    result = flexspi_nor_flash_page_program(EXAMPLE_FLEXSPI, address, (uint32_t *)data);
    __enable_irq();
    DCACHE_InvalidateByRange(page_address, HAL_EEPROM_PAGE_SIZE);
    
    return result;
}

const uint32_t * GetActivePage(void) 
{///
	const uint32_t * ptr;
	if((eeprom_data1[0] & 0x80000000) == 0) 
	{
		ptr = &eeprom_data1[0];
	}
	else if((eeprom_data2[0] & 0x80000000) == 0)
	{ 
		ptr = &eeprom_data2[0];
	}
	else 
	{
		ptr = 0;
	}
	return ptr;
}

int GetNextLocation(void) 
{///
	if(_next_location > 0) 
	{
		return _next_location;
	}
	
	int i;
	for(i = ((FLASH_PAGE_SIZE)-1); i >= 0; i--) {
		if(*(data_page_ptr+i) != 0xFFFFFFFF) {
			_next_location = i + 1;
			return _next_location;
		}
	}
	return 0;
}



