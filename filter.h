// Filter Module


// ****CHECKPOINT GOALS****
// USER INTERFACE
// PSEUDO CODE
//IMPLEMENTATION
//WELL DOCUMENTATED HEADER FILE
// DOXYGEN FILE 
// TEMPLATE FOR C FILE



/*
@param filter_t * - a pointer of struct type filter_t
@param offset -  describes postion of the data going into an array
@param scale -  the parameter value that multiples with the normalized input

*/


void Filter_Init(filter_t *, uint16_t  offset, float scale); 




/* 
@param raw_value - quantized values stored in the array 
@param filter_t * filter_ptr - pointer that points to the raw values in the array

*/

void Filter_Update(uint16_t raw_value, filter_t * filter_ptr);



/*
 
@param filter_t * -


*/


uint16_t Filter_Get(filter_t *); // return value might need to be float





/*
@param threshold - minimum value i.e 0 
@param - call back function - function to be passed by user 

*/




void Filter_SetMin(uint16_t threashold, void(*callback)(uint16_t));





/*
@param threshold - maximum value i.e 65536 
@param - call back function - function to be passed by user 

*/

void Filter_SetMax(uint16_t threashold, void(*callback)(uint16_t));
