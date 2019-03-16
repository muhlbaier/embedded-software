#include "adc.h"

/** documentation for struct */
struct adc_channel {
	uint8_t channel; // channel index for the ADC
	uint16_t value; ///< result of ADC measurement (documentation for struct member)
	void(*callback)(uint16_t, void *); // callback function pointer
	void * ptr; // optional pointer for the callback function
};

static struct adc_channel channels[ADC_MAX_CHANNELS];
// number of channels that have been added via ADC_AddChannel()
static uint8_t num_channels;

#define ADC_QUEUE_SIZE 10
static struct adc_channel * pending_measurements[ADC_QUEUE_SIZE];
// start is the index of the first pending measurement when start != end and full is 0
// end is the next available index to add a pending measurement
// full is a binary flag to indicate if the circular queue is full or not
static uint8_t start, end, full;


// Initialized the QueueMeasurement functions
static void QueueMeasurement(struct adc_channel * channel);

// Initialized the CallCallback functions
static void CallCallback(struct adc_channel * channel_struct);

void ADC_Init(void) {
	num_channels = 0;
	start = 0;
	end = 0;
	full = 0;
	hal_ADC_Init();
}

void ADC_AddChannel(uint8_t channel, uint16_t period, void(*callback)(uint16_t, void *), void * ptr) {
	if(num_channels < ADC_MAX_CHANNELS) {
		channels[num_channels].channel = channel;
		channels[num_channels].callback = callback;
		channels[num_channels].ptr = ptr;
		Task_Schedule(QueueMeasurement, &channels[num_channels], period, period);
		num_channels++;
	}
}

static void QueueMeasurement(struct adc_channel * channel) {
	// add measurement to the queue
	if(!full) {
		BlockInterrupts();
		// if empty then start the ADC conversion
		if(start == end) {
			hal_ADC_StartChannel(channel->channel);
		}
		pending_measurements[end++] = channel;
		if(end >= ADC_QUEUE_SIZE) end = 0;
		if(end == start) full = 1;
		RestoreInterrupts();
	}
}

// must be interrupt safe
void ADC_ProcessMeasurementFromISR(uint16_t value) {
	pending_measurements[start]->value = value;
	Task_Queue(CallCallback, pending_measurements[start]);
	full = 0;
	start++;
	if(start >= ADC_QUEUE_SIZE) start = 0;
	// if not empty then start the next conversion
	if(start != end) {
		hal_ADC_StartChannel(pending_measurements[start]->channel);
	}
}

static void CallCallback(struct adc_channel * channel_struct) {
	// if the pointer is set then call the callback with the value and the pointer
	if(channel_struct->ptr) {
		channel_struct->callback(channel_struct->value, channel_struct->ptr);
	}else {
		// typedef to cast callback to have only one input
		((callback_no_input_t)channel_struct->callback)(channel_struct->value);
	}
}
