#include <stddef.h>
#include "AlenconModbus.h"
#include "SpotTypes.h"
#include "hardware.h"
#include "string.h"
#include "timing.h"
//#include "subsys.h"
#include "mb.h"
// firmware update related includes
#include "bootloader_app.h"
#include "hal_general.h"
#ifdef GARD_BOARD
#include "gard_board.h"
#elif defined CUBE_BOARD
#include "cube_board.h"
#else
#include "board.h"
#endif
#include "dcdc_gate_driver.h"
//#include "fsl_uart_edma_driver.h"
//#include "hal_fsl_wdog.h"
#include "eeprom.h"
#include "crc16.h"
//#include "SSD_FTFx.h"
#include "bootloader.h"
#include "int_def.h"
#include "AlenVer.h"
#include "mbcrc.h"

#include "mcuid.h"
#include "flash.h"

#define STATUS_LED_TOGGLE
#ifndef MODBUS_TEST

#define RTU_MODBUS_MAP_VERSION_ADDRESS 30001
#define RTU_PRODUCT_CODE_ADDRESS 30002
#define RTU_MCU_CODE_ADDRESS 30003
#define RTU_COMMAND_ADDRESS 40100
#define RTU_TELEMETRY_BASE_ADDRESS 40101
#define RTU_FREQUENCY_ADDRESS RTU_TELEMETRY_BASE_ADDRESS+10
#define RTU_TELEMETRY_LENGTH 20
#define RTU_INDECIES_BASE_ADDRESS 41000
#define RTU_MODBUS_UNIT_ADDRESS_ADDRESS 41002
#define RTU_INDECIES_LENGTH 3
#define RTU_PROVISIONING_BASE_ADDRESS 41900
#define RTU_GUID_VALUE RTU_PROVISIONING_BASE_ADDRESS+4
#define RTU_NEW_SLAVE_ADDRESS RTU_PROVISIONING_BASE_ADDRESS+8
#define RTU_FIRMWARE_BASE_ADDRESS 46000
#define RTU_FIRMWARE_VERSION_LENGTH 8 // includes bootloader version, firmware version, GIT version, and update status
#define RTU_FIRMWARE_START_UPDATE_ADDRESS RTU_FIRMWARE_BASE_ADDRESS+RTU_FIRMWARE_VERSION_LENGTH
#define RTU_WRITE_FIRMWARE_BLOCK_ADDRESS 46009
#define RTU_END_OF_FIRMWARE_ADDRESS 46020
#define RTU_RESET_MCU_ADDRESS 46030
#define RTU_RESET_UPDATE_STATE_ADDRESS 46031
#define RTU_BLOCK_BASE_ADDRESS_ADDRESS 46998

enum {
    FW_STATUS_INIT = 1,
    FW_STATUS_UPDATING_BLOCK,
    FW_STATUS_BLOCK_WRITTEN,
    FW_STATUS_BLOCK_WRITE_FAILED,
    FW_STATUS_UPDATE_COMPLETE,
    FW_STATUS_UPDATE_FAILED,
    FW_STATUS_WRITING_BLOCK
} firmware_status_e;

// number of bytes to write at a time (making this number too large can cause communications to become corrupt)
//#define BYTES_PER_PARTIAL_WRITE 16
#define BYTES_PER_PARTIAL_WRITE 256

static void WriteProvisioning(uint16_t * data, uint16_t address);
static void ReadProvisioning(void);
static void UpdateIndecies(void);
static void UpdateFirmwareVersion(void);
static void StartFirmwareUpdate(uint16_t * data, uint16_t address);
static void WriteFirmwareBlock(uint16_t * data, uint16_t address);
static void UpdateBlockCRC(void);
static void EndOfFirmware(uint16_t * data, uint16_t address);
static void ResetMCUCallback(uint16_t * data, uint16_t address);
static void ResetUpdateState(uint16_t * data, uint16_t address);
static void BlockDataCallback(uint16_t * data, uint16_t address);

