/*
 * hal_adc.h
 *
 *  Created on: Mar 3, 2019
 *      Author: Lonnie Souder II
 */

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <stdint.h>

/** @brief hardware abstraction layer ADC initialization
 *
 * Must be implemented for each MCU in hal_adc.c and configure the ADC for single measurements with
 * interrupts enabled after completion of measurements.
 */
void hal_ADC_Init(void);

/** @brief hardware abstraction layer start ADC measurement for a channel
 *
 * Must be implemented for each MCU in hal_adc.c and setup the ADC to sample the given channel, automatically
 * start the conversion, and interrupt when the conversion is complete. An interrupt service routine also
 * needs to be added to hal_adc.c which calls ADC_ProcessMeasurementFromISR() with the measured value.
 * Note: This triggers a measurement from the channel indicated by 'channel', but the measurement
 * is always stored in ADC12MEM0
 */
void hal_ADC_StartChannel(uint8_t channel);


#endif /* HAL_ADC_H_ */
