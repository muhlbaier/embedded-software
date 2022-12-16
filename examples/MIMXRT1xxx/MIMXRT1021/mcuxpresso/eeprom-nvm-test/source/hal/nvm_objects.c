#include "system.h"
#include "hal_eeprom.h"
//#include "eeprom.h" // for definition of hal_EEPROM_Init()
#include "nvm_objects.h"
#include "string.h"


#define BANK1_ADDRESS NVM_OBJECT_START_ADDRESS
#define BANK2_ADDRESS (NVM_OBJECT_START_ADDRESS + HAL_EEPROM_PAGE_SIZE * NVM_OBJECT_MAX_PAGES)
static uint8_t RepackObjects(void);

static struct {
    uint8_t last_active_page;   // use removed due to bugs, can be reimplemented to make searching faster
    uint32_t start_of_bank;
} nvm_object;

// NVM_Object_Init not exposed as any call to the other functions will call the init function to make sure it is initialized
static void NVM_Object_Init(void) 
{
    static int init = 0;
    // if initiazlized already then return
    if(init) return;
    hal_EEPROM_Init();
    nvm_object.start_of_bank = BANK1_ADDRESS;
    // bank 1      bank 2     meaning
    // F..F F..F   F..F F..F  initial condition
    // 0..0 xxxx   F..F F..F  bank 1 in use
    // 0..0 xxxx   F..F xxxx  bank 1 being repacked to bank 2
    // F..F F..F   F..F xxxx  bank 1 repacked into bank 2 but erase didn't finish
    // F..F F..F   0..0 xxxx  bank 2 in use
    // F..F xxxx   0..0 xxxx  bank 2 being repacked to bank 1
    // F..F xxxx   F..F F..F  bank 2 repacked into bank 1 but erase didn't finish
    // determine which bank we are using, the first 32 bits of each bank are left as 0xFFFFFFFF until they are invalidated (0x00000000) by a new bank being used
    if(*(uint32_t *)nvm_object.start_of_bank != 0) {
        // if bank 2 is 0 then it is the active bank
        if(*(uint32_t *)(BANK2_ADDRESS) == 0) {
            nvm_object.start_of_bank = BANK2_ADDRESS;
        }else {
            // if bank 2 word 2 is not FFFF then bank 1 must have repacked to bank 2 but didnt finish otherwise
            // either erase of bank 2 didn't finish or it is the initial condition
            // in all these cases the active bank needs its first word written to 0
            if(*(uint32_t *)(BANK2_ADDRESS + NVM_OBJECT_WRITE_SIZE) != 0xFFFFFFFF) {
                // bank 1 must have repacked to bank 2 but didn't finsih
                nvm_object.start_of_bank = BANK2_ADDRESS;
            }
            uint8_t data[NVM_OBJECT_WRITE_SIZE] = {0};
            hal_EEPROM_WritePartialPage(nvm_object.start_of_bank, data, NVM_OBJECT_WRITE_SIZE);
        }
    }
    init = 1;
}

void NVM_Object_EraseAll(void) 
{
    int page;
    uint32_t * page_start_address;
    // make sure we are initalized
    
	NVM_Object_Init();
	// Go through and erase each page in bank 1
    for(page = 0; page < NVM_OBJECT_MAX_PAGES; page++) {
		
		page_start_address = (uint32_t *)(BANK1_ADDRESS + page * HAL_EEPROM_PAGE_SIZE);
		hal_EEPROM_ErasePage((uint32_t) page_start_address);
    }
	// Go through and erase each page in bank 2
    for(page = 0; page < NVM_OBJECT_MAX_PAGES; page++) {
		page_start_address = (uint32_t *)(BANK2_ADDRESS + page * HAL_EEPROM_PAGE_SIZE);
		hal_EEPROM_ErasePage((uint32_t) page_start_address);
    }
	
	// Reset start of bank to bank 1 and write condition bank 1 in use
	nvm_object.start_of_bank = BANK1_ADDRESS;
	uint8_t data[NVM_OBJECT_WRITE_SIZE] = {0};
    hal_EEPROM_WritePartialPage(nvm_object.start_of_bank, data, NVM_OBJECT_WRITE_SIZE);
}