// Firmware update helpers
static void StartWriteBlockToFlash(void);
static void UpdateFirmwareCRC(void);
static void WritePartialBlockToFlash(void);
enum bootloader_error_t {
    FLASH_INIT_FAILED,
    FLASH_APPOK_ERASE_FAILED,
    FLASH_APPOK_PROGRAM_FAILED,
    FLASH_BLOCK_ERASE_FAILED,
    FLASH_BLOCK_PROGRAM_FAILED
};
static void ErrorHandler(enum bootloader_error_t error);

// variables and functions for diagnostic UART features
static uint8_t sys_id;
static void DiagnosticCallback(int argc, char *argv[]);
static uint8_t corrupt_block_crc = 0;
static uint8_t corrupt_firmware_crc = 0;

// helper to see if the user image is valid
static uint8_t Boot_StrCompare(const uint8_t *s1, const uint8_t *s2,  uint32_t len);

// 30001
static uint16_t modbus_map_version_product_code_and_mcu_code[3];
// 41000-41002
static uint16_t index_holding_registers[RTU_INDECIES_LENGTH] = {0};
// 41900-41908
static uint16_t provisioning_holding_registers[9] = {0};
// 46000-46007
static uint16_t firmware_version_holding_registers[RTU_FIRMWARE_VERSION_LENGTH] = {0};
// 46008
static uint16_t firmware_start_update_holding_register;
// 46009-46017 (key[1], expected CRC[2], actual CRC[2], partial blocks written[4])
static uint16_t firmware_write_block_holding_registers[9];
// 46020-46024 (key, expected CRC, actual CRC)
static uint16_t end_of_firmware_holding_registers[5];
// 46030
static uint16_t reset_mcu_holding_register;
// 46031
static uint16_t reset_update_state_holding_register;
// 46998-48023 (address[2] and data[BOOTLOADER_BLOCK_SIZE / 2])
static uint16_t block_data_holding_registers[BOOTLOADER_BLOCK_SIZE / 2 + 2];
static uint16_t block_data_holding_registers_backup[BOOTLOADER_BLOCK_SIZE / 2 + 2];

static uint64_t guid;
static uint8_t guid_written_flag = 0;

// other firmware update data
static uint16_t * status_ptr;
static uint32_t _firmware_crc;
// flags to indicate what tasks need to be completed
static uint8_t write_flash_flag = 0;
static uint8_t erase_block_flag = 0;
// vars for tracking which partial block to write
static uint16_t num_partial_writes = 0;
static uint16_t partial_write_index = 0;
// pointer to flash functions in RAM
///pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;
// Array to copy __Launch_Command func to RAM.
uint16_t ramFunc[LAUNCH_CMD_SIZE/2];
// Flash Standard Software Driver Structure.
//FLASH_SSD_CONFIG flashSSDConfig =
//{
    //FTFx_REG_BASE,          /*! FTFx control register base */
    //P_FLASH_BASE,           /*! Base address of PFlash block */
    //P_FLASH_SIZE,           /*! Size of PFlash block */
    //FLEXNVM_BASE,           /*! Base address of DFlash block */
    //0,                      /*! Size of DFlash block */
    //EERAM_BASE,             /*! Base address of EERAM block */
    //0,                      /*! Size of EEE block */
    //DEBUGENABLE,            /*! Background debug mode enable bit */
    //NULL_CALLBACK           /*! Pointer to callback function */
//};
// validation key
uint8_t str_app_ok[8]	= "APP_OK";
// timestamp for delaying when it is time to jump to the user app after bootloading
uint32_t jump_to_app_timestamp;

uint32_t last_erase_address = 0;

