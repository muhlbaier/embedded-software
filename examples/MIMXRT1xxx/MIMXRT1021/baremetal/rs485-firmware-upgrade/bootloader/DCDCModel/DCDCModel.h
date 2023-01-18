/**
*	@file 	DCDCModel.h
*	@brief
*	@date
*	@author	James Ethridge <jethridge@alenconsystems.com>
*   @author Michael Muhlbaier
*
* Copyright (c) 2020, Alencon Systems
* All rights reserved.
*
*/


/** PackageName
*	@addtogroup PackageName
*	@{
*/

/** DCDCModel
*	@addtogroup DCDCModel
*	@{
*
* Terminology Clarification:
* output means the output of the circuit in the forward direction.
* directional_output means the rectification side of the circuit regardless of direction.
* input means the input side of the circuit in the forward direction.
* directional_input means the input side of the circuit regardless of direction.
*
*/
/* ***** Inclusion Gaurd ****** */
#ifndef __DCDCModel_H__
#define __DCDCModel_H__

/* ***** Dependencies ****** */
#include <stdint.h>
#include "SpotTypes.h"
/** DCDCModel_Public_Macros
*	@{
*/
//*****************************************************************************
//
// Flags used to maintain SPOT Parameters within desired boundary
//
//
//*****************************************************************************
#define DCDC_DEFAULT_PERIOD           9433        // 9433 ns -> 106 Khz
#define DCDC_PERIOD_STEP                60        // 60ns is default periode step
#define DCDC_MIN_STARTUP_PERIOD       2777        // 360 Khz
#define DCDC_IDLE_STARTUP_PERIOD      6024        // 166 Khz
#define DCDC_DEATIME_ADJ_PERIOD       8333        // 120 Khz
#define DCDC_DEFAULT_FREQ           106000        // default frequency
#define DCDC_PERIOD_MIN               2777        // 360 Khz
#define DCDC_PERIOD_MAX              15000        // 66Khz     (before running MPPT)


#define DCDC_MANUAL_TESTMODE 0x7E5730DE
#define DCDC_BENCH_TESTMODE 0xBE2C8123

#define DCDC_DIRECTION_AUTO    0 // starts out forward unless a negative setpoint is set
#define DCDC_DIRECTION_FORWARD 1
#define DCDC_DIRECTION_REVERSE 2

/** DCDCModel_Public_Macros
*	@}
*/

/** DCDCModel_Public_Types
*	@{
*/


/* This typedefs dcdc_model_handle_t as a constant pointer
     to a dcdc_model_t. This tells the compiler
     that the address of the dcdc_model_t in the system can't change,
     but the data can. (In fact the data is treated as volatile
     due to the way the dcdc_model_t struct is typedef'd)
*/
//typedef dcdc_model_t* const dcdc_model_handle_t;
typedef struct dcdc_model_t* dcdc_model_handle_t;

/** DCDCModel_Public_Types
*	@}
*/

/** DCDCModel_Global_Variables
*	@{
*/

/** DCDCModel_Global_Variables
*	@}
*/

/** DCDCModel_Public_Functions
*	@{
*/

/********************************************************************//**
 * @brief 		Get a handle for the DCDCModel instance
 * @param[in]	None
 * @return 		dcdc_model_handle_t
 *********************************************************************/
dcdc_model_handle_t DCDCModel_get_instance(void);

/********************************************************************//**
 * @brief 		Get the size of the DCDC Model
 * @param[in]	None
 * @return 		size of DCDCModel datastructure as uint32_t
 *********************************************************************/
uint32_t DCDCModel_get_size(void);

void DCDCModel_InitSpotParams(dcdc_model_handle_t model);

/////////////// dcdc_descriptor_t data ////////////////////////////
void DCDCModel_get_descriptor(dcdc_model_handle_t model, dcdc_descriptor_t* descriptor);
// status
uint16_t DCDCModel_get_dcdc_status(dcdc_model_handle_t model);
void     DCDCModel_set_dcdc_status(dcdc_model_handle_t model, uint16_t value);
// timestamp
uint32_t DCDCModel_get_system_time(dcdc_model_handle_t model);
// error_code
void     DCDCModel_set_primary_error_mask(dcdc_model_handle_t model, uint32_t error_mask);
void     DCDCModel_clear_primary_error_mask(dcdc_model_handle_t model, uint32_t error_mask);
uint32_t DCDCModel_get_primary_error_code(dcdc_model_handle_t model);

