#include "filter.h"
#include <int_def.h>
#include "hal_general.h"
#include <string.h>
#include <stdio.h>

#define MAX 32768       
#define FILTER_BUFFER_SIZE 10




void Filter_Init(filter_t * f_ptr, int16_t offset, float scale)
{

  f_ptr->offset = offset;
  
 
    uint16_t counter = 0 ;
  while(scale <  MAX)
  {
            scale = 2*(scale);
            counter++;
  }
    
        f_ptr->shift = counter ;     
  
        f_ptr->scale = scale;
  
  //set min/max to zero
    f_ptr->min = 0;
    f_ptr->max = 0; 
  
  //set min max call back functions null pointer
  f_ptr->min_callback = NULL;
  f_ptr->max_callback = NULL;
  
    //set index and value to 0 
    f_ptr->index = 0;
    f_ptr->value = 0;
  
  //initialize array to 0
  memset(f_ptr->raw_values,0,sizeof(f_ptr->raw_values));

   //point filter to MovingAvgFilter function
    f_ptr->filter = MovingAvgFilter;
                                                                                                                                                                                                                                

}


void Filter_Update(int16_t raw_value, filter_t * f_ptr)
{
 
  raw_value = (raw_value)*(f_ptr->scale);   //scale the value
  
  raw_value = raw_value >> f_ptr->shift; // shift the value
  
  raw_value = raw_value + f_ptr->offset; //offset if needed
  

  for (int i = 0 ; i < FILTER_BUFFER_SIZE ; i++)    //input value into buffer array
  {
      f_ptr->raw_values[i] = raw_value;
  }


  
    //  int16_t MovingAvgFilter (int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value)
    //need to input those parameters
       int16_t new_value = MovingAvgFilter(&raw_value,f_ptr->index,FILTER_BUFFER_SIZE, FILTER_BUFFER_SIZE - 1);
        
  
  

    if (f_ptr->min_callback) //call back is set
            //new value is less than fmin and fvalue >= fmin 
        if ( new_value < f_ptr->min && f_ptr->value >= f_ptr->min  ) {
          
          f_ptr->min_callback(new_value);   //call the callback with current value
                                                    
        }
            
        if (f_ptr->max_callback) //call back is set
            //new value is greater  than fmax and fvalue <= fmax
        if ( new_value > f_ptr->max && f_ptr->value <= f_ptr->max  ) {
          
          f_ptr->max_callback(new_value);   //call the callback with current value
                                                    
        }
  
  f_ptr->value = new_value;     //sets the value to the new value
  
  
}


int16_t Filter_Get(filter_t * f_ptr ) 
{
        const int16_t get_val = 0;
    
    f_ptr->value = get_val;

    return get_val;
  

}


void Filter_SetMin(filter_t * f_ptr, int16_t threshold, void(*callback)(int16_t))
{       
  
    f_ptr->min = threshold;
    f_ptr->min_callback = callback;
       
}





void Filter_SetMax(filter_t * f_ptr, int16_t threshold, void(*callback)(int16_t))
{
    
    f_ptr->max = threshold;
    f_ptr->max_callback = callback;
  
}



int16_t MovingAvgFilter (int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value)
{

    int32_t sum, avg_value;

        volatile int i;

  			for ( i = 0 ; i < FILTER_BUFFER_SIZE; i++)
        {
          			sum+= values[index];
          				
                 if(index > 0)
                     index--;				
                  else
                    index = FILTER_BUFFER_SIZE - 1;
        }
  		
  	avg_value = sum/FILTER_BUFFER_SIZE;
  
  return avg_value;
}



void Filter_SetFilter(filter_t * f_ptr, int16_t (*new_filter)(int16_t *values, uint16_t index ,uint16_t size, int16_t last_value))
{
  
  
     f_ptr->filter = new_filter ;
  
}


