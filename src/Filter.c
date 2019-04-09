#include <msp430>
#include "filter.h"
#include <int_def.h>


#define MAX 65536       //2^16 = 65536
#define FILTER_BUFFER_SIZE 10



typed def struct  {
    
int16_t   offset ;
uint32_t   scale;
uint16_t   shift ;
int16_t   min;
int16_t   max ;
void (*min_callback)(int16_t);
void (*max_callback)(int16_t); 
int16_t (*filter) (int 16_t x values, uint16_t index ,uint16_t size, int16_t Last_Value);
uint16_t raw_values[FILTER_BUFFER_SIZE];   
uint16_t index;
int16_t value;  
void (*set_filter)(filter_t * , new_filter *);      
                                                                                                    
} filter_t;


// convert flot scale value that to integer scale 




void Filter_Init(filter_t *, int16_t offset, float scale)
{

  filter_t->offset = offset;
  
 
    uint16_t counter = 0 ;
  while(scale <  MAX)
  {
            scale = 2*(scale);
            counter++;
  }
    
        filter_t->shift = counter ;     
  
        filter_t->scale = scale;
  
  //set min/max to zero
    filter_t->min = 0;
    filter_t->max = 0; 
  
  //set min max call back functions null pointer
  filter_t->min_callback = NULL;
  filter_t->max_callback = NULL;
  
    //set index and value to 0 
    filter_t->index = 0;
    filter_t->value = 0;
  //initiliaze array to 0
    filter_t->raw_values = 0;
    //default set filter to moving avg filter
    filter_t->set_filter = &MovingAvgFilter; //not sure if this line is correct 
                                                                                                                                                                                                                                

}





void Filter_Update(uint16_t raw_value, filter_t * )    
{
 
  raw_value = (raw_value)*(filter_t->scale);   //scale the value
  
  raw_value = raw_value >> filter_t->shift; // shift the value
  
  raw_value = raw_value + filter_t->offset; //offset if needed
  
  filter_t->raw_values = raw_value;    //input value into buffer array 
  
 

    
    //  int16_t MovingAvgFilter (int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value)
    //need to input those parameters
        new_value = MovingAvgFilter();
        
  
  
  //update filter with 
  //check against thresholds

    if (filter_t->min_callback) //call back is set
            
        if ( new_value < filter_t->min && filter_t->value >= filter_t->min  ) {
          
          filter_t->min_callback(new_value);   //call the callback with current value
                                                    
        }
            
        if (filter_t->max_callback) //call back is set
          
        if ( new_value > filter_t->max && filter_t->value <= filter_t->max  ) {
          
          filter_t->max_callback(new_value);   //call the callback with current value
                                                    
        }
  
  filter_t->value = new_value;     //sets the value to the new value
  
  
}




int16_t Filter_Get(filter_t * ) 
{
        int16_t get_val;
    
    filter_t->value = get_val;

    return get_val;
  

}




void Filter_SetMin(filter_t *, int16_t threshold, void(*callback)(int16_t))
{       
  
    filter_t->min = threshold;
    filter_t->min_callback = callback;
    

  
    
}





void Filter_SetMax(filter_t *, int16_t threshold, void(*callback)(int16_t))
{
    
    filter_t->max = threshold;
    filter_t->max_callback = callback;

    

  
}





int16_t MovingAvgFilter (int16_t *values, uint16_t index ,uint16_t size, int16_t Last_Value)
{
  
  	int32_t sum;
  	
        values[index];
  
            //deceremnt index to get the next value
           

  			for (int i = 0 ; i < size; i++)
        {
          			sum+= values[index];
          				
                 if(index > 0)       // don't decrement if index is 0 
                     index--;				
                  else
                    index = size - 1;
        }
  		
  	avg_value = sum/size;
  
  return avg_value;
}


void set_filter (filter_t * , new_filter * )
{
        //pass pointer to instance of struct and a function pointer 
        // set filter to the new filter 
   filter_t->set_filter = new_filter ;
  
  
}

