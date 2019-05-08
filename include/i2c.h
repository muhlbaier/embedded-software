/*
 * File:   i2c.h
 * Author: Anthony Merlino
 *
 * Created on February 24, 2015, 11:03 PM
 *
 * Edited by: Nicholas Klein
 * Edited on March 25, 2019
 *
 * Edited by: Stephen Glass
 * Edited on April 12, 2019
 * Added functionality to keep connection active after transaction
 * Useful if you want to send repeated start condition for next transaction
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "project_settings.h"
#include "hal_i2c.h"

#ifndef I2C_MAX_TX_SIZE
#define I2C_MAX_TX_SIZE 4
#endif

#ifndef I2C_MAX_RX_SIZE
#define I2C_MAX_RX_SIZE 4
#endif

#ifndef I2C_MAX_TRANSACTIONS
#define I2C_MAX_TRANSACTIONS 4
#endif

struct i2c_transaction {
    uint8_t writeData[I2C_MAX_TX_SIZE];
    uint8_t readData[I2C_MAX_RX_SIZE];
    uint16_t slave_address;
    struct {
        uint16_t writeLength : 4;
        uint16_t readLength : 4;
        uint16_t channel : 3;
        uint16_t retryCount : 3;
        uint16_t blocking : 1;
        uint16_t finished : 1;
        uint16_t error : 1;
        uint16_t stayActive : 1; // doesn't send stop bit after transmitting
        // enable if you want repeated-start signal SR on next transaction
    };
    void (*callback)(struct i2c_transaction);
};

typedef struct i2c_transaction i2c_transaction_t;

#define TRANSACTION_FAILED 1
#define TRANSACTION_SUCCESSFUL 0

void I2C_Init(i2c_settings_t* i2c_settings);
void I2C_Transact(i2c_transaction_t* transaction);

uint8_t I2C_Tx_Handler(uint8_t channel);
void I2C_Rx_Handler(uint8_t channel, uint8_t byte);

/******************************************
 * HAL Support Functions
 *****************************************/
uint16_t I2C_GetSlaveAddress(uint8_t channel);
uint8_t I2C_GetTxByte(uint8_t channel);
uint8_t I2C_GetTxSize(uint8_t channel);
uint8_t I2C_GetRxSize(uint8_t channel);
uint8_t I2C_GetStayActive(uint8_t channel);
void I2C_TransactionSuccess(uint8_t channel);
void I2C_TransactionFail(uint8_t channel);

/******************************************
 * HAL Functions
 *****************************************/

#ifndef hal_I2C_Init
void hal_I2C_Init(i2c_settings_t* settings);
#endif

#ifndef hal_I2C_Enable
void hal_I2C_Enable(uint8_t channel);
#endif

#ifndef hal_I2C_Disable
void hal_I2C_Disable(uint8_t channel);
#endif

#ifndef hal_I2C_SendStart
void hal_I2C_SendStart(uint8_t channel);
#endif

#ifndef hal_I2C_EnableInterrupts
void hal_I2C_EnableInterrupts(uint8_t channel);
#endif

#ifndef hal_I2C_DisableInterrupts
void hal_I2C_DisableInterrupts(uint8_t channel);
#endif

#endif // _I2C_H_
