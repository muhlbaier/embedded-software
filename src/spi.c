#include <string.h>

#include "spi.h"
#include "list.h"
#include "task.h"
#include "subsystem.h"

#ifdef USE_SPI5
#define NUM_SPI 6
#elif defined USE_SPI4
#define NUM_SPI 5
#elif defined USE_SPI3
#define NUM_SPI 4
#elif defined USE_SPI2
#define NUM_SPI 3
#elif defined USE_SPI1
#define NUM_SPI 2
#elif defined USE_SPI0
#define NUM_SPI 1
#else
#warning No SPI channels used, remove this file from project
#define NUM_SPI 0
#endif

#ifdef USE_SPI5
#ifndef SPI5_MAX_TRANSACTIONS
#define SPI5_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray5[SPI5_MAX_TRANSACTIONS];
list_link_t linkArray5[SPI5_MAX_TRANSACTIONS];
#endif
#ifdef USE_SPI4
#ifndef SPI4_MAX_TRANSACTIONS
#define SPI4_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray4[SPI4_MAX_TRANSACTIONS];
list_link_t linkArray4[SPI4_MAX_TRANSACTIONS];
#endif
#ifdef USE_SPI3
#ifndef SPI3_MAX_TRANSACTIONS
#define SPI3_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray3[SPI3_MAX_TRANSACTIONS];
list_link_t linkArray3[SPI3_MAX_TRANSACTIONS];
#endif
#ifdef USE_SPI2
#ifndef SPI2_MAX_TRANSACTIONS
#define SPI2_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray2[SPI2_MAX_TRANSACTIONS];
list_link_t linkArray2[SPI2_MAX_TRANSACTIONS];
#endif
#ifdef USE_SPI1
#ifndef SPI1_MAX_TRANSACTIONS
#define SPI1_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray1[SPI1_MAX_TRANSACTIONS];
list_link_t linkArray1[SPI1_MAX_TRANSACTIONS];
#endif
#ifdef USE_SPI0
#ifndef SPI0_MAX_TRANSACTIONS
#define SPI0_MAX_TRANSACTIONS SPI_MAX_TRANSACTIONS
#endif
spi_transaction_t transactionArray0[SPI0_MAX_TRANSACTIONS];
list_link_t linkArray0[SPI0_MAX_TRANSACTIONS];
#endif


typedef enum {
    SPI_IDLE = 0,
    SPI_BUSY
} spi_status_t;

/// spi module structure

struct spi {
    spi_status_t state; ///< state
    spi_transaction_t* currentTransaction_ptr; ///< current transaction pointer
    list_t transactionList; ///< transaction list
    uint8_t bytesToWrite; ///< the number of bytes to write from the buffer
    uint8_t blankBytesToWrite; ///< the number of dummy blank bytes to write so reading can occur
    uint8_t bytesToReceive; ///< the total number of bytes left to be received from SPI, not neccesarily used
    uint8_t bytesRead; ///< the number of bytes read into the data buffer
    uint8_t bytesToDelay; ///< the number of bytes to delay when receiving data before reading the bytes to the data buffer
} spi[NUM_SPI];

/****************************************
 * Function Declarations
 ***************************************/
static void CheckTransactions(uint8_t channel);
static void HandleCallback(spi_transaction_t* transaction_ptr);
static void FinishTransaction(spi_transaction_t* transaction);
void SPI_Tx_Handler(uint8_t channel);
void SPI_Rx_Handler(uint8_t channel);

void SPI_Init(spi_settings_t* spi_settings) {
    hal_SPI_Init(spi_settings);
    spi[spi_settings->channel].state = SPI_IDLE;
    switch (spi_settings->channel) {
#ifdef USE_SPI0
    case 0:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray0, SPI0_MAX_TRANSACTIONS, linkArray0);
        break;
#endif
#ifdef USE_SPI1
    case 1:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray1, SPI1_MAX_TRANSACTIONS, linkArray1);
        break;
#endif
#ifdef USE_SPI2
    case 2:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray2, SPI2_MAX_TRANSACTIONS, linkArray2);
        break;
#endif
#ifdef USE_SPI3
    case 3:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray3, SPI3_MAX_TRANSACTIONS, linkArray3);
        break;
#endif
#ifdef USE_SPI4
    case 4:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray4, SPI4_MAX_TRANSACTIONS, linkArray4);
        break;
