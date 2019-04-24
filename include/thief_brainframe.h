#ifndef _THIEF_BRAINFRAME_H_
#define _THIEF_BRAINFRAME_H_

#include <stdint.h>

/** @defgroup thief_brainframe Thief Brainframe Module
 * @ingroup thief
 * 
 * THIEF_SPI should be defined in project_settings.h
 * 
 * This module will be used by the brainframe application and by
 * Thief nodes for testing without the actual brainframe present.
 * @{
 */
 
/** @brief send ARM message over network to a theif node
 *
 * @param difficulty difficulty byte (0 - no change, 1 - easy peasy, 255 - too hard)
 * @param address node address (0x3F for broadcast to all)
 */
void Thief_Arm(uint8_t difficulty, uint8_t address);

/** @brief send DISARM message over network to a theif node
 *
 * @param difficulty difficulty byte (0 - no change, 1 - easy peasy, 255 - too hard)
 * @param address node address (0x3F for broadcast to all)
 */
void Thief_Disarm(uint8_t difficulty, uint8_t address);

/** @brief send DIFFICULTY message over network to a theif node
 *
 * @param difficulty difficulty byte (0 - no change, 1 - easy peasy, 255 - too hard)
 * @param address node address (0x3F for broadcast to all)
 */
void Thief_Difficulty(uint8_t difficulty, uint8_t address);

/** @brief initialize the nRF24 Network module for the Thief Brainframe and set callbacks
 *
 * If using brainframe for testing (e.g. already using the default nrf24 network for Thief_Init)
 * then define THIEF_BRAINFRAME_NETWORK in project_settings.h to be the nrf24network pointer
 * for the brainframe network instance.
 *
 * @param ChipEnable Function pointer to method that controls chip enable pin via GPIO
 * @param ChipSelectNot Function pointer to method that controls the SPI chip select for the nRF24L01+
 * @param Trip (optional)Function pointer to call when TRIP message received from Thief node
 * @param Access (optional)Function pointer to call when ACCESS message received from Thief node
 */
void Thief_BrainframeInit(void(*ChipEnable)(uint8_t), void(*ChipSelectNot)(uint8_t), 
						 void(*Trip)(uint8_t, uint8_t), void(*Access)(uint8_t, uint8_t));
				
 /** @} */
 
#endif // _THIEF_BRAINFRAME_H_