void AlenconModbus_Init(void)
{
    uint32_t result, i;
    uint8_t address;
    uint32_t baud_rate;
    
    //debugMSG(0, "A0\n");
    // setup interface for UART diagnostic
    if(Hardware_HasDiagnosticUART()) {
        version_t version;
        version.word = BOOTLOADER_VERSION;
        sys_id = Subsystem_Init("DCDC", version, DiagnosticCallback);
    }
    
    //debugMSG(0, "A1\n");
    // put most used HRs first to shorten search
    Modbus_AddHoldingRegister(RTU_BLOCK_BASE_ADDRESS_ADDRESS, &block_data_holding_registers[0], BlockDataCallback, 0, 1026);
    Modbus_AddHoldingRegister(RTU_WRITE_FIRMWARE_BLOCK_ADDRESS, &firmware_write_block_holding_registers[0], WriteFirmwareBlock, UpdateBlockCRC, 6);
    Modbus_AddHoldingRegister(RTU_PROVISIONING_BASE_ADDRESS, &provisioning_holding_registers[0], WriteProvisioning, ReadProvisioning, 9);
    Modbus_AddHoldingRegister(RTU_INDECIES_BASE_ADDRESS, &index_holding_registers[0], 0, UpdateIndecies, RTU_INDECIES_LENGTH);
    Modbus_AddHoldingRegister(RTU_FIRMWARE_BASE_ADDRESS, &firmware_version_holding_registers[0], 0, UpdateFirmwareVersion, RTU_FIRMWARE_VERSION_LENGTH);
    Modbus_AddHoldingRegister(RTU_FIRMWARE_START_UPDATE_ADDRESS, &firmware_start_update_holding_register, StartFirmwareUpdate, 0, 1);
    Modbus_AddHoldingRegister(RTU_END_OF_FIRMWARE_ADDRESS, &end_of_firmware_holding_registers[0], EndOfFirmware, 0, 5);
    Modbus_AddHoldingRegister(RTU_RESET_MCU_ADDRESS, &reset_mcu_holding_register, ResetMCUCallback, 0, 1);
    Modbus_AddHoldingRegister(RTU_RESET_UPDATE_STATE_ADDRESS, &reset_update_state_holding_register, ResetUpdateState, 0, 1);
    Modbus_AddInputSource(RTU_MODBUS_MAP_VERSION_ADDRESS, &modbus_map_version_product_code_and_mcu_code[0]);
    Modbus_AddInputSource(RTU_PRODUCT_CODE_ADDRESS, &modbus_map_version_product_code_and_mcu_code[1]);
    Modbus_AddInputSource(RTU_MCU_CODE_ADDRESS, &modbus_map_version_product_code_and_mcu_code[2]);
    
    #ifdef AFD_BOARD
    modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_AFD;
    #elif defined CUBE_BOARD
    modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_CUBE;
    #elif defined GARD_BOARD
    modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_GARD;
    #else
    // product code for SPOT is 1, CLLC BOSS is 2, DAB BOSS is 5
    if(Hardware_HasDabTopology()){
        modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_BOSS_DAB;
    }else if(Hardware_HasBidirectionalDCDC()) {
        modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_BOSS_CLLC;
    }else {
        modbus_map_version_product_code_and_mcu_code[1] = PRODUCT_CODE_SPOT;
    }
    #endif
    const hardware_mcu_info_t * mcu_info = Hardware_GetMCU();
    modbus_map_version_product_code_and_mcu_code[2] = mcu_info->combined.mcu_code;
    
    //debugMSG(0, "A2\n");
	
	
    // get guid
    guid = Hardware_GetGUID();
    // set slave ID from eeprom
    EEPROM_Init();
    
    /*
    {
		uint8_t * ptr = (uint8_t *)(APPOK_START_ADDRESS);
		debugMSG(DWARNING, "FLASH_APP_START: %x  \n",APPOK_START_ADDRESS);
		uint32_t k;
		for(k=0;k<0x40;k++)
		{
			uint8_t data = ptr[k];
			__ISB();
			__DSB();
			__DMB();
			if((k%16) == 0)
			{
				debugMSG(DAPPEND, "\n%x  ",k);
			}
			else
			{
				debugMSG(DAPPEND, "%xb ",data);
			}
		}
		
		debugMSG(DAPPEND, "\n");
		//for(k=0x1F00;k<0x2100;k++)
		//{
			//if((k%16) == 0)
			//{
				//debugMSG(DAPPEND, "\n%x  ",k);
			//}
			//else
			//{
				//debugMSG(DAPPEND, "%xb ",ptr[k]);
			//}
		//}
		//debugMSG(DAPPEND, "\n");
		//JumpToUserApplication(0x60103000,0x60103000);
		
	}
    */
    
    #ifndef AFD_BOARD
    i = offsetof(dcdc_config_t, modbus_address);
    address = EEPROM_Read(i/2);
    //debugMSG(0, "A3\n");

    Modbus_ChangeSlaveAddress(address);
    // set baud rate from eeprom
    i = offsetof(dcdc_config_t, baud_rate);
    baud_rate = EEPROM_Read(i/2);
    baud_rate |= EEPROM_Read(i/2 + 1) << 16;
    Modbus_SetBaudRate(baud_rate);
    #endif
	
	#ifdef AFD_BOARD
	Modbus_ChangeSlaveAddress(MB_DEFAULT_ADDRESS);
	Modbus_SetBaudRate(UART1_BAUD);
	#endif
    
    //debugMSG(0, "A4\n");

    _firmware_crc = CRC16_Init();
    status_ptr = &firmware_version_holding_registers[RTU_FIRMWARE_VERSION_LENGTH-1];
    *status_ptr = FW_STATUS_INIT;
    write_flash_flag = 0;
    erase_block_flag = 0;
    
    CACHE_DISABLE;
        
    // Setup flash SSD structure for device and initialize variables.
    ///result = FlashInit(&flashSSDConfig);
    result = FLASH_Init();
    if (result) ErrorHandler(FLASH_INIT_FAILED);

    // Set command to RAM.
    ///g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)ramFunc , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);
    
    jump_to_app_timestamp = 0;
}

