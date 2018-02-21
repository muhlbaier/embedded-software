/*
 * random_int.c
 *
 *  Created on: Mar 14, 2014
 *      Author: Muhlbaier
 *
 *      code compliments of the interwebs
 */

#include <stdlib.h>
#include <stdint.h>

#include "random_int.h"
#include "timing.h"

int random_int(int16_t min_num, int16_t max_num)
{
	// we need to keep track of the first call to this function
	static char first = 0;
	int result=0,low_num=0,hi_num=0;
	if(min_num<max_num)
	{
		low_num=min_num;
		hi_num=max_num+1; // this is done to include max_num in output.
	}else{
		low_num=max_num+1;// this is done to include max_num in output.
		hi_num=min_num;
	}
	// if it is the first call then initialize the randomizer
	if(first == 0) {
		srand(TimeNow());
		first = 1;
	}
	// generate a random number between low and high num
	result = (rand()%(hi_num-low_num))+low_num;
	return result;
}
