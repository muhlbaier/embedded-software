/*
 * accelerometer.c
 *
 *  Created on: Mar 25, 2019
 *      Author: Stephen Glass
 */

#include <stdint.h>
#include "accelerometer.h"
#include "project_settings.h"
#include "task.h"

/* should be defined project_settings */
#define I2C_MAX_RX_SIZE 6
#define I2C_MAX_RX_SIZE 6
/* ----- */

#include "i2c.h"
#include "hal_general.h"

#define GSCALE          2
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low
#define WHO_AM_I        0x0D
#define DEVICE_FOUND    0x2A
#define CTRL_REG1       0x2A
#define XYZ_DATA_CFG    0x0E

struct accelerometer_data {
    uint16_t raw_XYZ[3];
    float g_XYZ[3];
    void(*callback)(uint16_t x, uint16_t y, uint16_t z);
};
static struct accelerometer_data accel_data;

static void ReadAccelerometer(void);

void Accelerometer_Init(uint16_t period, void(*callback)(uint16_t x, uint16_t y, uint16_t z)) {
    /// Initialize I2C communication with accelerometer
    i2c_settings_t * settings;
    settings->channel = I2C_B0;
    settings->bit_rate = 100000; //100kHz bitrate
    settings->address_length = 0b1;
    I2C_Init(settings);

    i2c_transaction_t * transaction;
    /* Check if device is online */
    transaction->slave_address = MMA8452_ADDRESS; // address of accelerometer
    transaction->writeData[0] = WHO_AM_I; // Read address 0x0D
    transaction->writeLength = 0b0001; // 1 bit
    transaction->readLength = 0b0001; // 1 bit
    transaction->blocking = 0b1; // blocking
    I2C_Transact(transaction);
    if(transaction->readData[0] == 0x2A) {
        // device is online
    }
    else {
        // could not connect
        while(1); // failure terminate
    }

    /* Put in standby mode */
    transaction->writeData[0] = CTRL_REG1;
    I2C_Transact(transaction);
    transaction->readLength = 0b0000;
    transaction->writeLength = 0b0010; // write 2 bits
    transaction->writeData[0] = CTRL_REG1;
    transaction->writeData[1] = transaction->readData[0] & ~(0x01);
    I2C_Transact(transaction);

    /* Set up the full scale range to 2, 4, or 8g */
    uint8_t fsr = GSCALE;
    if(fsr > 8) fsr = 8; // error check
    fsr >>= 2;
    transaction->writeLength = 0b0010; // write 2 bit
    transaction->writeData[0] = XYZ_DATA_CFG;
    transaction->writeData[1] = fsr;
    I2C_Transact(transaction);

    /* Put into active mode */
    transaction->readLength = 0b0001;
    transaction->writeLength = 0b0001;
    transaction->writeData[0] = CTRL_REG1;
    I2C_Transact(transaction);
    transaction->readLength = 0b0000;
    transaction->writeLength = 0b0010; // write 2 bits
    transaction->writeData[0] = CTRL_REG1;
    transaction->writeData[1] = transaction->readData[0] | (0x01);
    I2C_Transact(transaction);

   /* Assign callback pointer to data structure */
   accel_data.callback = callback;

   /* Schedule task to read data */
   Task_Schedule((task_t)ReadAccelerometer, 0, period, period);
}

void Accelerometer_Deinit() {
    // remove the reading task
    Task_Remove((task_t)ReadAccelerometer, 0);
}

static void ReadAccelerometer(void) {
    i2c_transaction_t * transaction;
    transaction->slave_address = MMA8452_ADDRESS; // address of accelerometer
    transaction->writeData[0] = 0x01; // Read address OUT_X_MSG [0x01]
    transaction->writeLength = 0b0001; // 1 bit
    transaction->readLength = 0b0110; // 6 length
    transaction->blocking = 0b1; // blocking
    I2C_Transact(transaction); // perform the transaction

    /* Fetching raw data from sensor */
    volatile uint8_t i;
    for(i = 0; i < 3; i++) {
        // combine two 8 bit registers into one 12-bit number
        uint16_t gCount = (transaction->readData[i*2] << 8) | transaction->readData[(i*2)+1];
        gCount >>= 4; // registers left align, so we right aligng 12-bit int

        // if number is negative, we make it manually (no 12-bit data type)
        if(transaction->readData[i*2] > 0x7F) {
            gCount -= 0x1000;
        }
        accel_data.raw_XYZ[i] = gCount;
    }
    /* Calculating acceleration value into g's */
    for(i = 0; i < 3; i++) {
        // calculate g value dependent on the scale
        accel_data.g_XYZ[i] = (float)accel_data.raw_XYZ[i] / ((1<<12)/(2*GSCALE));
    }
    // call the callback with our newfound data
    accel_data.callback(accel_data.g_XYZ[0], accel_data.g_XYZ[1], accel_data.g_XYZ[2]);
}