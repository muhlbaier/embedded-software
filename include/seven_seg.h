#ifndef _SEVEN_SEG_H_
#define _SEVEN_SEG_H_

/**
 * @defgroup seven_seg Seven Seg Module
 * @brief The Seven Seg Module allows for values to be displayed on a seven segment display
 *
 * What Module Does: This module provides functions to populate up to four seven segment displays.
 * Available functions allow for four displays to be populated based on a singular 16-bit uint16_t input
 * as well as 4 individual 8-bit uint8_t inputs.
 * 
 * How To Use Module: Call SevenSeg_Init() to initialize the module.
 * Once initialized, calling SevenSeg_DisplayHex() or SevenSeg_DisplayRaw()
 * will populate four seven segment displays based on the function inputs.
 *
 * @Authors: Michael Sorce, Alex Marino, Michael Maloney, and David Gaffney
 * @version 0.0
 *
 * @{
 */
 

/** @brief Initialize the Seven Seg Module
 * 
 * Initializes the seven segment module for operation. Calls hal_seven_seg_Init() which can be found in the
 * hal_seven_seg.c file for the respective MCU being used.
 * Initalizes all display values to 0s.
 *
 */ 
void SevenSeg_Init(void);

/** @brief Displays value on seven seg in Hex
 * 
 * This function will convert a 16-bit input into a 4 digit HEX number from 0000 to FFFF.
 * This number will then be displayed on up to four seven segment displays.
 * Max decimal input: 65535
 *
 * @param value number that will be displayed in up to 4 seven segment display in HEX
 */
void SevenSeg_DisplayHex(uint16_t value);

/** @brief Displays individual parameters on up to four seven segment displays
 *
 * This function will take four uint8_t values and display them across four respective seven
 * segment displays. Currently only supports whole numbers 0 to F as parameters.
 *
 * @param seg0 number to be displayed in the left most display
 * @param seg1 number to be displayed in the second display from the left
 * @param seg2 number to be displayed in the third display from the left
 * @param seg3 number to be displayed in the right most display
 *
 * Example: SevenSeg_DisplayRaw(1, 5, 8, 2); => will display as "1582" on seven segment display
 *
 */
void SevenSeg_DisplayRaw(uint8_t seg0, uint8_t seg1, uint8_t seg2, uint8_t seg3);

 /** @} */
#endif /* _SEVEN_SEG_H_ */