uint8_t AlenconModbus_InitBootloader(void) {
    int i;
    
    // fetch the boot flag
    uint32_t boot_flag;
    boot_flag = BOOT_FLAG;

    // if we are going to boot we need to reset one more time to enable flash erase/write (not sure why this is, maybe not clearing the HSRUN mode on the first hardware_init()?)
    // Note: boot flag should be set to BOOT_FLAG_UPDATE_FIRMWARE by the application before firmware udpate is started
    if(boot_flag == BOOT_FLAG_UPDATE_FIRMWARE) {
        BOOT_FLAG = BOOT_FLAG_UPDATE_FIRMWARE_STEP2;
        DelayMs(1);
        NVIC_SystemReset();
        while(1);
    }
   
    // try to launch to app up to 3 times
    for(i = 0; i < 3; i++) {
        // if boot_flag doesnt indicate we should bootload and user app is valid then jump to user app
        if(boot_flag != BOOT_FLAG_UPDATE_FIRMWARE_STEP2 
            && Boot_StrCompare((uint8_t*)APPOK_START_ADDRESS, str_app_ok, APPOK_LENGTH) == CHECK_OK) 
        {
            return 1;
        }
        DelayMs(1);
    }
    // clear boot flag
    BOOT_FLAG = 0;
    DelayMs(10);
    return 0;
}

static void WriteProvisioning(uint16_t * data, uint16_t address) {
    uint16_t flipped_guid[4];
    
    if(address == RTU_GUID_VALUE - 40000) {
        guid_written_flag = 1;
    }
    // if trying to write a new slave address
    else if(address == RTU_NEW_SLAVE_ADDRESS - 40000) {
        // if the guid was written and if it matches our guid then use the new slave address
        if(guid_written_flag) {
            flipped_guid[3] = provisioning_holding_registers[4];
            flipped_guid[2] = provisioning_holding_registers[5];
            flipped_guid[1] = provisioning_holding_registers[6];
            flipped_guid[0] = provisioning_holding_registers[7];
            if(memcmp(&guid, flipped_guid, 8) == 0) {
                Modbus_ChangeSlaveAddress(*data);
                // not slave address not saved when in bootloader mode
            }
        }
    }
    /// @todo add support for GUID Mask / Value for discovery
}

