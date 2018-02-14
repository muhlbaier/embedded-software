#include "buffer.h"
#include "string.h"
#include "hal_general.h"

void Buffer_Enqueue(buffer_t *buffer, uint8_t data) {
	uint8_t overwrite = 0;
	// disable interrupts to make the function interrupt safe
	BlockInterrupts();
	
	// increment the rear pointer
	buffer->rear++;
	// check to make sure rear isn't past the end of the buffer
	if(buffer->rear > buffer->buffer_end) buffer->rear = buffer->buffer_start;
	// put data onto rear location
	*buffer->rear = data;
	// Increment the size of the buffer
	buffer->size++;
	
	/*	If front = rear and the buffer is not empty then overwrite what is on
		the front, this allows use as a revolving circular buffer for receivers */
	if(buffer->front == buffer->rear && buffer->size) {
		overwrite = 1;
		buffer->front++;
		// check to make sure front isn't past the end of the buffer
		if(buffer->front > buffer->buffer_end)
			buffer->front = buffer->buffer_start;
		buffer->size--;
	}
	
	// If there is a callback, call it
	if(buffer->Callback) buffer->Callback(buffer);

	//ReEnable Interrupts
	RestoreInterrupts();

	if(overwrite && buffer->OverwriteCallback){
		buffer->OverwriteCallback();
	}
}

uint8_t Buffer_Dequeue(buffer_t *buffer) {
	uint8_t data;
	//x in light of what was documented in the Push function in this file
	//x there is nothing in the Pop function which is not plain given the
	//x good use of variable names in this module
	BlockInterrupts();
	data = *buffer->front;
	if(buffer->size != 0) {
		buffer->front++;
		if(buffer->front > buffer->buffer_end) buffer->front = buffer->buffer_start;
		buffer->size--;
	}
	RestoreInterrupts();
	return data;
}

uint16_t Buffer_GetSize(buffer_t *buffer) {
	return buffer->size;
}

void Buffer_Init(buffer_t *buffer, uint8_t *data_array, uint16_t max_size) {
	buffer->buffer_start = data_array;
	buffer->front = data_array;
	buffer->rear = data_array - 1;
	buffer->buffer_end = data_array + max_size - 1;
	buffer->size = 0;
	buffer->Callback = 0;
}

void Buffer_SetCallback(buffer_t * buffer, void (*Callback)(buffer_t * buffer)) {
	buffer->Callback = (void (*)(void *))Callback; // cast callback to void pointer
	//x a void pointer was used in the structure for buffer_t because a buffer_t pointer
	//x couldn't be used since buffer_t hadn't been defined inside of the definition of
	//x buffer_t. See the .h file for more info.
}

void Buffer_ClearCallback(buffer_t * buffer) { 
    buffer->Callback = 0;
}

uint8_t Buffer_EnqueueData(buffer_t * buffer, uint8_t * data, uint16_t length) {
	BlockInterrupts();
	// check if there is enough space in the buffer to load the data
	if(buffer->buffer_end - buffer->buffer_start + 1 - buffer->size >= length) {
		buffer->size += length;
		if(buffer->buffer_end - buffer->rear >= length) {
			memcpy(buffer->rear + 1, data, length);
			buffer->rear += length;
		}else {
			// copy up to the end
			memcpy(buffer->rear + 1, data, buffer->buffer_end - buffer->rear);
			// over write length with remaining length
			length = length - (buffer->buffer_end - buffer->rear);
			memcpy(buffer->buffer_start, data + (buffer->buffer_end - buffer->rear), length);
			buffer->rear = buffer->buffer_start + length - 1;
		}

		if(buffer->Callback) buffer->Callback(buffer);

		RestoreInterrupts();

		return BUFFER_ENQUEUE_SUCCEEDED;
	}

	RestoreInterrupts();

	return BUFFER_ENQUEUE_FAILED;
}