void * NVM_Object_Get(enum nvm_object_id id, uint16_t * length_pointer) {
    int page;
    void * data_ptr = 0;
    uint16_t object_length = 0;
    uint16_t temp_length;
    uint16_t * object_id_ptr;
    uint32_t * object_ptr;
    // make sure we are initalized
    NVM_Object_Init();
    // search starting from the last page in bank
    for(page = NVM_OBJECT_MAX_PAGES-1; page >= 0; page--) {
        // within the page start at the begining and go to the end
        object_ptr = (uint32_t *)(nvm_object.start_of_bank + page * HAL_EEPROM_PAGE_SIZE + NVM_OBJECT_WRITE_SIZE);  // + NVM_OBJECT_WRITE_SIZE to skip the bank valid bits
        object_id_ptr = (uint16_t *)object_ptr;
        temp_length = *(object_id_ptr + 1);
        
		while(temp_length > 0 && temp_length <= NVM_OBJECT_MAX_LENGTH) {
            if(*object_id_ptr == id) {
                object_length = temp_length;
                data_ptr = (uint8_t *)(object_id_ptr) + NVM_OBJECT_WRITE_SIZE; // data starts at the next write size after id / length
            }
            // object_ptr is 32bit so add (NVM_OBJECT_WRITE_SIZE - 1) and NVM_OBJECT_WRITE_SIZE (to skip id/length word) to byte length then divide by 4 to make sure we are at the next NVM_OBJECT_WRITE_SIZE byte boundry
            object_ptr = object_ptr + (((temp_length + (NVM_OBJECT_WRITE_SIZE - 1) + NVM_OBJECT_WRITE_SIZE) / NVM_OBJECT_WRITE_SIZE) * (NVM_OBJECT_WRITE_SIZE / 4));
            object_id_ptr = (uint16_t *)object_ptr;
            temp_length = *(object_id_ptr + 1);
        }
        // if we found the object in this page then stop looking
        if(data_ptr) break;
    }
    
    
    
        
    if(length_pointer) {
        *length_pointer = object_length;
    }
    return data_ptr;
}

void NVM_Object_Set(enum nvm_object_id id, uint16_t length, void * object_data) {
    static uint8_t packed = 0;
    int page;
	uint32_t page_start_addr;
	uint32_t page_end_addr;
    uint16_t temp_length;
    uint16_t * object_id_ptr;
    uint32_t * object_ptr;
    int16_t remaining_length;
    uint16_t id_and_length[NVM_OBJECT_WRITE_SIZE / 2] = {0};
    // make sure we are initalized
    NVM_Object_Init();
    // read the object to make sure we actually need to write it
    object_ptr = NVM_Object_Get(id, &temp_length);
    // if length and contents are equal then just return, no need to write
    if(temp_length == length) {
        if(memcmp(object_data, object_ptr, length) == 0) {
            return;
        }
    }
    // look for room to write the object starting at the first page of bank (this allows us to pack smaller objects into spaces on previously written pages)
    for(page = 0; page < NVM_OBJECT_MAX_PAGES; page++) {
		
		// calculate page start and page end addresses to ensure memory read/write bounds
		page_start_addr = (nvm_object.start_of_bank + page * HAL_EEPROM_PAGE_SIZE);
		page_end_addr = (nvm_object.start_of_bank + (page + 1) * HAL_EEPROM_PAGE_SIZE);
        
        
       
        // we need room for the object plus the id and length so subtract NVM_OBJECT_WRITE_SIZE from remaining length and another NVM_OBJECT_WRITE_SIZE for the bank valid bits
        remaining_length = HAL_EEPROM_PAGE_SIZE - (NVM_OBJECT_WRITE_SIZE * 2);
        // within the page start at the begining and go to the end
        object_ptr = (uint32_t *)(nvm_object.start_of_bank + page * HAL_EEPROM_PAGE_SIZE + NVM_OBJECT_WRITE_SIZE);  // + NVM_OBJECT_WRITE_SIZE to skip the bank valid bits
        object_id_ptr = (uint16_t *)object_ptr;
        temp_length = *(object_id_ptr + 1);
        
        
        
        // while not a blank object and enough room for this object go to the next object
        while(*object_id_ptr != 0xFFFF && temp_length != 0xFFFF && remaining_length > length) {
			// calculate object length to jump to next object (header + length + padding)
			// (NVM_OBJECT_WRITE_SIZE - 1) and integer division are used to pad any objects that are not a multiple of NVM_OBJECT_WRITE_SIZE
			uint16_t mem_jump_length = (((NVM_OBJECT_WRITE_SIZE + temp_length + (NVM_OBJECT_WRITE_SIZE - 1)) / NVM_OBJECT_WRITE_SIZE) * NVM_OBJECT_WRITE_SIZE);
            remaining_length -= mem_jump_length; // Subtract object length and header size from remaining length
            // object_ptr is 32bit so add (NVM_OBJECT_WRITE_SIZE - 1) and NVM_OBJECT_WRITE_SIZE (to skip id/length word) to byte length then divide by 4 to make sure we are at the next NVM_OBJECT_WRITE_SIZE byte boundry
            object_ptr = object_ptr + (mem_jump_length / 4);
            object_id_ptr = (uint16_t *)object_ptr;
            temp_length = *(object_id_ptr + 1);
			
			// check to make sure memory searching is valid and no junk was loaded into memory pointers
			// check object pointer did not go outside of page range
			if(((uint32_t)object_ptr < page_start_addr) || ((uint32_t)object_ptr > page_end_addr)){
				id_and_length[0] = id;  // Dummy instruction for now, replace with proper error handling
			}
			// check length isn't larger than largest allowed object
			if((temp_length != 0xFFFF) && (temp_length > NVM_OBJECT_MAX_LENGTH)){
				id_and_length[0] = id;  // Dummy instruction for now, replace with proper error handling
			}
			// check object id is valid
			if((*object_id_ptr != 0xFFFF) && (*object_id_ptr > NVM_OBJECT_ID_END)){
				id_and_length[0] = id;  // Dummy instruction for now, replace with proper error handling
			}
			// check object pointer and remaining length match
			if(((uint32_t)object_ptr + NVM_OBJECT_WRITE_SIZE + remaining_length) != page_end_addr){
				id_and_length[0] = id;  // Dummy instruction for now, replace with proper error handling
			}
			
        }
        // if we are at the start of the blank space and enough room exists then write it
        if(*object_id_ptr == 0xFFFF && temp_length == 0xFFFF && remaining_length > length) {
            id_and_length[0] = id;
            id_and_length[1] = length;
            
            
			
			hal_EEPROM_WritePartialPage((uint32_t)object_ptr, (uint8_t *)id_and_length, NVM_OBJECT_WRITE_SIZE);
            object_ptr = (uint32_t *)((uint32_t)object_ptr + NVM_OBJECT_WRITE_SIZE);
            
			hal_EEPROM_WritePartialPage((uint32_t)object_ptr, object_data, ((length + NVM_OBJECT_WRITE_SIZE - 1) / NVM_OBJECT_WRITE_SIZE) * NVM_OBJECT_WRITE_SIZE);
            return;
        }
    }
    // error, not enough room to store object, repack and try again
    if(packed == 0) {
        packed = 1;
        RepackObjects();
        NVM_Object_Set(id, length, object_data);
        packed = 0;
    }
}

