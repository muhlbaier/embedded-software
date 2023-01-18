#ifndef _BOOTLOADER_LOADER_H_
#define _BOOTLOADER_LOADER_H_

/** @brief Bootloader Loader module for the DCDC ARM
 *
 * This module will check the bootloader versoin against a version stored in user flash
 * and replace the bootloader with the version stored in user flash if the versions don't
 * match.
 *
 * See the below link for usage details:
 * https://alencondev.atlassian.net/wiki/display/DOC/Updating+the+DCDC+ARM+Bootloader
 * 
 * @defgroup bootloader_loader DCDC Bootloader Loader
 * @{
 */

#include "stdint.h"
#include "board.h"

/** memory location of bootloader major version byte, this is BackDoorK0 set in the startup_*.s file */
#define BOOTLOADER_MAJOR_VERSION_ADDRESS (FLASH_BOOTLOADER_START + 0x00000401)

/** memory location of bootloader minor version byte, this is BackDoorK1 set in the startup_*.s file */
#define BOOTLOADER_MINOR_VERSION_ADDRESS (FLASH_BOOTLOADER_START + 0x00000400)

/** macro to retrieve the bootloader major version from BOOTLOADER_MAJOR_VERSION_ADDRESS */
#define GET_BOOTLOADER_MAJOR_VERSION (*(uint8_t *)(BOOTLOADER_MAJOR_VERSION_ADDRESS))

/** macro to retrieve the bootloader minor version from BOOTLOADER_MINOR_VERSION_ADDRESS */
#define GET_BOOTLOADER_MINOR_VERSION (*(uint8_t *)(BOOTLOADER_MINOR_VERSION_ADDRESS))

/** the major version of the new image is in the image at byte index BOOTLOADER_MAJOR_VERSION_ADDRESS-BOOTLOADER_BASE_ADDRESS */
#define NEW_BOOTLOADER_MAJOR_VERSION bootloader_hex[BOOTLOADER_MAJOR_VERSION_ADDRESS-BOOTLOADER_BASE_ADDRESS]

/** the minor version of the new image is in the image at byte index BOOTLOADER_MINOR_VERSION_ADDRESS-BOOTLOADER_BASE_ADDRESS */
#define NEW_BOOTLOADER_MINOR_VERSION bootloader_hex[BOOTLOADER_MINOR_VERSION_ADDRESS-BOOTLOADER_BASE_ADDRESS]

/** the base address of the bootloader, 0x00000000 for MKV31, 0x10000000 for MKV58 */
extern const uint32_t BOOTLOADER_BASE_ADDRESS;

/** the length of the new bootloader hex file / bootloader_hex array */
extern const uint32_t BOOTLOADER_HEX_FILE_LENGTH;

/** new bootloader image. See https://alencondev.atlassian.net/wiki/display/DOC/Updating+the+DCDC+ARM+Bootloader for details on creating this array. */
extern const uint8_t bootloader_hex[];

/** @brief Check the existing bootloader version against the image stored in bootloader_hex[] and replace the existing version if they differ.
 *
 * This function should be called to check if the bootloader needs to be updated. If not it returns. If so it will replace the bootloader then reset the processor.
 */
void BootloaderLoader_CheckAndUpdate(void);

/** @} */
#endif
