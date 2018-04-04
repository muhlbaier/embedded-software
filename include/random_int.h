/**
 * @defgroup random_int Generate random integer in a given range
 * @ingroup helpers
 *
 *  Created on: Mar 14, 2014
 *      Author: Muhlbaier
 * @{
 */

#ifndef RANDOM_INT_H
#define RANDOM_INT_H

#include <stdint.h>
/** Generate a random integer
 *
 * Generates a pseudo random value between min_num (inclusive) and max_num (exclusive).
 *
 * Based on rand() and srand().
 *
 * Note: srand will be called on the first call of this function. Thus the time to run will
 * be significantly longer on the first call.
 *
 * @param min_num min value to generate
 * @param max_num max value to generate
 * @return
 */
int random_int(int16_t min_num, int16_t max_num);

///@}
#endif /* RANDOM_INT_H */
