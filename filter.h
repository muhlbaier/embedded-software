#ifndef _FILTER_H_
#define _FILTER_H_

//Standard Includes 
#include <stdint.h>



/** @file
 * @defgroup MovingAvg Filter Module   
 * 	@ingroup Filters
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
 * 
 * 
 * 
 * Example for implementation with ADC module -- in progress 
 * @code
 * 
 * 
 * ADC_Init();
 * Filter_Init();
 * 
 * QueueMeasurement()
 * 
 * 
 * 
 * 
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
 * 
 * 
 * **/ 








/**
 *  Filter_Init initliazes all variables/arrays for the filter i.e (min,max,index,) including setting the call back functions to NULL 
 *  sets the scale/offset values from user and calculates the value for the shift  
 * 	Sets the default filter to MovingAvgFilter
 * 
 * @param scale - Value that multiples with the input 
 * @param offset - A constant value added to the linear transfer function to compute the desired result if needed
 * 
**/


void Filter_Init(filter_t *, int16_t  offset, float scale); 




/**
 * 
 * Filter update will be called when raw value goes through linear transfer function 
 * new_value = (raw*scale >> shift) + offset 
 * Calls the filter function which returns a int16_t new_value
 * Depending on the thresholds and if min/max callbacks are set 
 * It will call the min/max callback functions with the new filtered value
 * 
 * @param raw_value - Values from ADC module that will go through the linear transfer function 
 * 
**/

void Filter_Update(uint16_t raw_value, filter_t * filter_ptr);



/**
 * 
 *  Filter_Get will retrieve the current value from the filter instance 
 *  
 * @return int16_t value stored in filter_t->value 
 * 
 * 
 * 
 * 

 


**/


int16_t Filter_Get(filter_t *); // return value might need to be float





/**
 * 
 * Filter_SetMin will set the MIN value in the filter to the threshold specified 
 *  Sets the callback function that is passed by the user 
 * 
 * @param threshold - min value 
 * 
**/




void Filter_SetMin(int16_t threshold, void(*callback)(int16_t));





/**
 * 
 * Filter_SetMin will set the MAX value in the filter to the threshold specified 
 * Sets the callback function that is passed by the user 
 * 
 * 
 * @param threshold - max value 
**/

void Filter_SetMax(int16_t threshold, void(*callback)(int16_t));



/**
 * 
 * Moving Average filter - Default filter that adds the previous X values and divides by number of values to compute a rolling mean 
 * 
 * @param  *values - pointer to the values  
 * @param index - location in array  
 * @param size - size of data
 * @param Last_Value - last value in the array
 * @return - returns the average value as int16_t
 * 
**/


int16_t MovingAvgFilter (int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value);


/**
 * 
 * Set_Filter will set the filter structure pointer to the filter function passed by the user 
 * Used if the user wants to implement a different type of filter other than MovingAvg Filter 
 * 
 * 
 * @param new_filter * -  function pointer to the new filter 
 * 
**/


void Set_Filter ( filter_t * , new_filter * );




#endif // _FILTER_H_
