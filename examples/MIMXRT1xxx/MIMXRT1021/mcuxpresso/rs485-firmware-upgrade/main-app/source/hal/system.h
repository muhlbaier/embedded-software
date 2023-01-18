#ifndef __SYSTEM_HAL_H__
#define __SYSTEM_HAL_H__                    

#define FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE		4


enum nvm_object_id {
    ZERO_ID_INVALID_OBJECT_ID = 0,      ///< [REQUIRED] used to detect invalid objects
    NVM_OBJECT_ID_START = 1,            ///< [REQUIRED] used as the start id when searching for objects to repack
    NAME_OF_OBJECT_1 = 1,
    NAME_OF_OBJECT_2,
    NVM_OBJECT_ID_END,                  ///< [REQUIRED] used as the end id when searching for objects to repack
    NVM_OBJECT_MAX_VALUE = 0xFFFF       ///< [REQUIRED] this is very important to ensure the compiler make this a 16-bit enum
};


#endif  