#endif
#ifdef USE_SPI5
    case 5:
        //Setup the list to be used for transactions
        List_Init(&spi[spi_settings->channel].transactionList, sizeof (spi_transaction_t), (uint16_t *) transactionArray5, SPI5_MAX_TRANSACTIONS, linkArray5);
        break;
#endif
    }
}

int8_t SPI_Transact(spi_transaction_t * transaction) {
    spi_transaction_t* transaction_ptr;
    uint32_t time;
    uint8_t ch;
    ch = transaction->flags.channel;

    // Warn the user if they are probably accidentally overwriting data
    if((transaction->writeLength + transaction->readLength) > SPI_MAX_SIZE){
        LogWarning(0x00, "SPI transaction has a longer length (read + write) than the max transaction size");
        return -1;
    }

    transaction->flags.finished = 0;
    // Get a spot in the list to load the transaction
    transaction_ptr = List_AddIndirect(&spi[ch].transactionList);
    // if a null pointer is returned then abort
    if(transaction_ptr == 0) {
        LogWarning(0x00, "no room in list, ch: %d", ch);
        return -2;
    }

    // only worry about copying the data to write, the rest will be junk
    memcpy(transaction_ptr, transaction, (4 + 2*sizeof( void(*)(void) ) + transaction->writeLength) );
    // link the item in the list.
    List_LinkItem(&spi[ch].transactionList, transaction_ptr);

    //Check to see if the SPI peripheral is available to transmit
    CheckTransactions(ch);

    // if the transaction is blocking, wait for it to be finished
    if (transaction_ptr->flags.blocking) {
    	time = TimeNow();
        while (!transaction_ptr->flags.finished) {
        	if(TimeSince(time) > 10) {
        		break;
        	}
        }
        // Since this is a blocking call, update the transaction's data
        memcpy(transaction->data, transaction_ptr->data, transaction_ptr->writeLength + transaction_ptr->readLength);
        FinishTransaction(transaction_ptr);
    }
    return 0;
}

static void CheckTransactions(uint8_t channel) {
    spi_transaction_t * transaction;

    //If the SPI peripheral is not actively in a transaction
    if (spi[channel].state == SPI_IDLE) {
        // Immediately set the spi state to busy so another transaction can't get into here
        spi[channel].state = SPI_BUSY;

        // Get the next transaction off the front of the list
        transaction = List_GetFirst(&spi[channel].transactionList);
        spi[channel].currentTransaction_ptr = transaction;

        //If the transaction pointer is null, return
        if(transaction == 0){
            // Set the state back to IDLE
            spi[channel].state = SPI_IDLE;
            return;
        }

        // Setup the SPI structure with the required information about the transaction
        spi[channel].bytesToWrite = transaction->writeLength;
        if(transaction->readLength == 0){
            transaction->readDelay = 0;
        }
        if(transaction->readLength + transaction->readDelay > transaction->writeLength){
            spi[channel].blankBytesToWrite = transaction->readLength + transaction->readDelay - transaction->writeLength;
        } else {
            spi[channel].blankBytesToWrite = 0;
        }
        spi[channel].bytesRead = 0;
        spi[channel].bytesToDelay = transaction->readDelay;
        spi[channel].bytesToReceive = spi[channel].bytesToWrite + spi[channel].blankBytesToWrite;

        // Lower the CS if there is a provided method
        if(transaction->cs_control){
            transaction->cs_control(0);
        }

        while (hal_SPI_SpaceAvailable(channel)) {
            // This should never be able to happen the first time this loop runs, but if there is a FIFO,
            // and we push the last byte, but there is still room available, we should simply break.    The
            // transaction will be finished AFTER the correct amount of bytes are received
            if(spi[channel].bytesToWrite == 0 && spi[channel].blankBytesToWrite == 0) break;

            if (spi[channel].bytesToWrite) {
                hal_SPI_TxByte(channel, transaction->data[transaction->writeLength - spi[channel].bytesToWrite--]);
            } else if (spi[channel].blankBytesToWrite) {
                spi[channel].blankBytesToWrite--;
                hal_SPI_TxByte(channel, 0xFF);
            }
        }

        hal_SPI_EnableTxInterrupt(channel);
    }
}

