#ifndef _HAL_KEYFOB_RECEIVER_H_
#define _HAL_KEYFOB_RECEIVER_H_

 /**
 * @defgroup hal_keyfob HAL Keyfob Receiver Module
 * @ingroup keyfob
 * @file hal_keyfob_receiver.h
 *
 *  Created on: Nov 17, 2015
 * @author Michael Muhlbaier
  * 
  * @todo Add macros to allow other IC modules besides IC1 and IC2 from being used
 * @{
 */

/** Override default bit times to avoid conversion to us each time
 * 
 * HAL is setup to count in microseconds when FCPU is 64000000
 */
#ifndef KEYFOB_MIN_TIME_UNIT
#define KEYFOB_MIN_TIME_UNIT ((280 * (FCPU / 64)) / 1000000)
#endif

#ifndef KEYFOB_MAX_TIME_UNIT
#define KEYFOB_MAX_TIME_UNIT ((520 * (FCPU / 64)) / 1000000)
#endif

/** @} */
#endif // _HAL_KEYFOB_RECEIVER_H_
