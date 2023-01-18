#ifndef _HAL_FLASH_H_
#define _HAL_FLASH_H_

#include <stdint.h>
#include "mcuid.h"

#define FLASH_MEMORY_ADDRESS	0x60000000
#define FLASH_SECTOR_SIZE		4096
#define FLASH_PAGE_SIZE 		256

uint32_t	FLASH_Init(void);
uint32_t	FLASH_EraseSector(uint32_t sector_address, uint32_t sector_size);
uint32_t	FLASH_Program(uint32_t page_address, uint32_t length, uint8_t * data);

#endif
