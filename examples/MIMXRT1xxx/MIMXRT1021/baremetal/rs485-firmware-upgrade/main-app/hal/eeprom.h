/** @file
 * @defgroup eeprom EEPROM Module
 *
 * Provides a constant EEPROM or other persistent memory interface for applications
 * requiring persistent data. On targets which don't have a physical EEPROM
 * this will typically be implemented using program memory (flash)
 * 
 * If the subsystem module (@ref subsys) is used then this module will register
 * as subsystem "EE" to provide a UART/terminal interface for reading and writing
 * the EEPROM.
 * @{
 */

#ifndef _EEPROM_H_
#define	_EEPROM_H_

#include "stdint.h"

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 255
#endif

/** Initialize the EEPROM as required (calls hal_EEPROM_Init() ) and register
 * the module as a subsystem if the subsys module is used. 
 *
 */
void EEPROM_Init(void);

/**
 * 
 * @param address EEPROM address to read from (must be <= EEPROM_SIZE)
 * @return value read from EEPROM
 */
uint16_t EEPROM_Read(uint16_t address);

/**
 * 
 * @param address EEPROM address to write to (must be <= EEPROM_SIZE)
 * @param data data to write to EEPROM
 */
void EEPROM_Write(uint16_t address, uint16_t data);

//////////// HAL INTERFACE /////////////////

/** Initailize the EEPROM */
void hal_EEPROM_Init(void);

/**
 * 
 * @param address EEPROM address to read from (must be <= EEPROM_SIZE)
 * @return value read from EEPROM
 */
uint16_t hal_EEPROM_Read(uint16_t address);

/**
 * 
 * @param address EEPROM address to write to (must be <= EEPROM_SIZE)
 * @param data data to write to EEPROM
 */
void hal_EEPROM_Write(uint16_t address, uint16_t data);

/** @}*/
#endif	/* EEPROM_H */

