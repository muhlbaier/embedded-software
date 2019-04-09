#ifndef _SEVEN_SEG_H_
#define _SEVEN_SEG_H_

#include <stdint.h>

/**
 * @defgroup seven_seg Seven Seg Module
 * @brief The Seven Seg Module allows for values to be displayed on a seven segment display
 *
 * What Module Does: This module provides functions to populate up to four seven segment displays.
 * Available functions allow for four displays to be populated based on a singular 16-bit uint16_t input
 * as well as 4 individual 8-bit uint8_t inputs.
 * 
 * How To Use Module: Call SevenSeg_Init() to initialize the module.
 * Once initialized, calling SevenSeg_DisplayHex(), SevenSeg_DisplayRaw() or SevenSeg_DisplayFloat()
 * will populate four seven segment displays based on the function inputs. These functions will only be
 * run once, flashing the input on the display. To make a persistently readable number, run function in
 * a while loop. Alternatively, if you run UpdateDisplay as a scheduled task, the displays will be constantly
 * populated, and the inputs can be changed using any of the other functions.
 *
 *@code
 * //code initializes Seven Seg Module and prints number 8569 in Hex and decimal
 *
 * uint16_t value = 8569;
 * SevenSeg_Init(void);
 *
 * //displays 2179 on seven seg display (8569 in HEX)
 * SevenSeg_DisplayHex(value);
 *
 * //displays 8569 on seven seg display
 * SevenSeg_DisplayRaw(8, 5, 6, 9);
 *
 * //displays 79.45 on seven seg display
 * float value2 = 79.45;
 * SevenSeg_DisplayFloat(float value2);
 *
 *  //code to schedule 12.34 to be displayed every 10 ticks
 * void example(void)
 * {
 *   SevenSeg_DisplayFloat(12.34);
 * }
 *
 * Task_Schedule(example, 0, 0, 10);
 *
 *   while(1)
 *   {
 *       SystemTick();
 *       UpdateDisplay();
 *   }
 *
 *@endcode
 *
 * @author: Michael Sorce 
 * @author: Alex Marino
 * @author: Michael Maloney
 * @author: David Gaffney
 * @version 0.1
 *
 * @{
 */
 

/** @brief Initialize the Seven Seg Module
 * 
 * Initializes the seven segment module for operation. Calls hal_seven_seg_Init() which can be found in the
 * hal_seven_seg.c file for the respective MCU being used.
 * Initializes all display values to 0s.
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
 *@code
 * //will display as "1582" on seven segment display
 * SevenSeg_DisplayRaw(1, 5, 8, 2);
 *@endcode
 *
 */
void SevenSeg_DisplayRaw(uint8_t seg0, uint8_t seg1, uint8_t seg2, uint8_t seg3);

/** @brief Displays float value on seven seg in decimal
 * 
 * This function will convert a 32-bit float input into a 4 digit decimal number from 0000 to 9999.
 * This number will then be displayed on up to four seven segment displays.
 * Max decimal input: 9999
 * Will take the most significant 4 digits i.e. 9998.5 will display 9998, 85.123 will display 85.12
 * @warning DOES NOT ROUND
 *
 * @param value float that will be displayed in up to 4 seven segment display in decimal
 */
void SevenSeg_DisplayFloat(float value);

/** @brief Converts binary number (uint16_t) to BCD format
 * 
 * Converts unint16_t input from its binary encoding to BCD format
 *
 * @param value binary input that will be converted to BCD format (BCD - Binary Coded Decimal)
 */
uint16_t SevenSeg_BCD(uint16_t value);

/** @brief updates static variables and displays on hex
 * task-compatible function
 * void function to easily display values on display using scheduled tasks
 * Calls hal_Display to display current value of static variables
 */
void UpdateDisplay(void);

 /** @} */
#endif /* _SEVEN_SEG_H_ */
