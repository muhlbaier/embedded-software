#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "bootloader_app.h"

#define BOOTOK_START_ADDRESS       P_FLASH_BASE + 0x400
//#define APPOK_START_ADDRESS	       FLASH_APP_START + 0x000
#define APPOK_START_ADDRESS	       FLASH_APP_START + 0x400
#define APPOK_LENGTH			   8

#define CHECK_OK     			   0
#define CHECK_FAIL   			   1

// writes are done in this size but erase might be bigger (smaller not supported as of 12/16/2021)
#define BOOTLOADER_BLOCK_SIZE   2048

// size of array to copy__Launch_Command function to
// It should be at least equal to actual size of __Launch_Command func
// User can change this value based on RAM size availability and actual size of __Launch_Command function
#define LAUNCH_CMD_SIZE           0x100

#define BUFFER_SIZE_BYTE          0x80

#define DEBUGENABLE               0x00

#define READ_NORMAL_MARGIN        0x00
#define READ_USER_MARGIN          0x01
#define READ_FACTORY_MARGIN       0x02

#define ONE_KB                    1024

#define BACKDOOR_KEY_LOCATION         0x400

// Program flash IFR
#if (FSL_FEATURE_FLASH_IS_FTFE == 1)
#define PFLASH_IFR                0x3C0
#else // FSL_FEATURE_FLASH_IS_FTFL == 1 or FSL_FEATURE_FLASH_IS_FTFA = =1
#define PFLASH_IFR                0xC0
#endif
// Program Flash block information
//#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
//#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
//#define P_SECTOR_SIZE           FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE

#define P_BLOCK_NUM             256
#define P_SECTOR_SIZE           4096
//#define P_SECTOR_SIZE           2048
#define P_FLASH_SIZE            (P_BLOCK_NUM * P_SECTOR_SIZE)

#if P_SECTOR_SIZE < BOOTLOADER_BLOCK_SIZE
#warning "Erase sector size is less than bootloader block size"
#endif

// Data Flash block information
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
#define FLEXNVM_SECTOR_SIZE     FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE
#define FLEXNVM_BLOCK_SIZE      FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SIZE
#define FLEXNVM_BLOCK_NUM       FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_COUNT

// Flex Ram block information
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

// Has flash cache control in MCM module
#if (FSL_FEATURE_FLASH_HAS_MCM_FLASH_CACHE_CONTROLS == 1)
#define CACHE_DISABLE             MCM_WR_PLACR_DFCS(MCM_BASE_PTR, 1);
// Has flash cache control in FMC module
#elif (FSL_FEATURE_FLASH_HAS_FMC_FLASH_CACHE_CONTROLS == 1)
#if defined(FMC_PFB1CR) && defined(FMC_PFB1CR_B1SEBE_MASK)
#define CACHE_DISABLE     FMC_PFB0CR &= ~(FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);\
                                  FMC_PFB1CR &= ~(FMC_PFB1CR_B1SEBE_MASK | FMC_PFB1CR_B1IPE_MASK | FMC_PFB1CR_B1DPE_MASK | FMC_PFB1CR_B1ICE_MASK | FMC_PFB1CR_B1DCE_MASK);
#else
#define CACHE_DISABLE     FMC_PFB0CR &= ~(FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);
#endif
#else
// No cache in the device
#define CACHE_DISABLE
#endif

/** @brief run modbus bootloader tasks
 *
 * This function will check various flags and handle writting data to flash and launching the user app at the right time
 */
 
void Bootloader_Init(void); 
void ModbusBootloader_Task(void);
void Bootloader_DontLoadApp(void);
void JumpToAppStateMachine(uint8_t state) ;
void JumpToUserApplication(uint32_t userSP);

#endif