static void HandleCallback(spi_transaction_t* transaction_ptr) {
    // Call the callback
    transaction_ptr->callback(transaction_ptr);
    // If it is not a blocking call, remove the callback from the list
    List_Remove(&spi[transaction_ptr->flags.channel].transactionList, transaction_ptr);
}

static void FinishTransaction(spi_transaction_t* transaction) {
    uint8_t ch;
    ch = transaction->flags.channel;
    //The incoming transaction is also the first transaction on the list

    // Disable chip select if there is a method
    if(transaction->cs_control){
        transaction->cs_control(1);
    }
    if (transaction->flags.blocking) {
        List_RemoveFirst(&spi[ch].transactionList);
    } else if (transaction->callback != 0) {
        // If there is a callback, unlink it from the transaction list and add it to the callback buffer
        List_UnlinkFirst(&spi[ch].transactionList);
        transaction->flags.finished = 1;
        Task_Queue((task_t) HandleCallback, transaction);
    } else {
        //If there is no callback and this is not a blocking call, then remove it
        List_RemoveFirst(&spi[ch].transactionList);
    }

    //Disable the interrupts so that if there isn't another transaction we keep interrupts off
    //but if there is another transaction, CheckTransactions will take care of enabling interrupts
    hal_SPI_DisableTxInterrupt(ch);
    spi[ch].state = SPI_IDLE;
    CheckTransactions(ch);
}

void SPI_Rx_Handler(uint8_t channel) {
	uint8_t c;
    // If the interrupt flag is not set, return
    if(!hal_SPI_RxIntStatus(channel)) return;

    // For all data in the buffer
    while(hal_SPI_DataAvailable(channel)) {
        // Get the current transaction
        spi_transaction_t * transaction;
        transaction = spi[channel].currentTransaction_ptr;

        if(hal_SPI_OverrunError(channel)) {
            transaction->flags.overrun = 1;
            // force bytes to receive to 1 so it will abort the transaction
            spi[channel].bytesToReceive = 1;
		}
		c = hal_SPI_RxByte(channel);

        // If the current transaction is NULL, return
        if(transaction == 0) {
            hal_SPI_ClearRxIF(channel);
            return;
        }

        // If the transaction cares about the data, grab it, if not, just do a dummy read
        if(transaction->readLength){
            if(spi[channel].bytesToDelay == 0){
                //This is data we care about
                transaction->data[transaction->writeLength + spi[channel].bytesRead++] = c;
            } else {
                spi[channel].bytesToDelay--;
            }
        }

        // If we have received the last byte, end the transaction
        if(--spi[channel].bytesToReceive == 0){
            if(transaction->flags.blocking){
                transaction->flags.finished = 1;
            } else {
                Task_Queue((task_t) FinishTransaction, transaction);
            }
            break;
        }
    }
    hal_SPI_ClearRxIF(channel);
}

void SPI_ISR(uint8_t channel){
    SPI_Tx_Handler(channel);
    SPI_Rx_Handler(channel);
}

void SPI_Tx_Handler(uint8_t channel) {

    // If the interrupt flag is not set, return
    if(!hal_SPI_TxIntStatus(channel)) return;

    spi_transaction_t * transaction;
    transaction = spi[channel].currentTransaction_ptr;

    if(transaction == 0) {
        hal_SPI_ClearTxIF(channel);
        return;
    }

    // If there is nothing left to transmit, disable the interrupt
    if(spi[channel].bytesToWrite == 0 && spi[channel].blankBytesToWrite == 0){
        hal_SPI_DisableTxInterrupt(channel);
        return;
    }

    // If we've gotten here, there is at least one byte to send
    while (hal_SPI_SpaceAvailable(channel)) {
        // This should never be able to happen the first time this loop runs, but if there is a FIFO,
        // and we push the last byte, but there is still room available, we should simply return.   The
        // transaction will be finished AFTER we know that the data has left the buffer
        if(spi[channel].bytesToWrite == 0 && spi[channel].blankBytesToWrite == 0) break;

        if (spi[channel].bytesToWrite) {
            hal_SPI_TxByte(channel, transaction->data[transaction->writeLength - spi[channel].bytesToWrite--]);
        } else if (spi[channel].blankBytesToWrite) {
            spi[channel].blankBytesToWrite--;
            hal_SPI_TxByte(channel, 0xFF);
        }
    }

    hal_SPI_ClearTxIF(channel);
}
