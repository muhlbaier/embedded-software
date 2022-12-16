#ifndef _BUFFER_H_
#define _BUFFER_H_

// Standard Includes
#include <stdint.h>
#include <stdbool.h>

#include "library.h"

/** @file
 * @defgroup buffer FIFO Byte Buffer
 * @ingroup data_structure
 * 
 * This module implements a software FIFO buffer of bytes. It provides 
 * methods Buffer_Enqueue() and Buffer_Dequeue() to add and remove bytes from the buffer.
 *
 * The user is responsible for allocating the structure used to manage the buffer,
 * buffer_t, as well as the actual byte array which will be used to implement the buffer.
 * These are then passed to Buffer_Init() which must be called prior to any attempts 
 * to Enqueue or Dequeue.
 *
 * There is also a Buffer_EnqueueData() method which allows multiple bytes to be added to the buffer
 * at once.
 *
 * An alternative to this FIFO byte buffer is the Item Buffer module which works the same
 * except with items of any size instead of being limited to bytes only.
 *
 * @author Michael Muhlbaier
 * @author Anthony Merlino
 *
 * @version 0.1 Initial implementation
 * @version 1.0 Made Push and Pop interrupt safe
 * @version 1.1 Added PushData
 * @version 1.2 Added and corrected documentation, removed include "project_settings.h"
 * @version 1.3 Changed char to uint8_t for the data bytes and updated method names to Buffer_[] and Push/Pop to Enqueue/Dequeue
 * @{
 */


/** data structure to hold the required information for each buffer
*/
typedef struct {
    uint16_t size; /**< size is the number of items in the buffer >*/
    uint16_t max_size; /**< not used >*/
    uint8_t *front; /**< pointer to first byte in buffer >*/
    uint8_t *rear; /**< pointer to last byte in the buffer >*/
    uint8_t *buffer_start; /**< buffer start location in memory >*/
    uint8_t *buffer_end; /**< buffer end location in memory (buffer_start + max_size) >*/
    void (*Callback)(void * buf); /**< Enqueue callback, useful if buffer is used
        for communications, does not need to be used/set, initializes to 0>*/
    //x the input to the Callback should be a buffer_t pointer; however, buffer_t
    //x has not been defined thus the pointer is cast as a void pointer.
    void (*OverwriteCallback)(void);
} buffer_t;

/** Buffer_Enqueue will add one item, data, to the FIFO buffer
 * 
 * Buffer_Enqueue will add one item to the rear of the data buffer then increment (and
 * wrap is needed) the rear. If the buffer is full it will overwrite the data
 * at the front of the buffer and increment the front.
 * 
 * Buffer_Init() must be used to initialize the buffer prior to calling Buffer_Enqueue and
 * passing it a pointer to the buffer.
 * 
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @param data char data to be added to the rear of the FIFO buffer
 * 
 * @warning Buffer_Enqueue is only interrupt safe if EnableInterrupts() and DisableInterrupts()
 * are defined by hal_general.h
 */
void Buffer_Enqueue(buffer_t *buffer, uint8_t data);

/** Buffer_Dequeue will return one item from the front of the FIFO buffer
 * 
 * Buffer_Dequeue will return the item at the front of the FIFO buffer then increment (and
 * wrap as needed) the front. If the buffer is empty it will return 0.
 * 
 * Buffer_Init() must be used to initialize the buffer prior to calling Buffer_Dequeue and
 * passing it a pointer to the buffer.
 * 
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @return Data of type char from the front of the buffer
 * 
 * @warning is only interrupt safe if EnableInterrupts() and DisableInterrupts()
 * are defined by hal_general.h
 */
uint8_t Buffer_Dequeue(buffer_t *buffer);

/** Buffer_GetSize returns the number of items in the FIFO buffer
 * 
 * Buffer_Init() should be used to initialize the buffer otherwise the return
 * value will be meaningless
 * 
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @return Number of items in the buffer
 */
uint16_t Buffer_GetSize(buffer_t *buffer);

/** Helpful macros for allocating and initializing buffer module
 *
 *  BUFFER_ALLOCATE uses a name to declare the array and buffer structure
 * 
 *  BUFFER_INIT uses the name given in BUFFER_ALLOCATE to initialize the 
 *  buffer structure
 */
