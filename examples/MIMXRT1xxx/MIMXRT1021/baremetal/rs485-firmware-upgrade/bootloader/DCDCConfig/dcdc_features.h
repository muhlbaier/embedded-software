/**
*	@file 	dcdc_features.h
*	@brief
*	@date
*	@author	James Ethridge <jethridge@alenconsystems.com>
*   @author Michael Muhlbaier
*
* Copyright (c) 2019, Alencon Systems
* All rights reserved.
*/


/** DCDC_FEATURES
*	@addtogroup DCDC_FEATURES
*	@{
*/

/* ***** Inclusion Gaurd ****** */
#ifndef __dcdc_features_H__
#define __dcdc_features_H__

/* The following is defined in the build configurations for _Rev5 and _Rev6_0 build configs
 * when Rev6_0 build configs are removed uncomment the flags below to build for Rev 6.0 DCDCs
 */
//#define USE_PRE_6_1_PINOUT
//#define DCDC_REV_6_0

/* Enable populating the model with sensor data
*/
#define DCDC_FEATURE_ENABLE_SENSORS 1

/* Enable event injection through the watch window via helper fcn in main
*/
#define DCDC_FEATURE_ENABLE_EVENT_INJECTION 0

/* Enable cmd injection through the watch window via helper fcn in main
*/
#define DCDC_FEATURE_ENABLE_RS485_CMD_INJECTION 0

/* Enable rs485 communications
*/
#define DCDC_FEATURE_ENABLE_DCDC_COMMS 1
#define DCDC_FEATURE_ENABLE_RS485_USART_LOOPBACK_TEST 0

/* OCP handling is validated, but we should keep
this flag until the event dispatching to
the state machine is handled better.
*/
#define DCDC_FEATURE_ENABLE_OVP_DISPATCH 1

/* Enable watchdog timer to prevent DC/DC program from hanging
*/
#define DCDC_FEATURE_WATCHDOG_TIMER 1

/* Test the MPPT algo - sensors must be disabled for this to work.*/
#define DCDC_FEATURE_TEST_MPPT 0

#define DCDC_FEATURE_LOAD_INITIAL_CONFIG 1

// Enable fault detection interrupts from grate driver circuit
#define DCDC_FEATURE_GATE_FAULT_DETECTION 1

// Enable Half period pulses from gate driver
#define DCDC_FEATURE_GATE_PULSE_HALF_PERIOD 1

// period in ms for temperature and humidity sensor readings
#define DCDC_HUMIDITY_SENSOR_PERIOD 1000

// turn DCDC on at resonant frequency as soon as it has aux power
#define DCDC_FEATURE_TEST_STATION_MODE 0

// enable fixed pulse mode in smart gate driver
#define DCDC_FEATURE_ENABLE_PULSE_MODE_SPOT 1
#define DCDC_FEATURE_ENABLE_PULSE_MODE_BOSS 1

// enable the RJ45 shutdown signal to be used to modulate the gate outputs
#define DCDC_ENABLE_PULSED_MODULATION 0

#endif

/** DCDC_FEATURES
*	@}
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */
