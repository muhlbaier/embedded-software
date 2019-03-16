#include <stdint.h>
#include "task.h"
#include "hal_general.h"

#ifndef _ADC_H_
#define _ADC_H_
/**
 * @defgroup adc_module ADC Module
 *
 * Here is my description of what this module does and how to use it.
 *
 * @{
 */
/**
 * @brief helpful typedef for call back
 *
 * To be used in adc.c where the call back is used without the optional pointer
 *
 * @author Page
 */
typedef void(*callback_no_input_t)(uint16_t);
/** @brief Initialize the ADC module
 * 
 * This is the longer description which uses more words to say initializes the ADC module.
 *
 * @author Muhlbaier
 * @version 0.0
 */ 
void ADC_Init(void);

/** @brief add a channel to the list of channels to measure, a period at which to measure it, and a callback
 *
 * @param channel ADC channel index base 0
 * @param period Period between measurements in milliseconds
 * @param callback Callback when measurements are complete. Can be void Function(uint16_t value) or void Function(uint16_t value, void * ptr)
 * @param ptr Optional pointer to be passed to the callback function
 */
void ADC_AddChannel(uint8_t channel, uint16_t period, void(*callback)(uint16_t, void *), void * ptr);

/**
 * Function to be called from the ADC interrupt in hal_adc.c with the measurement
 *
 * @param value the ADC measurement
 */
void ADC_ProcessMeasurementFromISR(uint16_t value);

/** @brief hardware abstaction layer ADC initialization
 *
 * Must be implemented for each MCU in hal_adc.c and configure the ADC for single measurements with
 * interrupts enabled after completion of measurements.
 */
void hal_ADC_Init(void);

/** @brief hardware abstaction layer start ADC measurement for a channel
 *
 * Must be implemented for each MCU in hal_adc.c and setup the ADC to sample the given channel, automatically
 * start the conversion, and interrupt when the conversion is complete. A interrupt service routine also
 * needs to be added to hal_adc.c which calls ADC_ProcessMeasurementFromISR() with the measured value.
 */
void hal_ADC_StartChannel(uint8_t channel);

/** Maximum number of concurrent channels */
#define ADC_MAX_CHANNELS 8

 /** @} */


#endif // _ADC_H_
