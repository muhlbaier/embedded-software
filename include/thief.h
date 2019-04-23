#ifndef _THIEF_H_
#define _THIEF_H_

#include <stdint.h>

/** @defgroup thief Thief Module
 * 
 * THIEF_SPI should be defined in project_settings.h
 * 
 * @{
 */
 
/** @brief send TRIP message over network to Brainframe
 *
 * @param score byte (0 - score not used, 1 - barely tripped, 255 - not trying)
 */
void Thief_Trip(uint8_t score);

/** @brief send ACCESS message over network to Brainframe
 *
 * @param score byte (0 - score not used, 1 - great job, 255 - face palm)
 */
void Thief_Access(uint8_t score);

/** @brief send ERROR message over network to Brainframe
 *
 * @param error_code byte (codes TBD)
 */
void Thief_Error(uint8_t error_code);

/** @brief initialize the nRF24 Network module for the Thief node and set callbacks
 *
 * @param ChipEnable Function pointer to method that controls chip enable pin via GPIO
 * @param ChipSelectNot Function pointer to method that controls the SPI chip select for the nRF24L01+
 * @param Arm (optional)Function pointer to call when ARM message received from brainframe
 * @param Disarm (optional)Function pointer to call when DISARM message received from brainframe
 * @param Difficulty (optional) Function pointer to call when a non-zero difficulty value is received from the brainframe (1 - easy, 255 - freakin impossible)
 */
void Thief_Init(void(*ChipEnable)(uint8_t), void(*ChipSelectNot)(uint8_t), 
                void(*Arm)(void), void(*Disarm)(void), void(*Difficulty)(uint8_t));
				
 /** @} */
 
#endif // _THIEF_H_