uint8_t RepackObjects(void) {
    const uint32_t new_bank = nvm_object.start_of_bank == BANK1_ADDRESS ? BANK2_ADDRESS : BANK1_ADDRESS;
    const uint32_t old_bank = nvm_object.start_of_bank;
    int page;
    enum nvm_object_id id;
    uint8_t * data_ptr;
    uint16_t length, remaining_length;
    uint16_t id_and_length[NVM_OBJECT_WRITE_SIZE / 2] = {0};
    uint32_t object_id_address;
    // start by erasing the new bank
    for(page = 0; page < NVM_OBJECT_MAX_PAGES; page++) {
        hal_EEPROM_ErasePage(new_bank + page * HAL_EEPROM_PAGE_SIZE);
    }
    // go through each id to copy over
    /// @todo if needed optomize this to track remaining length on all pages and fit smaller objects in previous pages.
    page = 0;
    object_id_address = new_bank + NVM_OBJECT_WRITE_SIZE;
    // we need room for the object plus the id and length so subtract NVM_OBJECT_WRITE_SIZE from remaining length and another NVM_OBJECT_WRITE_SIZE for the bank valid bits
    remaining_length = HAL_EEPROM_PAGE_SIZE - (NVM_OBJECT_WRITE_SIZE * 2);
    for(id = NVM_OBJECT_ID_START; id < NVM_OBJECT_ID_END; id++) {
        data_ptr = NVM_Object_Get(id, &length);
        // if out of length then go to next page
        if(length > remaining_length) {
            if(page < NVM_OBJECT_MAX_PAGES - 1) {
                page++;
                remaining_length = HAL_EEPROM_PAGE_SIZE - (NVM_OBJECT_WRITE_SIZE * 2);
            }else {
                // out of room, must drop the object
                /// @todo add warning about this
                length = 0;
            }
        }
        if(length) {
            id_and_length[0] = id;
            id_and_length[1] = length;
            hal_EEPROM_WritePartialPage(object_id_address, (uint8_t *)id_and_length, NVM_OBJECT_WRITE_SIZE);
            length = ((length + NVM_OBJECT_WRITE_SIZE - 1) / NVM_OBJECT_WRITE_SIZE) * NVM_OBJECT_WRITE_SIZE;
            hal_EEPROM_WritePartialPage(object_id_address + NVM_OBJECT_WRITE_SIZE, data_ptr, length);
            // change length to total object length in bytes including padding to maintain NVM_OBJECT_WRITE_SIZE byte boundaries
            length = length + NVM_OBJECT_WRITE_SIZE;
            object_id_address += length;
            remaining_length -= length;
        }
    }
    // erase the active bank first block
    hal_EEPROM_ErasePage(old_bank);
    // mark the new bank as active
    uint8_t data[NVM_OBJECT_WRITE_SIZE] = {0};
    hal_EEPROM_WritePartialPage(new_bank, data, NVM_OBJECT_WRITE_SIZE);
    nvm_object.start_of_bank = new_bank;
    // Info Page repack complete
    return 1;
}

void * NVM_Object_Get_AltBank(enum nvm_object_id id, uint16_t * length_pointer, uint32_t alternate_bank) {
    void * return_pointer;
    // make sure we are initalized
    NVM_Object_Init();
    // back up the start_of_bank
    const uint32_t original_start_of_bank = nvm_object.start_of_bank;
    // update to alternate bank address and run Get function
    nvm_object.start_of_bank = alternate_bank;
    return_pointer = NVM_Object_Get(id, length_pointer);
    // restore start_of_bank then return
    nvm_object.start_of_bank = original_start_of_bank;
    return return_pointer;
}
