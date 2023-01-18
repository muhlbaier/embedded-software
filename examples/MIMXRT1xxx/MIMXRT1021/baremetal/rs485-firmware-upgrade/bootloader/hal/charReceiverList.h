/** @file
 * @defgroup receiverlist Receiver List
 * @ingroup data_strucutre
 *
 * The @c char Receiver List module provides a simple way of keeping
 * a list of character receivers and processing a single character
 * through the receivers.
 *
 * This module was developed as a helper to the @ref uart UART Module
 * but it has use in any situation where multiple moldules may want
 * to process characters which are received.
 *
 * @author Anthony Merlino
 * @author Michael Muhlbaier
 *
 * @{
 */

#ifndef _CHAR_RECEIVER_LIST_H_
#define _CHAR_RECEIVER_LIST_H_

#include <stdint.h>
#include <stdbool.h>
#include "error.h"
/**
 * @warnung AA 7/10
 *
 * The charReceiver_t is used to create a synonym for a pointer to a function that accepts
 * a single argument of type char. 
 *
 * The receiver function will be passed a received character to be processed via CharReceiverList_Run()
 *
 */
typedef void(*charReceiver_t)(char);

/**
 */
typedef struct charReceiverList_t{
	uint16_t max_size; ///< max_size
	uint16_t size; ///< size
	charReceiver_t *receivers; ///< receivers
} charReceiverList_t;

/** Initialize receiver list
 *
 * List initialized to have no receivers and to allow a maximum of max_size receivers to be added to the list.
 *
 * @param rList Pointer to charReciverList_t data type to be initialized
 * @param receiver_array Pointer to charReciever_t array 
 * @param max_size Maximum size of the receiver list 
 *
 * @warning DB 7/10
 */
void CharReceiverList_Init(charReceiverList_t* rList, charReceiver_t* receiver_array, uint16_t max_size);

/**
 * @brief Adds character to the character receive list
 *
 * Add (i.e. register) a receiver to the list of character receivers. This receiver will be run whenever
 * CharReceiverList_Run() is called.
 *
 * Update 5/30/18 - prevents registration of already registered receiver
 * 
 * @param rList pointer to char receiver list
 * @param receiver function pointer to receiver function (function must have no return value and char input parameter)
 * 
 * @warning KC 8/10
 */
int8_t CharReceiverList_Add(charReceiverList_t* rList, charReceiver_t receiver);

/**
 *
 * @param rList
 * @param receiver
 *
 */
void CharReceiverList_Remove(charReceiverList_t* rList, charReceiver_t receiver);

/** Passes the char c into and runs the functions pointed to by receivers in the struct rList.
 * 
 * @param rList pointer to list of character receivers
 * @param c character to be processed by all receivers
 *
 * @twarning VC 9/10
 */
void CharReceiverList_Run(charReceiverList_t* rList, char c);

/** @} */
#endif // _CHAR_RECEIVER_LIST_H_
