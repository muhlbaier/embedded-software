#include <stdint.h>
#include <stdbool.h>

#include "eeprom.h"
#include "SSD_FTFx.h"
#include "library.h"
#include "project_settings.h"

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


#if EEPROM_SIZE > 2048
#warning "Max eeprom size is 2048 16-bit words"
#endif

// size of array to copy__Launch_Command function to
// It should be at least equal to actual size of __Launch_Command func
// User can change this value based on RAM size availability and actual size of __Launch_Command function
#define LAUNCH_CMD_SIZE           0x100
#define FTFx_REG_BASE             0x40020000
#define P_FLASH_BASE              0x10000000
// Program Flash block information
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
// Data Flash block information
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
// Flex Ram block information
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS

// data units per sector
#define EEPROM_DATA_UNITS_PER_SECTOR (FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE / FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE)
#define EEPROM_BYTES_PER_WRITE FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE
// pointer to flash functions in RAM
static pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

// Array to copy __Launch_Command func to RAM.
static uint16_t ramFunc[LAUNCH_CMD_SIZE/2];

// two pages/blocks of data, 2048 32-bit words each
#ifndef EEPROM_DATA1
#define EEPROM_DATA1 0x100FC000
#endif
#ifndef EEPROM_DATA2
#define EEPROM_DATA2 0x100FE000
#endif
const uint64_t * eeprom_data1 = (uint64_t*)EEPROM_DATA1;
const uint64_t * eeprom_data2 = (uint64_t*)EEPROM_DATA2;
/* requires something like the following added to the scatter file:
#define m_eeprom_start                 0x100FC000
#define m_eeprom_size                  0x00004000
...
LR_m_text m_text_start m_text_size {   ; load region size_region
  ...
  ER_m_eeprom m_eeprom_start EMPTY m_eeprom_size { ; special area for virtual eeprom
  }
  ...
*/

/* the following can be used if you don't care where eeprom goes and don't care if it is erased when programming or bootloading
__align(8192) const uint32_t eeprom_data1[2048];
__align(8192) const uint32_t eeprom_data2[2048];
*/

// Flash Standard Software Driver Structure.
static FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /*! FTFx control register base */
    P_FLASH_BASE,           /*! Base address of PFlash block */
    P_FLASH_SIZE,           /*! Size of PFlash block */
    FLEXNVM_BASE,           /*! Base address of DFlash block */
    0,                      /*! Size of DFlash block */
    EERAM_BASE,             /*! Base address of EERAM block */
    0,                      /*! Size of EEE block */
    0,                      /*! Background debug mode enable bit */
    NULL_CALLBACK           /*! Pointer to callback function */
};

// internal functions
const uint64_t * GetActivePage(void);
int GetNextLocation(void);

static int _next_location = -1;
static const uint64_t * data_page_ptr;

void hal_EEPROM_FlashInit(void) {
    static uint8_t initialized = 0;
    if(initialized == 0) {
        // Setup flash SSD structure for device and initialize variables.
        FlashInit(&flashSSDConfig); // returns result code
        
        // Set command to RAM.
        g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)ramFunc , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);
        
        initialized = 1;
    }
}

