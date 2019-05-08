#ifndef _HAL_SEVEN_SEG_H_
#define _HAL_SEVEN_SEG_H_

#include <stdint.h>

 /*
   * CONFIGURATION
   * P1.2 -> B -> PIN 7
   * P1.3 -> E -> PIN 1
   * P1.4 -> F -> PIN 10
   * P1.5 -> A -> PIN 11
   *
   * P2.2 -> G -> PIN 5
   * P2.3 -> D -> PIN 2
   * P2.4 -> Decimal -> PIN 3
   * P2.5 -> C -> PIN 4
   *
   * P3.3 -> D4 -> PIN 6
   * P3.4 -> D3 -> PIN 8
   * P3.5 -> D2 -> PIN 9
   * P3.6 -> D1 -> PIN 12
   *
   * */

/**
 * @defgroup hal_seven_seg Hal Seven Seg Module
 * @brief The Hal Seven Seg Module enables use of the Seven_Seg module with the MSP430F5529
 *
 * What Module Does: Abstracts Seven_Seg functions for use on appropriate pin-out
 *
 *
 * @author: Michael Sorce
 * @author: Alex Marino
 * @author: Michael Maloney
 * @author: David Gaffney
 * @version 0.0
 *
 * @{
 */

/** @brief Initialize the Hal Seven Seg Module
 *
 * Initializes the pin-out for the seven seg module
 *
 */
void hal_seven_seg_Init(void);


/** @brief Displays parameters on 4x7 seg display
 *
 */
void hal_Display(uint8_t seg0, uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t dec);

#endif /* _HAL_SEVEN_SEG_H_ */