// error_code_secondary (not in descriptor)
void     DCDCModel_set_secondary_error_mask(dcdc_model_handle_t model, uint32_t error_mask);
void     DCDCModel_clear_secondary_error_mask(dcdc_model_handle_t model, uint32_t error_mask);
uint32_t DCDCModel_get_secondary_error_code(dcdc_model_handle_t model);

/////////////// dcdc_telemetry_t data ////////////////////////////
void DCDCModel_get_telemetry(dcdc_model_handle_t model, dcdc_telemetry_t* telemetry);

// primary_voltage
float DCDCModel_get_primary_voltage(dcdc_model_handle_t model);
float DCDCModel_get_primary_voltage_unflitered(dcdc_model_handle_t model);
float DCDCModel_get_primary_voltage_delta(dcdc_model_handle_t model);
void  DCDCModel_update_primary_voltage(dcdc_model_handle_t model, float voltage);
void  DCDCModel_set_primary_voltage(dcdc_model_handle_t model, float value);
// input voltage (primary when forward, secondary when reverse)
float DCDCModel_get_input_voltage(dcdc_model_handle_t model);
float DCDCModel_get_input_voltage_unfiltered(dcdc_model_handle_t model);

// primary_current
float DCDCModel_get_primary_current(dcdc_model_handle_t model);
float DCDCModel_get_primary_current_unfiltered(dcdc_model_handle_t model);
float DCDCModel_get_primary_current_delta(dcdc_model_handle_t model);
void  DCDCModel_update_primary_current(dcdc_model_handle_t model, float value);
void  DCDCModel_set_primary_current(dcdc_model_handle_t model, float value);
void  DCDCModel_estimate_primary_current(dcdc_model_handle_t model);
// input current (primary when forward, secondary when reverse)
float DCDCModel_get_input_current(dcdc_model_handle_t model);
float DCDCModel_get_input_current_unfiltered(dcdc_model_handle_t model);

// secondary_voltage
float DCDCModel_get_secondary_voltage(dcdc_model_handle_t model);
float DCDCModel_get_secondary_voltage_unflitered(dcdc_model_handle_t model);
float DCDCModel_get_secondary_voltage_delta(dcdc_model_handle_t model);
void  DCDCModel_set_secondary_voltage(dcdc_model_handle_t model, float voltage);
void  DCDCModel_update_secondary_voltage(dcdc_model_handle_t model, float value);
// output voltage (secondary when forward, primary when reverse)
float DCDCModel_get_output_voltage(dcdc_model_handle_t model );
float DCDCModel_get_output_voltage_unfiltered(dcdc_model_handle_t model);
float DCDCModel_get_output_voltage_delta(dcdc_model_handle_t model);

// secondary_current
float DCDCModel_get_secondary_current(dcdc_model_handle_t model);
float DCDCModel_get_secondary_current_unfiltered(dcdc_model_handle_t model);
float DCDCModel_get_secondary_current_delta(dcdc_model_handle_t model);
void  DCDCModel_update_secondary_current(dcdc_model_handle_t model, float value);
void  DCDCModel_set_secondary_current(dcdc_model_handle_t model, float value);
// output current (secondary when forward, primary when reverse)
float DCDCModel_get_output_current(dcdc_model_handle_t model);
float DCDCModel_get_output_current_unfiltered(dcdc_model_handle_t model);

// onboard_temperature
float DCDCModel_get_board_temperature(dcdc_model_handle_t model);
void  DCDCModel_update_board_temperature(dcdc_model_handle_t model, float value);
void  DCDCModel_set_board_temperature(dcdc_model_handle_t model, float value);
// primary_temperature
float DCDCModel_get_primary_temperature(dcdc_model_handle_t model);
float DCDCModel_get_primary_case_temperature(dcdc_model_handle_t model);
float DCDCModel_get_primary_ambient_temperature(dcdc_model_handle_t model);
void  DCDCModel_update_primary_temperature(dcdc_model_handle_t model, float value);
void  DCDCModel_set_primary_temperature(dcdc_model_handle_t model, float value);

