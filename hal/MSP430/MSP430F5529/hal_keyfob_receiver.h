#ifndef _HAL_KEYFOB_RECEIVER_H_
#define _HAL_KEYFOB_RECEIVER_H_

 /**
 * @defgroup hal_keyfob HAL Keyfob Receiver Module
 * @ingroup keyfob
 * @file hal_keyfob_receiver.h
 *
 * @author Chris Fredrickson
 * 
 * @{
 */

/** Override default bit times to avoid conversion to us each time
 * 
 * These defines may need to change for the MSP430F5529
 */
#ifndef KEYFOB_MIN_TIME_UNIT
#define KEYFOB_MIN_TIME_UNIT ((280 * FCPU) / 16000000) //280us
#endif

#ifndef KEYFOB_MAX_TIME_UNIT
#define KEYFOB_MAX_TIME_UNIT ((520 * FCPU) / 16000000) //520us
#endif

/** Initialize hardware required to measure time (in microseconds) between edges.
 * 
 */
void hal_Keyfob_Init(void);

/** @} */
#endif // _HAL_KEYFOB_RECEIVER_H_