static void ReadProvisioning(void) {
    uint16_t flipped_guid[4];
    // refresh the GUID
    memcpy(flipped_guid, &guid, 8);
    provisioning_holding_registers[4] = flipped_guid[3];
    provisioning_holding_registers[5] = flipped_guid[2];
    provisioning_holding_registers[6] = flipped_guid[1];
    provisioning_holding_registers[7] = flipped_guid[0];
    
    
    
    //debugMSG(DWARNING, "%xh %xh %xh %xh\n",provisioning_holding_registers[4],provisioning_holding_registers[5],provisioning_holding_registers[6],provisioning_holding_registers[7]);
    guid_written_flag = 0;
}

static void UpdateIndecies(void) {
    // only update slave address for now
    index_holding_registers[RTU_MODBUS_UNIT_ADDRESS_ADDRESS - RTU_INDECIES_BASE_ADDRESS] = Modbus_GetSlaveAddress();
}

static void UpdateFirmwareVersion(void) {
    firmware_version_holding_registers[0] = BOOTLOADER_VERSION;
}

static void StartFirmwareUpdate(uint16_t * data, uint16_t address) {
    uint32_t result;
    //debugMSG(DWARNING, "StartFirmwareUpdate: ADDR: %xh\n",address);
	UART_printf(SUBSYSTEM_UART, "StartFirmwareUpdate: ADDR: %d\r\n",address);
    
    if(*data == 0xB007 || *data == 0xB078) {
        //erase APP_OK to invalidate the user application
        DisableInterrupts();
        
        ///result = FlashEraseSector(&flashSSDConfig, FLASH_APP_START, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
        result = FLASH_EraseSector(FLASH_APP_START, FLASH_SECTOR_SIZE);
        
        last_erase_address = FLASH_APP_START;
        EnableInterrupts();
        if (result) ErrorHandler(FLASH_APPOK_ERASE_FAILED);
        jump_to_app_timestamp = 0;
        
        //debugMSG(DWARNING, "last_erase_address: ADDR: %x\n",last_erase_address);
    	UART_printf(SUBSYSTEM_UART, "last_erase_address: ADDR: %d\r\n",last_erase_address);
        Bootloader_DontLoadApp();
        // reset CRC and update status
        _firmware_crc = CRC16_Init();
        *status_ptr = FW_STATUS_INIT;
        firmware_write_block_holding_registers[5] = 0;
    }
}

static void WriteFirmwareBlock(uint16_t * data, uint16_t address) {
    // make sure we aren't already writing
    
    if(*status_ptr != FW_STATUS_WRITING_BLOCK) {
        // if this is the end of the expected CRC
        if(address == (RTU_WRITE_FIRMWARE_BLOCK_ADDRESS - 40000 + 2)) {
            UpdateBlockCRC();
            //debugMSG(DWARNING, "P1\n");
            // if CRC expected matches calculated CRC then write block and update firmware CRC
            if(firmware_write_block_holding_registers[1] == firmware_write_block_holding_registers[3] &&
                firmware_write_block_holding_registers[2] == firmware_write_block_holding_registers[4]) 
            {
				//debugMSG(DWARNING, "P2\n");
                // check that all partial blocks within the block have been received
                if(firmware_write_block_holding_registers[5] == 0xFFFF) {
                    // check that the block address is within the appropriate address space for this MCU
                    //const uint32_t block_address = block_data_holding_registers[1] + ((uint32_t)(block_data_holding_registers[0]) << 16);
                    // [*][AG][CHANGE
                    uint32_t block_address = block_data_holding_registers[1] + ((uint32_t)(block_data_holding_registers[0]) << 16);
                    
                    //debugMSG(DWARNING, "write block at ADDR: %x\n",block_address);
                	UART_printf(SUBSYSTEM_UART, "write block at ADDR: %d\n\r",block_address);
                    
                    if(block_address >= FLASH_APP_START && block_address < FLASH_END) {
                        // backup the address and block data so it can't be modified over modbus and start the write process
                        StartWriteBlockToFlash();
                        UpdateFirmwareCRC();
                        *status_ptr = FW_STATUS_WRITING_BLOCK;
                        
                        
						//debugMSG(DWARNING, "P3\n");
                    }else {
                        // if the address is bad then likely the wrong image is being loaded so abort the entire update
                        *status_ptr = FW_STATUS_UPDATE_FAILED;
                        
						//debugMSG(DWARNING, "P4\n");
                    }
                }
                firmware_write_block_holding_registers[5] = 0;
            }else {
                // CRC failed
                *status_ptr = FW_STATUS_BLOCK_WRITE_FAILED;
            }
        }
    }
    //debugMSG(DWARNING, "STATUSPTR: %d\n",*status_ptr);
    
}

