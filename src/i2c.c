#include "i2c.h"
#include "hal_i2c.h"
#include "list.h"
#include "task.h"

#ifdef USE_I2C5
#define NUM_I2C 6
#elif defined USE_I2C4
#define NUM_I2C 5
#elif defined USE_I2C3
#define NUM_I2C 4
#elif defined USE_I2C2
#define NUM_I2C 3
#elif defined USE_I2C1
#define NUM_I2C 2
#elif defined USE_I2C0
#define NUM_I2C 1
#else
#warning No I2C channels used, remove this file from project
#define NUM_I2C 0
#endif

#ifdef USE_I2C5
#ifndef I2C5_MAX_TRANSACTIONS
#define I2C5_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray5[I2C5_MAX_TRANSACTIONS];
list_link_t linkArray5[I2C5_MAX_TRANSACTIONS];
#endif
#ifdef USE_I2C4
#ifndef I2C4_MAX_TRANSACTIONS
#define I2C4_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray4[I2C4_MAX_TRANSACTIONS];
list_link_t linkArray4[I2C4_MAX_TRANSACTIONS];
#endif
#ifdef USE_I2C3
#ifndef I2C3_MAX_TRANSACTIONS
#define I2C3_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray3[I2C3_MAX_TRANSACTIONS];
list_link_t linkArray3[I2C3_MAX_TRANSACTIONS];
#endif
#ifdef USE_I2C2
#ifndef I2C2_MAX_TRANSACTIONS
#define I2C2_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray2[I2C2_MAX_TRANSACTIONS];
list_link_t linkArray2[I2C2_MAX_TRANSACTIONS];
#endif
#ifdef USE_I2C1
#ifndef I2C1_MAX_TRANSACTIONS
#define I2C1_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray1[I2C1_MAX_TRANSACTIONS];
list_link_t linkArray1[I2C1_MAX_TRANSACTIONS];
#endif
#ifdef USE_I2C0
#ifndef I2C0_MAX_TRANSACTIONS
#define I2C0_MAX_TRANSACTIONS I2C_MAX_TRANSACTIONS
#endif
i2c_transaction_t transactionArray0[I2C0_MAX_TRANSACTIONS];
list_link_t linkArray0[I2C0_MAX_TRANSACTIONS];
#endif

typedef enum {
    I2C_IDLE = 0,
    I2C_BUSY
} i2c_status_t;

struct i2c{
    i2c_status_t state;
    i2c_transaction_t* currentTransaction_ptr;
    list_t transactionList;
    struct{
        uint8_t bytesToWrite : 4;
        uint8_t bytesToRead : 4;
    };
} i2c[NUM_I2C];

/****************************************
 * Function Declarations
 ***************************************/
static void CheckTransactions(uint8_t channel);
static void HandleCallback(i2c_transaction_t* transaction_ptr);
static void FinishTransaction(i2c_transaction_t* transaction);

void I2C_Init(i2c_settings_t* i2c_settings){
    hal_I2C_Init(i2c_settings);

    switch(i2c_settings->channel){
#ifdef USE_I2C0
    case 0:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray0, I2C0_MAX_TRANSACTIONS, linkArray0);
        break;
#endif
#ifdef USE_I2C1
    case 1:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray1, I2C1_MAX_TRANSACTIONS, linkArray1);
        break;
#endif
#ifdef USE_I2C2
    case 2:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray2, I2C2_MAX_TRANSACTIONS, linkArray2);
        break;
#endif
#ifdef USE_I2C3
    case 3:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray3, I2C3_MAX_TRANSACTIONS, linkArray3);
        break;
#endif
#ifdef USE_I2C4
    case 4:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray4, I2C4_MAX_TRANSACTIONS, linkArray4);
        break;
#endif
#ifdef USE_I2C5
    case 5:
        //Setup the list to be used for transactions
        List_Init(&i2c[i2c_settings->channel].transactionList, sizeof(i2c_transaction_t), (uint16_t *)transactionArray5, I2C5_MAX_TRANSACTIONS, linkArray5);
        break;
#endif
    }
}

