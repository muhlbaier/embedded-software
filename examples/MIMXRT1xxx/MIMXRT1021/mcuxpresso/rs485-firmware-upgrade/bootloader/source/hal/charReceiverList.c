#include "system.h"
#ifndef _CHAR_RECEIVER_LIST_H_
#error "Please include charReceiverList.h in system.h"
#endif


void CharReceiverList_Init(charReceiverList_t* rList, charReceiver_t* receiver_array, uint16_t max_size){
	rList->max_size = max_size;
	rList->size = 0;
	rList->receivers = receiver_array;
}

int8_t CharReceiverList_Add(charReceiverList_t* rList, charReceiver_t receiver){
	uint16_t i;
    if(rList->size == rList->max_size) return ENOSPC;

    for(i = 0; i < rList->size; i++) {
        if(rList->receivers[i] == receiver) {
            // receiver already added
            return 0;
        }
    }
	rList->receivers[rList->size++] = receiver; 
	return 0;
}

void CharReceiverList_Remove(charReceiverList_t* rList, charReceiver_t receiver){
	volatile uint16_t i, j;
	// loop through the receivers array for the function pointer (fn)
	// when found shift remaining array items left to overwrite (fn)
	// then set the last function pointer to 0
	for(i = 0; i < rList->size; i++) {
		if(rList->receivers[i] == receiver) {
			for(j = i+1; j < rList->size; j++) {
				rList->receivers[j-1] = rList->receivers[j];
			}
			rList->receivers[j-1] = 0;
			rList->size--;
			return;
		}
	}
}

void CharReceiverList_Run(charReceiverList_t* rList, char c){
	volatile uint16_t i;
	for(i = 0; i < rList->size; i++){
		rList->receivers[i](c);
	}
}