static void UpdateBlockCRC(void) {
    crc_t crc;
    // our base address is big endian but it needs to be little endian for the CRC calculation
    union32_t address;
    address.word[0] = block_data_holding_registers[1];
    address.word[1] = block_data_holding_registers[0];
    crc = CRC16_Init();
    crc = CRC16_Update(crc, &address.phrase, sizeof(uint32_t));
    crc = CRC16_Update(crc, &block_data_holding_registers[2], 2048);
    crc = CRC16_Finalize(crc);
    if(corrupt_block_crc) {
        corrupt_block_crc = 0;
        crc += 1;
    }
    firmware_write_block_holding_registers[3] = 0;
    firmware_write_block_holding_registers[4] = crc;
}

static void EndOfFirmware(uint16_t * data, uint16_t address) {
    uint32_t result;
    //debugMSG(DNOTIFY, "End of firmware()\n");
    
    // if this is the end of the expected CRC
    if(address == (RTU_END_OF_FIRMWARE_ADDRESS - 40000 + 2)) {
        // if CRC expected matches calculated CRC then write block and update firmware CRC
        if(end_of_firmware_holding_registers[1] == end_of_firmware_holding_registers[3] &&
            end_of_firmware_holding_registers[2] == end_of_firmware_holding_registers[4])
        {
            /// @todo consider adding check for good program by recalculating the CRC on the entire image and validating against expected CRC
            // make sure the image wasn't pre-validated first (this only happens if you use a prevalidated image file)
            if(Boot_StrCompare((uint8_t*)APPOK_START_ADDRESS, str_app_ok, APPOK_LENGTH) != CHECK_OK) {
                // validate the user image
                DisableInterrupts();
                ///result = FlashProgram(&flashSSDConfig, APPOK_START_ADDRESS, APPOK_LENGTH, (uint8_t *)str_app_ok, g_FlashLaunchCommand);
                result = FLASH_Program(APPOK_START_ADDRESS, APPOK_LENGTH, (uint8_t *)str_app_ok);
				
                EnableInterrupts();
                if (result) ErrorHandler(FLASH_APPOK_PROGRAM_FAILED);
            }
            // set jump to app timestamp to initiate jump
            jump_to_app_timestamp = TimeNow();
            *status_ptr = FW_STATUS_UPDATE_COMPLETE;
        }else {
            // CRC failed
            *status_ptr = FW_STATUS_UPDATE_FAILED;
        }
    }
}

static void UpdateFirmwareCRC(void) {
    static uint32_t last_address = 0xFFFFFFFF;
    crc_t crc;
    // our base address is big endian but it needs to be little endian for the CRC calculation
    union32_t address;
    address.word[0] = block_data_holding_registers_backup[1];
    address.word[1] = block_data_holding_registers_backup[0];
    // only update if this is the first time the block is received
    if(address.phrase != last_address) {
        last_address = address.phrase;
        crc = _firmware_crc;
        crc = CRC16_Update(crc, &address.phrase, sizeof(uint32_t));
        crc = CRC16_Update(crc, &block_data_holding_registers_backup[2], BOOTLOADER_BLOCK_SIZE);
        if(corrupt_firmware_crc) {
            corrupt_firmware_crc = 0;
            crc += 1;
        }
        _firmware_crc = crc;
        end_of_firmware_holding_registers[3] = 0;
        end_of_firmware_holding_registers[4] = crc;
    }
}