// dcdc_gate_freq_hz
uint32_t DCDCModel_get_frequency(dcdc_model_handle_t model);
void     DCDCModel_set_frequency(dcdc_model_handle_t model, uint32_t value);
uint32_t DCDCModel_get_virtual_frequency(dcdc_model_handle_t model);
void     DCDCModel_set_virtual_frequency(dcdc_model_handle_t model, uint32_t value);

/////////////// dcdc_diagnostic_t data ////////////////////////////
void DCDCModel_get_diagnostic(dcdc_model_handle_t model, dcdc_diagnostic_t* diagnostic);

// mppt_status
uint16_t DCDCModel_get_control_state(dcdc_model_handle_t model);
void     DCDCModel_set_control_state(dcdc_model_handle_t model, uint16_t value );
// aux_5volts
float DCDCModel_get_aux_5volts(dcdc_model_handle_t model);
void  DCDCModel_update_aux_5volts(dcdc_model_handle_t model, float value);
void  DCDCModel_set_aux_5volts(dcdc_model_handle_t model, float value);
// aux_3_3volts
float DCDCModel_get_aux_3_3volts(dcdc_model_handle_t model);
void  DCDCModel_update_aux_3_3volts(dcdc_model_handle_t model, float value);
void  DCDCModel_set_aux_3_3volts(dcdc_model_handle_t model, float value);

////////////////// BOSS data ///////////////////////////////////////
// secondary temperature
float DCDCModel_get_secondary_temperature(dcdc_model_handle_t model);
float DCDCModel_get_secondary_case_temperature(dcdc_model_handle_t model);
float DCDCModel_get_secondary_ambient_temperature(dcdc_model_handle_t model);
void  DCDCModel_update_secondary_temperature(dcdc_model_handle_t model, float value);
void  DCDCModel_set_secondary_temperature(dcdc_model_handle_t model, float value);
////////////////// BOSS control ///////////////////////////////////////
// direction
uint16_t DCDCModel_get_direction(dcdc_model_handle_t model);
void  DCDCModel_set_direction(dcdc_model_handle_t model, uint16_t value);
// setpoint
float DCDCModel_get_setpoint(dcdc_model_handle_t model);
void  DCDCModel_set_setpoint(dcdc_model_handle_t model, float value);
void  DCDCModel_force_set_setpoint(dcdc_model_handle_t model, float value);
uint32_t DCDCModel_get_time_since_setpoint(dcdc_model_handle_t model);
float DCDCModel_get_current_setpoint(dcdc_model_handle_t model);				// Setpoint converted to current
/** Get target setpoint which may be lower than the actual setpoint due to
 *  curtailment.
 *
 *  @param (dcdc_model_handle_t) model - pointer to DCDC Model
 *  @return (float) setpoint - note: will be negative for reverse current/power
 */
float DCDCModel_get_target_setpoint(dcdc_model_handle_t model);
/** Set target setpoint.
 *
 *  @param (dcdc_model_handle_t) model - pointer to DCDC Model
 *  @param (float) setpoint - note: make negative for reverse current/power
 */
void  DCDCModel_set_target_setpoint(dcdc_model_handle_t model, float value);
// setpoint timeout
uint16_t DCDCModel_get_setpoint_timeout(dcdc_model_handle_t model);
void  DCDCModel_set_setpoint_timeout(dcdc_model_handle_t model, uint16_t value);
// voltage_setpoint
float DCDCModel_get_voltage_setpoint(dcdc_model_handle_t model);
void  DCDCModel_set_voltage_setpoint(dcdc_model_handle_t model, float value);
// control_mode
void DCDCModel_set_control_mode(dcdc_model_handle_t model, uint8_t mode);
uint8_t DCDCModel_get_control_mode(dcdc_model_handle_t model);
uint8_t  DCDCModel_get_secondary_control(dcdc_model_handle_t model);