void I2C_Transact(i2c_transaction_t* transaction){
    i2c_transaction_t* transaction_list_ptr;
    transaction->finished = 0;
    //Add the transaction to the list and link it
    transaction_list_ptr = (i2c_transaction_t*)List_AddAndLink(&i2c[transaction->channel].transactionList, (void *)transaction);
    //Check to see if the I2C peripheral is available to transmit
    CheckTransactions(transaction->channel);

    // if the transaction is blocking, wait for it to be finished
    if(transaction_list_ptr->blocking){
        while(!transaction_list_ptr->finished){
            SystemTick();
        }
        // Copy the transaction from the list to the original transaction
        *transaction = *transaction_list_ptr;
        List_RemoveFirst(&i2c[transaction->channel].transactionList);
        i2c[transaction->channel].state = I2C_IDLE;
        CheckTransactions(transaction->channel);
    }
}

static void CheckTransactions(uint8_t channel){
    //If the I2C peripheral is not actively in a transaction
    if(i2c[channel].state == I2C_IDLE){
        // Get the next transaction off the front of the list
        i2c[channel].currentTransaction_ptr = List_GetFirst(&i2c[channel].transactionList);
        //If the transaction pointer is not null then process it
        if(i2c[channel].currentTransaction_ptr){
            i2c[channel].bytesToWrite = i2c[channel].currentTransaction_ptr->writeLength;
            i2c[channel].bytesToRead = i2c[channel].currentTransaction_ptr->readLength;
            i2c[channel].state = I2C_BUSY;
            hal_I2C_SendStart(channel);
        }
    }
}

static void HandleCallback(i2c_transaction_t* transaction_ptr){
    // Call the callback
    transaction_ptr->callback(*transaction_ptr);
    // If it is not a blocking call, remove the callback from the list
    List_Remove(&i2c[transaction_ptr->channel].transactionList, transaction_ptr);
}

static void FinishTransaction(i2c_transaction_t* transaction){
    //The incoming transaction is also the first transaction on the list

    if(transaction->blocking){
        // If this is a blocking call, there is no callback
        transaction->finished = 1;
        return;
    } else if(transaction->callback != 0){
        // If there is a callback, unlink it from the transaction list and add it to the callback buffer
        List_UnlinkFirst(&i2c[transaction->channel].transactionList);
        transaction->finished = 1;
        Task_Queue(HandleCallback, transaction);
    } else {
        //If there is no callback and this is not a blocking call, then remove it
        List_RemoveFirst(&i2c[transaction->channel].transactionList);
    }

    i2c[transaction->channel].state = I2C_IDLE;
    CheckTransactions(transaction->channel);
}

uint16_t I2C_GetSlaveAddress(uint8_t channel){
    return i2c[channel].currentTransaction_ptr->slave_address;
}

void I2C_Rx_Handler(uint8_t channel, uint8_t byte){
    i2c[channel].currentTransaction_ptr->readData[i2c[channel].currentTransaction_ptr->readLength - i2c[channel].bytesToRead--] = byte;
}

uint8_t I2C_GetTxByte(uint8_t channel){
    return i2c[channel].currentTransaction_ptr->writeData[i2c[channel].currentTransaction_ptr->writeLength - i2c[channel].bytesToWrite--];
}

uint8_t I2C_GetTxSize(uint8_t channel){
    return i2c[channel].bytesToWrite;
}

uint8_t I2C_GetRxSize(uint8_t channel){
    return i2c[channel].bytesToRead;
}

void I2C_TransactionSuccess(uint8_t channel){
    i2c[channel].currentTransaction_ptr->error = TRANSACTION_SUCCESSFUL;
    // Queue task to finish task
    Task_Queue(FinishTransaction, i2c[channel].currentTransaction_ptr);
}

void I2C_TransactionFail(uint8_t channel){
    i2c[channel].state = I2C_IDLE;
    if(--i2c[channel].currentTransaction_ptr->retryCount){
        if(i2c[channel].currentTransaction_ptr->blocking){
            i2c[channel].bytesToWrite = i2c[channel].currentTransaction_ptr->writeLength;
            i2c[channel].bytesToRead = i2c[channel].currentTransaction_ptr->readLength;
            i2c[channel].state = I2C_BUSY;
            hal_I2C_SendStart(channel);
        } else {
            List_ResortFirst(&i2c[channel].transactionList);
            CheckTransactions(channel);
        }
    } else {
        i2c[channel].currentTransaction_ptr->error = TRANSACTION_FAILED;
        Task_Queue(FinishTransaction, i2c[channel].currentTransaction_ptr);
    }
}