static void ResetMCUCallback(uint16_t * data, uint16_t address) {
    if(*data == 0xFADE) {
        // reset the processor
        NVIC_SystemReset();
    }
}

static void ResetUpdateState(uint16_t * data, uint16_t address) {
    _firmware_crc = CRC16_Init();
    *status_ptr = FW_STATUS_INIT;
    firmware_write_block_holding_registers[5] = 0;
    Bootloader_DontLoadApp();
}

static void BlockDataCallback(uint16_t * data, uint16_t address) {
    // check if this is the last word in a partial block
    if(((address-7000) & 63) == 63) {
        if(address > 7000) {
            // mark the block as written
            // shift a 1 address/64 times and OR with partial blocks written
            firmware_write_block_holding_registers[5] |= 1 << ((address-7000)>>6);
            *status_ptr = FW_STATUS_UPDATING_BLOCK;
        }
    }
}
    
static void StartWriteBlockToFlash(void) {
    memcpy(block_data_holding_registers_backup, block_data_holding_registers, sizeof(block_data_holding_registers));
    const uint32_t address = block_data_holding_registers_backup[1] + ((uint32_t)(block_data_holding_registers_backup[0]) << 16);
    // set write flag
    
    //debugMSG(0, "StartWriteBlockToFlash(): %x\n",address);
    write_flash_flag = 1;
    // determine if sector needs to be erased
    if(address >= (last_erase_address + P_SECTOR_SIZE)) {
        erase_block_flag = 1;
    }
    
    num_partial_writes = BOOTLOADER_BLOCK_SIZE / BYTES_PER_PARTIAL_WRITE;
    partial_write_index = 0;
}

void ModbusBootloader_Task(void) {
    if(write_flash_flag) {
        WritePartialBlockToFlash();
    }
    // jump_to_app_timestamp is set to TimeNow() when firmware update is complete
    if(jump_to_app_timestamp) {
        if(TimeSince(jump_to_app_timestamp) > 500) {
            // Restart the watchdog so it doesn't reset.
            //HAL_WDOG_Refresh(WDOG);
            NVIC_SystemReset();
            while(1);
        }
    }
}