////////////// Misc /////////////////////////
// dcdc_state
uint16_t DCDCModel_get_dcdc_state(dcdc_model_handle_t model);
void     DCDCModel_set_dcdc_state(dcdc_model_handle_t model, uint16_t value);
// dcdc_running
uint16_t DCDCModel_get_dcdc_running(dcdc_model_handle_t model);
void     DCDCModel_set_dcdc_running(dcdc_model_handle_t model, uint16_t value);
// zero_current_measurements
void DCDCModel_zero_current_measurements(dcdc_model_handle_t model);
// kill_switch
uint8_t DCDCModel_get_kill_switch(dcdc_model_handle_t model);
void    DCDCModel_set_kill_switch(dcdc_model_handle_t model, uint8_t ks_state);
// testmode_data
void     DCDCModel_set_testmode_data(dcdc_model_handle_t model, uint32_t data);
uint32_t DCDCModel_get_testmode_data(dcdc_model_handle_t model);

/*** Internal/Debug Data ***/
uint32_t DCDCModel_get_i2c_transaction_count(dcdc_model_handle_t model);
void     DCDCModel_set_i2c_transaction_count(dcdc_model_handle_t model, uint32_t value);

uint32_t DCDCModel_get_rs485_tx_count(dcdc_model_handle_t model);
void     DCDCModel_set_rs485_tx_count(dcdc_model_handle_t model, uint32_t value);

uint32_t DCDCModel_get_rs485_rx_count(dcdc_model_handle_t model);
void     DCDCModel_set_rs485_rx_count(dcdc_model_handle_t model, uint32_t value);

uint16_t DCDCModel_get_mppt_running(dcdc_model_handle_t model);
void     DCDCModel_set_mppt_running(dcdc_model_handle_t model, uint16_t value);

uint8_t DCDCModel_get_ovp_state(dcdc_model_handle_t model);
void    DCDCModel_set_ovp_state(dcdc_model_handle_t model, uint8_t state);

uint8_t DCDCModel_get_ovp_isr_latch(dcdc_model_handle_t model);
void    DCDCModel_set_ovp_isr_latch(dcdc_model_handle_t model, uint8_t state);

void DCDCModel_gate_fault_detected(dcdc_model_handle_t model);
uint32_t DCDCModel_get_gate_fault_count(dcdc_model_handle_t model);

float DCDCModel_get_humidity_sensor_temperature(dcdc_model_handle_t model);
void DCDCModel_update_humidity_sensor_temperature(dcdc_model_handle_t model, float temperature);
void DCDCModel_set_humidity_sensor_temperature(dcdc_model_handle_t model, float temperature);

float DCDCModel_get_humidity(dcdc_model_handle_t model);
void DCDCModel_update_humidity(dcdc_model_handle_t model, float humidity);
void DCDCModel_set_humidity(dcdc_model_handle_t model, float humidity);

void DCDCModel_set_ground_fault_meas_positive(dcdc_model_handle_t model, float value);
float DCDCModel_get_ground_fault_meas_positive(dcdc_model_handle_t model);
void DCDCModel_set_ground_fault_meas_negative(dcdc_model_handle_t model, float value);
float DCDCModel_get_ground_fault_meas_negative(dcdc_model_handle_t model);

void DCDCModel_set_ground_fault_resistance(dcdc_model_handle_t model, float value);
float DCDCModel_get_ground_fault_resistance(dcdc_model_handle_t model);
void DCDCModel_set_ground_fault_location(dcdc_model_handle_t model, float value);
float DCDCModel_get_ground_fault_location(dcdc_model_handle_t model);

float DCDCModel_convert_to_output_current(dcdc_model_handle_t model, float input_current);
float DCDCModel_convert_to_input_current(dcdc_model_handle_t model, float output_current);

float DCDCModel_get_dPdt(dcdc_model_handle_t model);

////////////// PODD Data /////////////////////////
void DCDCModel_set_ambient_temperature(dcdc_model_handle_t model, int16_t temperature);
int16_t DCDCModel_get_ambient_temperature(dcdc_model_handle_t model);
void DCDCModel_set_ambient_humidity(dcdc_model_handle_t model, uint16_t humidity);
uint16_t DCDCModel_get_ambient_humidity(dcdc_model_handle_t model);