void hal_EEPROM_Init(void) {
    static uint8_t initialized = 0;
    if(initialized == 0) {
        hal_EEPROM_FlashInit();
        
        // check if both pages are active, if so we need to erase them both:
        if(eeprom_data1[0] != 0xFFFFFFFFFFFFFFFF && eeprom_data2[0] != 0xFFFFFFFFFFFFFFFF) {
            #warning "Make the larger EEPROM of the two active EEPROMs the active one instead of wiping both out if a repack failed during a write."
            DisableInterrupts();
            FlashEraseSector(&flashSSDConfig, (uint32_t)&eeprom_data1[0], FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
            FlashEraseSector(&flashSSDConfig, (uint32_t)&eeprom_data2[0], FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
            EnableInterrupts();
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

uint16_t hal_EEPROM_Read(uint16_t address) {
	int i;
	union64_t u64;
	// make sure the data_page_ptr is good
	if(data_page_ptr != &eeprom_data1[0] && data_page_ptr != &eeprom_data2[0]) return 0xFFFF;
	// find the most recent occurance of the address
	for(i = GetNextLocation()-1; i >= 0; i--) {
		u64.quad_word = *(data_page_ptr+i);
		if(address == u64.word[1]) {
			return (u64.word[0] & 0xFFFF);
		}
	}
	// if address not found then return 0xFFFF
	return 0xFFFF;
}

void hal_EEPROM_Write(uint16_t address, uint16_t data) {
	union64_t u64;

	// make sure the data is different than stored before writing
	if(hal_EEPROM_Read(address) == data) return;
	
	// get end of page
	int i;
	i = GetNextLocation();
	
	// if no space is left in the current page then repack to the other page and erase this page
	if(i >= EEPROM_DATA_UNITS_PER_SECTOR) {
		const uint64_t * next_data_page;
		if(data_page_ptr == &eeprom_data1[0]) {
			next_data_page = &eeprom_data2[0];
		}else {
			next_data_page = &eeprom_data1[0];
		}
		// make sure the other page is erased
		DisableInterrupts();
		FlashEraseSector(&flashSSDConfig, (uint32_t)next_data_page, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
		EnableInterrupts();
		
		// move data from this page to the other page (make sure to update the data for this address if it exists aleady)
		// where to write the data
		uint32_t write_location;
		write_location = (uint32_t)next_data_page;
		int j;
		for(i = 0; i < EEPROM_SIZE; i++) {
			// get the data
			if(i == address) {
				u64.word[0] = data;
				u64.word[1] = address;
			}else {		
				for(j = EEPROM_DATA_UNITS_PER_SECTOR - 1; j >= 0; j--) {
					u64.quad_word = *(data_page_ptr+j);
					if(i == u64.word[1]) {
						break;
					}else {
						u64.word[0] = 0xFFFF;
					}
				}
			}
			// write it to the new page if it is non 0xFFFF
			if(u64.word[0] != 0xFFFF) {
				DisableInterrupts();
				FlashProgram(&flashSSDConfig, write_location, EEPROM_BYTES_PER_WRITE, (uint8_t *)&u64.quad_word, g_FlashLaunchCommand); // returns result code
				EnableInterrupts();
				write_location += EEPROM_BYTES_PER_WRITE;
			}
		}
		
		// erase this page
		DisableInterrupts();
		FlashEraseSector(&flashSSDConfig, (uint32_t)data_page_ptr, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
		EnableInterrupts();
		
		data_page_ptr = next_data_page;
		_next_location = -1;
	}else {
		// write to the page
		u64.word[1] = address;
		u64.word[0] = data;
		DisableInterrupts();
		FlashProgram(&flashSSDConfig, (uint32_t)(data_page_ptr + i), EEPROM_BYTES_PER_WRITE, ((uint8_t *)&u64.quad_word), g_FlashLaunchCommand); // returns result code
		EnableInterrupts();
		_next_location = i + 1;;
	}
	// else EEPROM SIZE must be 512 and it must be completely full meaning we just repacked and wrote the new date during that process
}

uint32_t hal_EEPROM_ErasePage(uint32_t page_address) {
    uint32_t result;
    DisableInterrupts();
    result = FlashEraseSector(&flashSSDConfig, page_address, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
    EnableInterrupts();
    return result;
}

uint32_t hal_EEPROM_EraseAll(void) {
    uint32_t result;
    DisableInterrupts();
    result = FlashEraseSector(&flashSSDConfig, (uint32_t)&eeprom_data1[0], FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
    result = FlashEraseSector(&flashSSDConfig, (uint32_t)&eeprom_data2[0], FTFx_PSECTOR_SIZE, g_FlashLaunchCommand); // returns result code
    EnableInterrupts();
    return result;
}

uint32_t hal_EEPROM_WritePartialPage(uint32_t page_address, uint8_t * data, uint16_t length) {
    uint32_t result;
    DisableInterrupts();
    result = FlashProgram(&flashSSDConfig, page_address, length, data, g_FlashLaunchCommand);
    EnableInterrupts();
    return result;
}

uint32_t hal_EEPROM_WritePage(uint32_t page_address, uint8_t * data) {
    uint32_t result;
    DisableInterrupts();
    result = FlashProgram(&flashSSDConfig, page_address, FTFx_PSECTOR_SIZE, data, g_FlashLaunchCommand);
    EnableInterrupts();
    return result;
}

const uint64_t * GetActivePage(void) {
	const uint64_t * ptr;
	if((eeprom_data1[0] & 0x80000000) == 0) ptr = &eeprom_data1[0];
	else if((eeprom_data2[0] & 0x80000000) == 0) ptr = &eeprom_data2[0];
	else ptr = 0;
	return ptr;
}

int GetNextLocation(void) {
	if(_next_location > 0) return _next_location;
	int i;
	for(i = EEPROM_DATA_UNITS_PER_SECTOR - 1; i >= 0; i--) {
		if(*(data_page_ptr+i) != 0xFFFFFFFFFFFFFFFF) {
			_next_location = i + 1;
			return _next_location;
		}
	}
	return 0;
}
