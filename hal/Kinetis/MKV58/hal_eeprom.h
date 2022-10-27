#ifndef _HAL_EEPROM_H_
#define _HAL_EEPROM_H_

#include <stdint.h>
#include "SSD_FTFx.h"

// documentation on most hal_eeprom functions is in eeprom.h
// this header is only for non-standard functions that apply to the MKV5 to expose the flash interface for other software modules.

#define HAL_EEPROM_PAGE_SIZE FTFx_PSECTOR_SIZE

uint32_t hal_EEPROM_ErasePage(uint32_t page_address);

uint32_t hal_EEPROM_EraseAll(void);

uint32_t hal_EEPROM_WritePage(uint32_t page_address, uint8_t * data);

uint32_t hal_EEPROM_WritePartialPage(uint32_t page_address, uint8_t * data, uint16_t length);

void hal_EEPROM_FlashInit(void);

#endif