#define BUFFER_ALLOCATE(buffer_name,  max_size)\
    char CAT2(buffer_name, _array)[max_size];\
    buffer_t buffer_name

#define BUFFER_INIT(buffer_name, max_size)\
    Buffer_Init(&buffer_name, &CAT2(buffer_name, _array)[0], max_size)

/** Initialize a FIFO buffer
 * 
 * Example code:
 * @code
 * #define TX_BUFFER_LENGTH 512
 * buffer_t tx_buffer; // transmit buffer
 * uint8_t tx_buffer_array[TX_BUFFER_LENGTH]
 * ...
 * Buffer_Init(&tx_buffer, &tx_buffer_array[0], TX_BUFFER_LENGTH);
 * // or
 * BUFFER_ALLOCATE(tx_buffer, TX_BUFFER_LENGTH);
 * ...
 * BUFFER_INIT(tx_buffer, TX_BUFFER_LENGTH);
 * @endcode
 * 
 * @param buffer Pointer to the buffer_t data structure to be initialized
 * @param data_array Array of char data to implement the actual buffer
 * @param max_size Maximum size of the buffer (should be the same length as the
 * array)
 */
void Buffer_Init(buffer_t *buffer, uint8_t *data_array, uint16_t max_size);

/** Set Callback function for buffer to be called after items are Queued to the buffer
 *
 * The callback function will be called after anything is Queued to
 * the buffer. The function will be called with a pointer to the buffer which had a byte queued
 * onto it.
 *
 * Example:
 * @code
 * void TxCallback(buffer_t * buf);
 * #define TX_BUFFER_LENGTH 512
 * buffer_t tx; // transmit buffer
 * char tx_buffer_array[TX_BUFFER_LENGTH]
 * ...
 * Buffer_Init(&tx, &tx_buffer_array[0], TX_BUFFER_LENGTH);
 * Buffer_SetCallback(&tx, TxCallback);
 * ...
 * void TxCallback(buffer_t * buf) {
 * 		SET_UART_TX_IE();
 * }
 * @endcode
 * This example is useful for a uC which has a hardware Tx interrupt flag which is set
 * whenever there is room in the hardware Tx FIFO buffer. When done transmitting the
 * interrupt must be disabled to avoid getting stuck in the ISR. When data needs to be
 * sent the interrupt must be enabled again, thus the need for the callback.
 *
 * Another usage could be to handle received data on a receive buffer.
 *
 * @warning many applications may use the Enqueue method in a ISR which means the Callback
 * would be called from a ISR. Thus care should be taken to ensure callbacks are both
 * fast and interrupt safe
 *
 * @param buffer Pointer to the buffer_t data structure whose callback function is to be set
 * @param Callback Function pointer to a callback function with no return value  and a
 * 	buffer_t pointer input.
 */
void Buffer_SetCallback(buffer_t * buffer, void (*Callback)(buffer_t * buffer));

/** Clear/remove the callback function for 'buffer'
 *
 * @param buffer Pointer to the buffer_t data structure whose callback function is to be cleared
 */
void Buffer_ClearCallback(buffer_t * buffer);

/** Queue a array of data to the buffer
 *
 * @warning Buffer_EnqueueData will disable interrupts while writing to the buffer. If
 * length is long this could interfere with time sensitive ISRs. Consider using
 * Buffer_Enqueue() if this is an issue.
 *
 * Buffer_EnqueueData will add an array of items to the rear of the data buffer and increment
 * (and wrap if needed) the rear. If the buffer does not have room none of the data
 * will be queued to the buffer.
 *
 * Buffer_Init() must be used to initialize the buffer prior to calling Buffer_EnqueueData and
 * passing it a pointer to the buffer.
 *
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @param data byte pointer to data array to be added to the rear of the FIFO buffer
 * @param length the number of items in the data array
 * @return 0 if succeeded, 1 if not enough room in buffer
 */
uint8_t Buffer_EnqueueData(buffer_t * buffer, uint8_t * data, uint16_t length);

#define BUFFER_ENQUEUE_FAILED 1 ///< enqueue failed (return value of Buffer_EnqueueData() )
#define BUFFER_ENQUEUE_SUCCEEDED 0 ///< enqueue succeeded (return value of Buffer_EnqueueData() )

/** @} */

#endif // _BUFFER_H_
