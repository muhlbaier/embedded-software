/*
 * accelerometer.c
 *
 *  Created on: Mar 25, 2019
 *  Authors: Stephen Glass, Lonnie Souder
 */

#include <stdint.h>
#include "accelerometer.h"
#include "project_settings.h"
#include "task.h"

/* following macros should be defined project_settings
#define I2C_MAX_TX_SIZE 2
#define I2C_MAX_RX_SIZE 6 */

#include "i2c.h"
#include "hal_general.h"

#define GSCALE          2
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low
#define WHO_AM_I        0x0D
#define DEVICE_FOUND    0x2A
#define CTRL_REG1       0x2A
#define XYZ_DATA_CFG    0x0E
#define OUT_X_MSB       0x01

#define I2C_CLOCK_SPEED 100000

struct accelerometer_data {
    int16_t raw_XYZ[3];
    float g_XYZ[3];
    void(*callback)(float x, float y, float z);
};
static struct accelerometer_data accel_data;

static void ReadAccelerometer(void);

int8_t Accelerometer_Init(uint16_t period, void(*callback)(float x, float y, float z)) {

    if(I2C_MAX_TX_SIZE < 2) {
        return -1; // transmit buffer not long enough
    }
    else if(I2C_MAX_RX_SIZE < 6) {
        return -1; // receive buffer not long enough
    }

    /// Initialize I2C communication with accelerometer
    i2c_settings_t settings;
    settings.channel = I2C_B0;
    settings.bit_rate = I2C_CLOCK_SPEED; //100kHz bitrate
    settings.address_length = 0; // Using 7-bit slave address
    I2C_Init(&settings);

    /// Configure base transaction configuration
    i2c_transaction_t configureTransaction;
    configureTransaction.slave_address = MMA8452_ADDRESS;
    configureTransaction.blocking = 1; // Transactions are blocking
    configureTransaction.channel = I2C_B0; // Same channel as initialization

    /// Start read of WHO_AM_I register to check if device is online
    /// We write the register we want to read in first transaction, while keeping connection active
    /// The next transaction we read the value from the previous register and terminate the connection
    configureTransaction.writeData[0] = WHO_AM_I;
    configureTransaction.writeLength = 1;
    configureTransaction.readLength = 0;
    configureTransaction.stayActive = 1;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeLength = 0;
    configureTransaction.readLength = 1;
    configureTransaction.stayActive = 0;
    I2C_Transact(&configureTransaction);
    /// Reading WHO_AM_I always returns DEVICE_FOUND
    if(configureTransaction.readData[0] != DEVICE_FOUND) {
        // device is not online
        return -1; // error
    }

    /// Put into STANDBY mode
    /// Registers can only be changed on accelerometer while in STANDBY mode
    /// Read the CTRL_REG1 value and then write the negated value to CTRL_REG1
    configureTransaction.writeData[0] = CTRL_REG1;
    configureTransaction.writeLength = 1;
    configureTransaction.readLength = 0;
    configureTransaction.stayActive = 1;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeLength = 0;
    configureTransaction.readLength = 1;
    configureTransaction.stayActive = 0;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeData[0] = CTRL_REG1;
    configureTransaction.writeData[1] = configureTransaction.readData[0] & ~(0x01);
    configureTransaction.writeLength = 2;
    configureTransaction.readLength = 0;
    I2C_Transact(&configureTransaction);


    /// Configure scale
    configureTransaction.writeData[0] = XYZ_DATA_CFG;
    configureTransaction.writeLength = 1;
    configureTransaction.readLength = 0;
    configureTransaction.stayActive = 1;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeLength = 0;
    configureTransaction.readLength = 1;
    configureTransaction.stayActive = 0;
    I2C_Transact(&configureTransaction); // Now we have the data in read buffer
    configureTransaction.writeData[0] = XYZ_DATA_CFG;
    configureTransaction.readData[0] &= 0xFC; // mask out scale bits
    configureTransaction.readData[0] |= (GSCALE >> 2); // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
    configureTransaction.writeData[1] = configureTransaction.readData[0];
    configureTransaction.writeLength = 2;
    configureTransaction.readLength = 0;
    I2C_Transact(&configureTransaction);

    /// Put into ACTIVE mode
    /// Read the CTRL_REG1 value and then write the enabled value to CTRL_REG1
    configureTransaction.writeData[0] = CTRL_REG1;
    configureTransaction.writeLength = 1;
    configureTransaction.readLength = 0;
    configureTransaction.stayActive = 1;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeLength = 0;
    configureTransaction.readLength = 1;
    configureTransaction.stayActive = 0;
    I2C_Transact(&configureTransaction);
    configureTransaction.writeData[0] = CTRL_REG1;
    configureTransaction.writeData[1] = configureTransaction.readData[0] | (0x01);
    configureTransaction.writeLength = 2;
    configureTransaction.readLength = 0;
    I2C_Transact(&configureTransaction);

    /// Assign callback pointer to data structure
    accel_data.callback = callback;

    /// Schedule task to read data
    Task_Schedule((task_t)ReadAccelerometer, 0, period, period);
    return 1;
}

void Accelerometer_Deinit() {
    // remove the reading task
    Task_Remove((task_t)ReadAccelerometer, 0);
}


/*
 * This function will read the acceleration values from the MMA8452Q. After
 * reading, it will update two triplets of variables:
 * int's raw_XYZ will store the signed 12-bit values read out of the acceleromter.
 * floats g_XYZ will store the calculated acceleration from those 12-bit values. These variables are in units of g's.
 */
static void ReadAccelerometer(void) {
    // Configure read transaction
    i2c_transaction_t readTransaction;
    readTransaction.slave_address = MMA8452_ADDRESS;
    readTransaction.blocking = 1;
    readTransaction.channel = I2C_B0;

    // Read multiple bytes from the OUT_X_MSB register for raw data
    // First write the register we want to read from while keeping connection active
    readTransaction.writeData[0] = OUT_X_MSB;
    readTransaction.writeLength = 1;
    readTransaction.readLength = 0;
    readTransaction.stayActive = 1;
    I2C_Transact(&readTransaction);
    // Read from the register six times and terminate the connection
    readTransaction.writeLength = 0;
    readTransaction.readLength = 6;
    readTransaction.stayActive = 0;
    I2C_Transact(&readTransaction);

    // Calculate signed values from the accelerometer
    accel_data.raw_XYZ[0] = ((readTransaction.readData[0] << 8 | readTransaction.readData[1])) >> 4;
    accel_data.raw_XYZ[1] = ((readTransaction.readData[2] << 8 | readTransaction.readData[3])) >> 4;
    accel_data.raw_XYZ[2] = ((readTransaction.readData[4] << 8 | readTransaction.readData[5])) >> 4;

    // Convert the signed values to calculated values in units of g
    accel_data.g_XYZ[0] = (float) accel_data.raw_XYZ[0] / (float)(1 << 11) * (float)(GSCALE);
    accel_data.g_XYZ[1] = (float) accel_data.raw_XYZ[1] / (float)(1 << 11) * (float)(GSCALE);
    accel_data.g_XYZ[2] = (float) accel_data.raw_XYZ[2] / (float)(1 << 11) * (float)(GSCALE);

    // call the callback with our newfound data
    accel_data.callback(accel_data.g_XYZ[0], accel_data.g_XYZ[1], accel_data.g_XYZ[2]);
}
