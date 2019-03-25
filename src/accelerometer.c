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
#include "hal_general.h"

#define GSCALE          2
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low

struct accelerometer_data {
    uint16_t raw_XYZ[3];
    float g_XYZ[3];
    void(*callback)(uint16_t x, uint16_t y, uint16_t z);
};
static struct accelerometer_data accel_data;

static void ReadAccelerometer(void);

void Accelerometer_Init(uint16_t period, void(*callback)(uint16_t x, uint16_t y, uint16_t z)) {
    /// Initialize I2C communication with accelerometer
    // hal_i2c_init();
    // check if online
    /* 
    c = Read 0x0D 
    if c == 0x2A then the accelerometer is online
    else could not connect
    */

    // put in standby mode
    /*
    c = Read(0x2A) (ctrl_reg1)
    // clear the active bit to put into standby
    write(0x2A, c & ~(0x01))
    */

    // set up the full scale range to 2, 4, or 8g
    /*
    fsr = GSCALE;
    if(fsr > 8) fsr = 8; // error check
    fsr >>= 2; 
    write(0x0E, fsr) // write to XYZ data config register
    */

    // put into active mode
    /*
    c = Read(0x2A)
    write(0x2A, c | 0x01) // set active bit high 
    */

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
    uint16_t rawData[6]; // xyz accel register data
    // read 0x01 (OUT_X_MSG) 
    // reads six raw data registers into array [rawData]

    /* Fetching raw data from sensor */
    volatile uint8_t i;
    for(i = 0; i < 3; i++) {
        // combine two 8 bit registers into one 12-bit number
        uint16_t gCount = (rawData[i*2] << 8) | rawData[(i*2)+1];
        gCount >>= 4; // registers left align, so we right aligng 12-bit int

        // if number is negative, we make it manually (no 12-bit data type)
        if(rawData[i*2] > 0x7F) {
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