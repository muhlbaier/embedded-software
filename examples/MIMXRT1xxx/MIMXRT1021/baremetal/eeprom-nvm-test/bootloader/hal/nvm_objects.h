#ifndef _NVM_OBJECTS_H_
#define _NVM_OBJECTS_H_
/** @file
 * @defgroup nvm_object Non-Volatile Memory Object Module
 *
 * Provides a method of storing and retrieving objects from memory.
 *
 * Depends on the following custom hal_eeprom functions:
 * uint32_t hal_EEPROM_ErasePage(uint32_t page_address)
 * uint32_t hal_EEPROM_WritePage(uint32_t page_address, uint8_t * data)
 * 
 * Provides no protection of data if power is lost while writing.
 *
 * system.h should define the nvm_object_id ennumeration:
 * @code
enum nvm_object_id {
    ZERO_ID_INVALID_OBJECT_ID = 0,      ///< [REQUIRED] used to detect invalid objects
    NVM_OBJECT_ID_START = 1,            ///< [REQUIRED] used as the start id when searching for objects to repack
    NAME_OF_OBJECT_1 = 1,
    NAME_OF_OBJECT_2,
    NVM_OBJECT_ID_END,                  ///< [REQUIRED] used as the end id when searching for objects to repack
    NVM_OBJECT_MAX_VALUE = 0xFFFF       ///< [REQUIRED] this is very important to ensure the compiler make this a 16-bit enum
};
 * Note: take care not to assume the size of the enum, different compilers treat them differently. Some make them 32-bit or more
 * others make them as small as possible. In the example above the max value is used to force the enum to 16-bit if the compiler
 * would have otherwise made it 8-bit.
 * @endcode
 * 
 * Uses flash memory from NVM_OBJECT_START_ADDRESS
 * to NVM_OBJECT_START_ADDRESS + (NVM_OBJECT_MAX_PAGES * 2 + 1) * HAL_EEPROM_PAGE_SIZE - 1
 * @{
 */
 
#include "system.h"
#include "stdint.h"

#ifndef NVM_OBJECT_WRITE_SIZE
#define NVM_OBJECT_WRITE_SIZE FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE
#endif

#ifndef NVM_OBJECT_START_ADDRESS
#if defined CPU_MKV31F256VLL12 || defined CPU_MKV31F256VLH12
#define NVM_OBJECT_START_ADDRESS 0x0003C000
#elif defined CPU_MKV58F1M0VLL24 || defined CPU_MKV58F512VLL24 || defined CPU_MKV56F1M0VLL24 || defined CPU_MKV56F512VLL24
#define NVM_OBJECT_START_ADDRESS 0x10070000
#else
#warning "invalid CPU selection"
#endif
#endif

#ifndef NVM_OBJECT_MAX_PAGES
#define NVM_OBJECT_MAX_PAGES 3
#endif

#define NVM_OBJECT_MAX_LENGTH 2042

/** @brief Get a NVM Object from its ID.
 *
 * Example:
 * @code
 * typedef struct {
 *     float stuff;
 * }stuff_t;
 *
 * stuff_t * pointer;
 * uint16_t length;
 * pointer = NVM_Object_Get(STUFF_ID, &length);
 * // now pointer points to the copy of stuff_t that is in NVM.
 * @endcode
 *
 * Note: since NVM Object memory can be repacked and old pointers may no longer point to good data it
 * is recommended to copy the object to RAM instead of reusing the pointer at a later time.
 *
 * @param id - nvm_object_id, typically this enum resides in system.h since it is project specific
 * @param length_pointer - optional pointer for writing object length
 * @return pointer to the object data
 */
void * NVM_Object_Get(enum nvm_object_id id, uint16_t * length_pointer);

/** @brief Get a NVM Object from its ID.
 *
 * Erase both banks and set the current bank to bank 1.
 */
void NVM_Object_EraseAll(void);

/** @brief Set a NVM Object given the ID, length, and data pointer.
 *
 * Example:
 * @code
 * typedef struct {
 *     float stuff;
 * }stuff_t;
 * stuff_t stuff;
 * // put some stuff into stuff
 * NVM_Object_Set(STUFF_ID, sizeof(stuff), &stuff);
 * @endcode
 *
 *
 * @param id - nvm_object_id, typically this enum resides in system.h since it is project specific
 * @param length - size in system units (usually bytes / same as sizeof()) of the object
 * @param pointer to the object data
 */
void NVM_Object_Set(enum nvm_object_id, uint16_t length, void * object_data);

/** @brief Same as NVM_Object_Get but allows the bank address to be set to support reading from old banks.
 * 
 * @param id - nvm_object_id, typically this enum resides in system.h since it is project specific
 * @param length_pointer - optional pointer for writing object length
 * @param alternate_bank - address of the alternate bank to look for the object in
 * @return pointer to the object data
 */
void * NVM_Object_Get_AltBank(enum nvm_object_id id, uint16_t * length_pointer, uint32_t alternate_bank);

#endif // _NVM_OBJECTS_H_
