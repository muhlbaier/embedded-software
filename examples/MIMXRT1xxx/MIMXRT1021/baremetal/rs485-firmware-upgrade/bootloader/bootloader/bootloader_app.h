#ifndef _BOOTLOADER_APP_H_
#define _BOOTLOADER_APP_H_
/** @brief this file is intended to be included with the application 
 * to assist in switching to the bootloader mode when it is time for a 
 * firmware update
 *
 */
#include "RS485Controller.h"
#include "board.h" // for RAM_END_ADDRESS definition

/** @brief use the very top of memory to hold the flag 
 * 
 * Valid values are defined below
 */

#define BOOT_FLAG *(uint32_t *)(RAM_END_ADDRESS-8)

#define BOOT_FLAG_UPDATE_FIRMWARE 0x0003
#define BOOT_FLAG_UPDATE_FIRMWARE_STEP2 0x0004

/** @brief  Start address of relocated interrupt vector table
 *
 * The location where the user firmware starts.
 */
// BOOTLOADER_SIZE needs to be defined by compiler directive
//#define RELOCATED_VECTORS          BOOTLOADER_SIZE                            

#define BOOTLOADER_VERSION *(uint16_t *)(FLASH_BOOTLOADER_START + 0x00000400)

/** @brief initialize bootloader and register rs485 callback for handeling
 * RS485_CTRL_FW_MSG message types.
 */
void BootloaderApp_Init(rs485ctrlr_handle_t rs485ctlr, uint16_t address);

#endif