static void WritePartialBlockToFlash(void) {
    uint32_t result;
    uint32_t offset;
    uint32_t address;
    //debugMSG(0, "WritePartial()\n");
    
    // wrong increment 2048 bytes but increment is 4096? how?
    address = block_data_holding_registers_backup[1] + ((uint32_t)(block_data_holding_registers_backup[0]) << 16);
    
    if(erase_block_flag) {
        erase_block_flag = 0;
        uint32_t result;
        // toggle LED as a timing mark
        STATUS_LED_TOGGLE;
        DisableInterrupts();
        ///result = FlashEraseSector(&flashSSDConfig, address, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
        result = FLASH_EraseSector(address, FLASH_SECTOR_SIZE);
        
        last_erase_address = address;
        EnableInterrupts();
        STATUS_LED_TOGGLE;
        if (result) ErrorHandler(FLASH_BLOCK_ERASE_FAILED);
        /// @todo consider adding check for good erase
        //int16_t number;               /*! Number of longword or phrase to be verified */
        //uint32_t marginReadLevel;      /*! 0=normal, 1=user, 2=factory - margin read for reading */
        // Verify sector if it's been erased.
        // Number of long to verify.
        //number = FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT;
        //for(marginReadLevel = 0; marginReadLevel <=0x2; marginReadLevel++)
        //{
        //	BlockInterrupts();
        //	result = FlashVerifySection(&flashSSDConfig, APPOK_START_ADDRESS&0xFFFFF800, number, marginReadLevel, g_FlashLaunchCommand);
        //	RestoreInterrupts();
        //	if (FTFx_OK != result) while(1);
        //}
    } else {
        offset = partial_write_index * BYTES_PER_PARTIAL_WRITE;
        STATUS_LED_TOGGLE;
        DisableInterrupts();
        ///result = FlashProgram(&flashSSDConfig, address + offset, BYTES_PER_PARTIAL_WRITE, ((uint8_t *)&block_data_holding_registers_backup[2])+offset, g_FlashLaunchCommand);
        
        
        //debugMSG(DWARNING, " ->  %d - %d\n",partial_write_index,num_partial_writes);
        result = FLASH_Program(address + offset, BYTES_PER_PARTIAL_WRITE, ((uint8_t *)&block_data_holding_registers_backup[2])+offset);
        
        //uint32_t k;
        //debugMSG(DWARNING, " ->  %d - %d\n",partial_write_index,num_partial_writes);
        
        
        EnableInterrupts();
        if (result) ErrorHandler(FLASH_BLOCK_PROGRAM_FAILED);
        /// @todo consider adding check for good program
        STATUS_LED_TOGGLE;
        partial_write_index++;
        if(partial_write_index >= num_partial_writes) {
            /// @todo consider recalculating the CRC for the block written and comparing to the expected CRC
            write_flash_flag = 0;
            *status_ptr = FW_STATUS_BLOCK_WRITTEN;
        }
    }
}

//-----------------------------------------------------------------------------
// FUNCTION:    StrCompare
// SCOPE:       Bootloader application system function
// DESCRIPTION: compare if the two strings are same          
// RETURNS:     CHECK_OK - 1: two strings are  same
//				CHECK_FAIL - 0: two strings are  different
//-----------------------------------------------------------------------------
static uint8_t Boot_StrCompare(const uint8_t *s1, const uint8_t *s2,  uint32_t len) {
    while(len) {
        if(*s1++ ==*s2++ )
            len--;
        else
            return CHECK_FAIL ;
    }
    return CHECK_OK;
}

static void ErrorHandler(enum bootloader_error_t error) {
    uint8_t i;
	
    //debugMSG(DERROR, "ErrorHandler()\n");
    switch(error) {
        /// @todo case FLASH_INIT_FAILED:
        /// @todo case FLASH_APPOK_ERASE_FAILED:
        /// @todo case FLASH_APPOK_PROGRAM_FAILED:
        /// @todo case FLASH_BLOCK_ERASE_FAILED:
        /// @todo case FLASH_BLOCK_PROGRAM_FAILED:
        default:
            // flash status LED 4x / second for 10 seconds
            for(i = 0; i < 40; i++) {
                DelayMs(125);
                STATUS_LED_TOGGLE;
            }
            break;
    }
}

static void DiagnosticCallback(int argc, char *argv[]) {
    if(argc) {
        if(strcasecmp(argv[0], "get") == 0) {
            if(argc >= 2) {
                if(strcasecmp(argv[1], "version") == 0) {
                    LogStr("{\"mode\":\"bootloader\", \"bootloader\":\"%x\", \"hardware_id\":%d}\n", BOOTLOADER_VERSION, (uint16_t)Hardware_GetID());
                }
            }else {
                LogMsg(sys_id, "Too few arguments for get");
            }
        }else if(strcasecmp(argv[0], "reset") == 0) {
            // immediate reset for testing purposes
            NVIC_SystemReset();
        }else if(strcasecmp(argv[0], "corrupt_block_crc") == 0) {
            corrupt_block_crc = 1;
        }else if(strcasecmp(argv[0], "corrupt_firmware_crc") == 0) {
            corrupt_firmware_crc = 1;
        }else {
            LogMsg(sys_id, "Unknown command %s", argv[0]);
        }
    }
}
#endif