/////////////////////// Gate Driver //////////////////////////////
uint32_t DCDCModel_get_actual_frequency(dcdc_model_handle_t model);

void DCDCModel_set_pulse_width_ns(dcdc_model_handle_t model, uint32_t pulse_width_ns);
uint32_t DCDCModel_get_pulse_width_ns(dcdc_model_handle_t model);
uint32_t DCDCModel_get_actual_pulse_width_ns(dcdc_model_handle_t model);

void DCDCModel_set_period_slew_rate_ns(dcdc_model_handle_t model, uint16_t slew_rate_ns);
uint16_t DCDCModel_get_period_slew_rate_ns(dcdc_model_handle_t model);

void DCDCModel_set_pulse_width_slew_rate_ns(dcdc_model_handle_t model, uint16_t slew_rate_ns);
uint16_t DCDCModel_get_pulse_width_slew_rate_ns(dcdc_model_handle_t model);

void DCDCModel_set_active_rect_enable(dcdc_model_handle_t model, uint16_t enable);
uint16_t DCDCModel_get_active_rect_enable(dcdc_model_handle_t model);

void DCDCModel_set_active_rect_delay_ns(dcdc_model_handle_t model, uint16_t delay_ns);
uint16_t DCDCModel_get_active_rect_delay_ns(dcdc_model_handle_t model);

void DCDCModel_set_active_rect_pulse_width_ns(dcdc_model_handle_t model, uint32_t pulse_width_ns);
uint32_t DCDCModel_get_active_rect_pulse_width_ns(dcdc_model_handle_t model);
uint32_t DCDCModel_get_actual_active_rect_pulse_width_ns(dcdc_model_handle_t model);

void DCDCModel_set_alpha(dcdc_model_handle_t model, float alpha);
float DCDCModel_get_alpha(dcdc_model_handle_t model);
float DCDCModel_get_actual_alpha(dcdc_model_handle_t model);
void DCDCModel_set_control_alpha(dcdc_model_handle_t model, float alpha);
float DCDCModel_get_control_alpha(dcdc_model_handle_t model);
void DCDCModel_set_overlap_alpha(dcdc_model_handle_t model, float alpha);
float DCDCModel_get_overlap_alpha(dcdc_model_handle_t model);

void DCDCModel_set_deadtime_ns(dcdc_model_handle_t model, uint16_t deadtime_ns);
uint16_t DCDCModel_get_deadtime_ns(dcdc_model_handle_t model);
uint32_t DCDCModel_get_actual_input_deadtime_ns(dcdc_model_handle_t model);
uint32_t DCDCModel_get_actual_output_deadtime_ns(dcdc_model_handle_t model);

void DCDCModel_set_dcdc_running(dcdc_model_handle_t model, uint16_t run);
uint16_t DCDCModel_get_dcdc_running(dcdc_model_handle_t model);

void DCDCModel_set_normal_mode_waveform(dcdc_model_handle_t model, uint32_t freqHz);
void DCDCModel_set_waveform(dcdc_model_handle_t model, uint32_t freqHz, uint32_t pulse_width_ns);
void DCDCModel_bidirectional_waveform_set(dcdc_model_handle_t model, uint32_t freqHz, uint32_t pulse_width_ns, uint32_t active_rect_pulse_width_ns);

void DCDCModel_SetTransformerTemperature(dcdc_model_handle_t model, float temperature);
float DCDCModel_GetTransformerTemperature(dcdc_model_handle_t model);
void DCDCModel_UpdateTransformerTemperature(dcdc_model_handle_t model, float temperature);
void DCDCModel_UpdateCalculatedTransformerTemperature(dcdc_model_handle_t model);

void DCDCModel_set_SBCM_nominal_voltage(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_max_discharge(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_max_charge(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_discharge_slope(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_charge_slope(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_discharge_deadband(dcdc_model_handle_t model, float value);
void DCDCModel_set_SBCM_charge_deadband(dcdc_model_handle_t model, float value);

#endif
/** DCDCModel
*	@}
*/

/** PackageName
*	@}
*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */
