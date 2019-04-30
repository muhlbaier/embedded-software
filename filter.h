#ifndef _FILTER_H_
#define _FILTER_H_

#include <stdint.h>
/** @file
* @defgroup filter Filter Module
*
*
*
* This module implements a Filter using a linear transfer function that takes in scale/offset in order to calculate new values
* new_value = raw_value * scale + offset
*
*
* This module defaults to a MovingAvg Filter function. The user is resposible for implementing a filter based on application.
* It can be changed using the Set_Filter function
*
*
*Example, MSP9700 temperature sensor, output voltage directly proportional to measured temperature
*   MCP9700 can accurately measure temperature from -40C to +150C. The output of the MCP9700 is calibrated to a slope of 10mV/°C and has a DC offset of 500mV.
*   The offset allows reading negative temperatures without the need for a negative supply.
*
*   V = T_c * 10mV + 500 mV
*
*   T_c = 0.322*V - 50
*
*   scale = 0.322
*   offset = -50
*
* Example for implementation with ADC module
* @code
*   //example threshold values
*
*   int16_t MinThreshold = 100;
    int16_t MaxThreshold = 150;
    int16_t offset = -50;
    float scale = 0.322;

    // void
    ADC_Init();
    //(filter_t * f_ptr, int16_t  offset, float scale);
    Filter_Init(f_ptr, offset,scale);


    //(int16_t raw_value, filter_t * f_ptr)
    Filter_Update(raw_value, f_ptr);
    //(filter_t * f_ptr, int16_t threshold, void(*callback)(int16_t))
    Filter_SetMax(f_ptr, MinThreshold, 0);
    Filter_SetMin( f_ptr, MaxThreshold,0 );

*
*
*
*
*
* @endcode
*
*
*
* @author Neil Surti
* @author Alexander Revolous
* @author Michael Muhlbaier
*
*@{
* **/










#define FILTER_BUFFER_SIZE 10

typedef struct {

int16_t   offset ;
uint32_t   scale;
uint16_t   shift ;
int16_t   min;
int16_t   max ;
void (*min_callback)(int16_t);
void (*max_callback)(int16_t);
int16_t (*filter)(int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value);
uint16_t raw_values[FILTER_BUFFER_SIZE];
uint16_t index;
int16_t value;
} filter_t;

/**
*
*  @brief Initializes all struct variables and calculates the shift based on scale input for linear transfer function. new_value = raw_value * scale + offset
*
*
* @param scale - Value that multiples with the input
* @param offset - A constant value added to the linear transfer function to compute the desired result if needed
*
**/
void Filter_Init(filter_t * f_ptr, int16_t  offset, float scale);




/**
*
*
* @brief computes the new value after going through the transfer function and inputs it into filter buffer array
*
*
* @param raw_value - Values from ADC module that will go through the linear transfer function
*
**/

void Filter_Update(int16_t raw_value, filter_t * f_ptr);



/**
*
*  @brief Filter_Get will retrieve the current value from the filter instance
*
* @return int16_t value stored in filter_t->value
*
*
*
*




**/


int16_t Filter_Get(filter_t * f_ptr);



/**
*
* @brief Filter_SetMin will set the MIN value in the filter to the threshold specified. Calls the callback function with the current value when the value drops below the threshold
*
* @param threshold - min value
*
**/


void Filter_SetMin(filter_t * f_ptr, int16_t threshold, void(*callback)(int16_t));



/**
*
* @brief  Filter_SetMin will set the MAX value in the filter to the threshold specified. Calls the callback function with the current value when the value goes above the threshold
*
*
* @param threshold - max value
**/

void Filter_SetMax(filter_t * f_ptr, int16_t threshold, void(*callback)(int16_t));


/**
*
* @brief  Moving Average filter - Default filter that adds the previous X values and divides by number of values to compute a rolling mean
*
* @param  *values - pointer to the values
* @param index - location in array
* @param size - size of data
* @param Last_Value - last value in the array
* @return - returns the average value as int16_t
*
**/


int16_t MovingAvgFilter(int16_t *values, uint16_t index, uint16_t size, int16_t Last_Value);


/**
*
* @brief Method that allows user to pass a function to a pointer a different type of filter (i.e IIR, FIR)
*
* @param new_filter * -  function pointer to the new filter
*
**/


void Filter_SetFilter(filter_t * f_ptr, int16_t(*new_filter)(int16_t *values, uint16_t index, uint16_t size, int16_t last_value));



/** @}*/

#endif // _FILTER_H_
