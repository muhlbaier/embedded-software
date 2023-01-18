#ifndef __SpotTypes_H__
#define __SpotTypes_H__
#include <stdint.h>
#include "system.h"

/*****************************************************************************/
//         (c) Copyright, Alencon Systems, All rights reserved.
//
/*****************************************************************************/

// allow anonymous structs for more readable unions
//#pragma anon_unions

/* The following constants will be available in the generated code as macros.
 * If you use #define in an IDL file, the value will be substituite by a
 * preprocessor before generating the code (this constant won't be available
 * in your program).
 */

/* Define DCDC Commands */
typedef enum
{
    DCDC_COMMAND_NONE,
    DCDC_COMMAND_POLL,			            ///< Not in use
    DCDC_COMMAND_CLEAR_FAULT,
    DCDC_COMMAND_SHUTDOWN,
    DCDC_COMMAND_GET_DIAGNOSTIC,            ///< Not in use
    DCDC_COMMAND_ADJUST_VOUT_CALIBRATION,   ///< Not in use
    DCDC_COMMAND_TESTMODE_OFF,              ///< remove and replace with a manual control mode
    DCDC_COMMAND_TESTMODE_ON,               ///< remove and replace with a manual control mode
    DCDC_COMMAND_DCDC_FREQUENCY,            ///< Not in use - done via frequency register in manual control mode
    DCDC_COMMAND_DCDC_DEADBAND,             ///< Not in use - done via pulse width register in manual control mode
    DCDC_COMMAND_CURRENT_SETPOINT,          ///< Not in use - done via setpoint register
    DCDC_COMMAND_SET_DIRECTION,             ///< Not in use - done via direction register
    DCDC_COMMAND_TEST_APPLICATION_HANG,     ///< Not in use / cause the application to hang forever -- TESTMODE
    DCDC_COMMAND_GET_CONFIGURATION_DATA,    ///< Not in use
    DCDC_COMMAND_SYSTEM_RESET,
    DCDC_COMMAND_PRECHARGE,                 ///< Used on GARD
    DCDC_COMMAND_RUN,
    DCDC_COMMAND_STANDBY,
    DCDC_COMMAND_MAX = INT16_MAX
}dcdc_command_t;

/*
*	DCDC Status Descriptor: state of the DCDC controller
*/
typedef enum {
    DCDC_STATUS_DEFAULT,
    DCDC_STATUS_INITIALIZING,
    DCDC_STATUS_STANDBY,
    DCDC_STATUS_STARTUP,
    DCDC_STATUS_RUN,
    DCDC_STATUS_LIMITING_ON,
    DCDC_STATUS_TRIPPED,
    DCDC_STATUS_SHUTDOWN,
    DCDC_STATUS_TESTMODE,
    DCDC_STATUS_TESTMODE_STANDBY,
    DCDC_STATUS_SYSTEM_RESET,
    DCDC_STATUS_BOOTLOADER,
    DCDC_STATUS_PRECHARGE,
    DCDC_STATUS_INVERTER_DETECT,    ///< Not in use
    DCDC_STATUS_TESTMODE_STARTUP,
    DCDC_STATUS_DESCRIPTOR_MAX = INT16_MAX
}dcdc_status_descriptor_t;

/* Describes the identity and global operating mode (status)
of a DCDC converter */
typedef struct {
    dcdc_status_descriptor_t status;
    uint32_t timestamp;
    uint32_t primary_error_code;
}dcdc_descriptor_t;

/* DCDC Error Codes */
#define DCDC_ERROR_OVP_PRIMARY          0x00000001
#define DCDC_ERROR_OCP_INPUT_PVDC       0x00000002
#define DCDC_ERROR_OCP_INPUT_AC         0x00000004 /// no longer in use
#define DCDC_ERROR_OCP_PRIMARY          0x00000008
#define DCDC_ERROR_ARCFAULT             0x00000010 /// no longer in use
#define DCDC_ERROR_GROUNDFAULT          0x00000020
#define DCDC_ERROR_I2C_FAILURE          0x00000040
#define DCDC_ERROR_RS485_FAILURE        0x00000080
#define DCDC_ERROR_OTP_PRIMARY	        0x00000100
#define DCDC_ERROR_OCP_INPUT_AC_SENSOR  0x00000200 /// no longer in use
#define DCDC_ERROR_OVP_SENSOR           0x00000400 /// no longer in use
#define DCDC_ERROR_OTP_MCU              0x00000800
#define DCDC_ERROR_BOOTLOADER_MODE      0x00001000
#define DCDC_ERROR_CONFIGURATION        0x00002000
#define DCDC_ERROR_KILL_SWITCH_ON       0x00004000
#define DCDC_ERROR_AUX_POWER            0x00008000
#define DCDC_ERROR_VOUT_SENSOR          0x00010000 /// no longer in use
#define DCDC_ERORR_Q3_GATE_FAULT        0x00020000
#define DCDC_ERORR_Q4_GATE_FAULT        0x00040000
#define DCDC_ERORR_Q5_GATE_FAULT        0x00080000
#define DCDC_ERORR_Q6_GATE_FAULT        0x00100000
#define DCDC_ERORR_Q3_GATE_FEEDBACK     0x00200000
#define DCDC_ERORR_Q4_GATE_FEEDBACK     0x00400000
#define DCDC_ERORR_Q5_GATE_FEEDBACK     0x00800000
#define DCDC_ERORR_Q6_GATE_FEEDBACK     0x01000000
#define DCDC_ERORR_CLOCK_FAULT          0x02000000
#define DCDC_ERORR_FAN_FAULT            0x04000000
#define DCDC_ERORR_GATE_INTERRUPT       0x08000000
#define DCDC_ERORR_RESERVED_28          0x10000000
#define DCDC_ERORR_RESERVED_29          0x20000000
#define DCDC_HARD_FAULT                 0x40000000
#define DCDC_ERROR_EVENT_DETECTED       0x80000000

#define DCDC_ERROR_OVP_SECONDARY        0x00000001
#define DCDC_ERROR_OCP_SECONDARY        0x00000008
#define DCDC_ERROR_OTP_SECONDARY        0x00000100
#define DCDC_ERORR_Q3_BATTERY_FAULT     0x00020000
#define DCDC_ERORR_Q4_BATTERY_FAULT     0x00040000
#define DCDC_ERORR_Q5_BATTERY_FAULT     0x00080000
#define DCDC_ERORR_Q6_BATTERY_FAULT     0x00100000
#define DCDC_ERORR_Q3_BATTERY_FEEDBACK  0x00200000
#define DCDC_ERORR_Q4_BATTERY_FEEDBACK  0x00400000
#define DCDC_ERORR_Q5_BATTERY_FEEDBACK  0x00800000
#define DCDC_ERORR_Q6_BATTERY_FEEDBACK  0x01000000

// CUBE Specific Codes
#define CUBE_ERROR_OTP_SECONDARY        0x20000000
#define DCDC_ERORR_OVP_SECONDARY_INTERRUPT 0x10000000
#define DCDC_ERORR_PM1_HIGH_SIDE_FAULT  0x00020000
#define DCDC_ERORR_PM1_LOW_SIDE_FAULT   0x00040000
#define DCDC_ERORR_PM2_HIGH_SIDE_FAULT  0x00080000
#define DCDC_ERORR_PM2_LOW_SIDE_FAULT   0x00100000
#define DCDC_ERORR_PM1_HIGH_SIDE_READY  0x00200000
#define DCDC_ERORR_PM1_LOW_SIDE_READY   0x00400000
#define DCDC_ERORR_PM2_HIGH_SIDE_READY  0x00800000
#define DCDC_ERORR_PM2_LOW_SIDE_READY   0x01000000

/* Describes the telemetry data for the DCDC
which is used in production*/
typedef struct {
    float   primary_current; // Amps
    float   primary_voltage; // Volts
    float   onboard_temperature; // C
    float   primary_temperature; // C
    float   secondary_current; // Amps
    float   secondary_voltage; // Volts
    float   primary_power; // kW
    float   secondary_power; // kW
}dcdc_telemetry_t;

/* Describes the data collected when DCDC is specifically
        configured for debug mode.
*/
typedef struct {
    uint8_t	mppt_status;
    float aux_5volts; // Volts
    float aux_3_3volts; // Volts
}dcdc_diagnostic_t;

typedef struct {
    // control_mode moved to config in v17
    // uint16_t control_mode;
    float setpoint;
    float last_setpoint;
	float target_setpoint;
    float voltage_setpoint;
    float setpoint_charge_max_amps;
    float setpoint_discharge_max_amps;
	uint16_t direction;
    uint16_t battery_voltage_max;
    uint16_t battery_voltage_min;
    uint16_t active_rectification;
	uint32_t setpoint_time;
	uint16_t setpoint_timeout;
}boss_control_t;

typedef struct {
    int16_t ambient_temperature;
    uint16_t ambient_humidity;
}podd_data_t;

typedef struct {
    uint16_t pulse_slew_rate_ns;
	uint16_t period_slew_rate_ns;
	uint16_t active_rect_delay_ns;
	uint16_t active_rect_enable;
	uint32_t active_rect_pulse_width_ns;
	uint32_t pulse_width_ns;
	uint32_t actual_frequency;
	float alpha;
	float control_alpha;
	float overlap_alpha;
	uint32_t deadtime_ns;
}gate_driver_t;

#define DCDC_CONTROL_STANDBY 0
#define DCDC_CONTROL_MPPT   1
#define DCDC_CONTROL_CURRENT 2 // primary
#define DCDC_CONTROL_VOLTAGE 3 // primary
#define DCDC_CONTROL_POWER 4 // primary
#define DCDC_CONTROL_MPPT_DEPRICATED 4
#define DCDC_CONTROL_SMART_CURRENT_DEPRICATED 5
#define DCDC_CONTORL_PRIMARY_UNUSED_6 6
#define DCDC_CONTROL_OLD_CURRENT 7
#define DCDC_CONTROL_MANUAL 8 ///< to replace testmode
#define DCDC_CONTORL_SECONDARY_UNUSED_9 9
#define DCDC_CONTROL_SECONDARY_CURRENT 10
#define DCDC_CONTROL_SECONDARY_VOLTAGE 11
#define DCDC_CONTROL_SECONDARY_POWER 12
#define DCDC_CONTROL_SECONDARY_UNUSED_13 13
#define DCDC_CONTROL_SECONDARY_UNUSED_14 14
#define DCDC_CONTROL_SECONDARY_UNUSED_15 15
#define DCDC_CONTROL_SBCM_SECONDARY_VOLTAGE_SECONDARY_CURRENT 32
#define DCDC_CONTROL_SBCM_SECONDARY_VOLTAGE_PRIMARY_CURRENT 33
#define DCDC_CONTROL_SBCM_PRIMARY_VOLTAGE_PRIMARY_CURRENT 34
#define DCDC_CONTROL_SBCM_PRIMARY_VOLTAGE_SECONDARY_CURRENT 35
#define DCDC_CONTROL_SBCM_SECONDARY_VOLTAGE_SECONDARY_POWER 36
#define DCDC_CONTROL_SBCM_SECONDARY_VOLTAGE_PRIMARY_POWER 37
#define DCDC_CONTROL_SBCM_PRIMARY_VOLTAGE_PRIMARY_POWER 38
#define DCDC_CONTROL_SBCM_PRIMARY_VOLTAGE_SECONDARY_POWER 39
#define DCDC_CONTROL_LAST_MODE 39

#define DCDC_STARTUP_SOFTSTART 0  // not supported
#define DCDC_STARTUP_UNIVERSAL 1
#define DCDC_STARTUP_UNUSED_2 2
#define DCDC_STARTUP_UNUSED_3 3
#define DCDC_STARTUP_UNUSED_4 4
#define DCDC_STARTUP_UNUSED_5 5
#define DCDC_STARTUP_RESONANT 6  // set frequency to resonant and go to run
#define DCDC_STARTUP_MAX_FREQUENCY 7  // set frequency to maximum_frequency_hz from control_config and go to run

/** Control mode union / structure for DCDC
 */
union u_control_mode {
    uint16_t all;
    struct {
        uint16_t control: 4;
        uint16_t startup: 3;
        uint16_t quick_startup_on_run_cmd: 1;
        uint16_t reverse_control: 4;
        uint16_t reverse_startup: 3;
        uint16_t reverse_quick_startup_on_run_cmd: 1;
    };
};

union u_curtailment_enable {
    uint32_t all;
    struct {
        // forward / primary -> secondary curtailment
        uint32_t forward_output_voltage: 1;
        uint32_t forward_input_voltage: 1;
        uint32_t forward_output_current: 1;
        uint32_t forward_input_current: 1;
        uint32_t forward_output_power: 1;
        uint32_t forward_input_power: 1;
        uint32_t forward_output_temperature: 1;
        uint32_t forward_input_temperature: 1;
        uint32_t forward_mcu_temperature: 1;
        uint32_t forward_output_voltage_voltage_control: 1;
        uint32_t forward_input_voltage_voltage_control: 1;
        uint32_t forward_output_voltage_passive: 1;
        uint32_t forward_dpdt: 1;
		uint32_t forward_output_under_voltage: 1;
        uint32_t forward_unused: 1; // expected to be forward_input_overvoltage
        uint32_t forward_transformer_temperature: 1; // added 5/12/2022
        // reverse / secondary -> primary curtailment
        uint32_t reverse_output_voltage: 1;
        uint32_t reverse_input_voltage: 1;
        uint32_t reverse_output_current: 1;
        uint32_t reverse_input_current: 1;
        uint32_t reverse_output_power: 1;
        uint32_t reverse_input_power: 1;
        uint32_t reverse_output_temperature: 1;
        uint32_t reverse_input_temperature: 1;
        uint32_t reverse_mcu_temperature: 1;
        uint32_t reverse_output_voltage_voltage_control: 1;
        uint32_t reverse_input_voltage_voltage_control: 1;
        uint32_t reverse_output_voltage_passive: 1;
        uint32_t reverse_dpdt: 1;
		uint32_t reverse_output_under_voltage: 1;
        uint32_t reverse_unused: 1; // expected to be reverse_input_overvoltage
        uint32_t reverse_transformer_temperature: 1; // added 5/12/2022
    };
};

union u_curtailment_options {
    uint16_t all;
    struct {
        // forward / primary -> secondary curtailment options
        uint16_t forward_output_voltage_power_mode: 1;  ///< if 1 then output voltage curtailment is based on power instead of curtailment
        uint16_t forward_input_voltage_power_mode: 1;  ///< if 1 then input voltage curtailment is based on power instead of curtailment
        uint16_t forward_output_voltage_unfiltered_mode: 1;  ///< use unfiltered measurement for output voltage curtailment
        uint16_t forward_input_voltage_unfiltered_mode: 1;  ///< use unfiltered measurement for input voltage curtailment
        uint16_t forward_output_current_unfiltered_mode: 1;  ///< use unfiltered measurement for output current curtailment
        uint16_t forward_input_current_unfiltered_mode: 1;  ///< use unfiltered measurement for input current curtailment
        uint16_t forward_input_voltage_auto_switch_direction_enable: 1;  ///< enable auto direction change when curtailment limit is reached
		uint16_t forward_output_voltage_auto_switch_direction_enable: 1;  ///< enable auto direction change when curtailment limit is reached
        // reverse / secondary -> primary curtailment options
        uint16_t reverse_output_voltage_power_mode: 1;  ///< if 1 then output voltage curtailment is based on power instead of curtailment
        uint16_t reverse_input_voltage_power_mode: 1;  ///< if 1 then input voltage curtailment is based on power instead of curtailment
        uint16_t reverse_output_voltage_unfiltered_mode: 1;  ///< use unfiltered measurement for output voltage curtailment
        uint16_t reverse_input_voltage_unfiltered_mode: 1;  ///< use unfiltered measurement for input voltage curtailment
        uint16_t reverse_output_current_unfiltered_mode: 1;  ///< use unfiltered measurement for output current curtailment
        uint16_t reverse_input_current_unfiltered_mode: 1;  ///< use unfiltered measurement for input current curtailment
        uint16_t reverse_input_voltage_auto_switch_direction_enable: 1;  ///< enable auto direction change when curtailment limit is reached
		uint16_t reverse_output_voltage_auto_switch_direction_enable: 1;  ///< enable auto direction change when curtailment limit is reached
    };
};

/** Configuration structure for DCDC board
*
* Max size supported is 1024 bytes.
*
* IMPORTANT:
* DCDCConfig_Write() in DCDCConfig.c must be updated when making any changes to this struct
*/
typedef struct {
    // Board information
    uint16_t config_version; // Must always be first element in struct
    uint16_t modbus_address; // Modbus slave ID, should be second element in struct (read only via Modbus)
    uint32_t board_revision; // read only via Modbus
    uint16_t comms_timeout_seconds; // Will trigger DCDC shutdown when the time between messages is exceeded
    uint16_t start_timeout_seconds; // The duration for which the unit will continue attempting to start before returning to a disabled state.
    uint32_t baud_rate; // WARNING MUST BE 12 BYTES FROM START OF CONFIG. Baud rate for UART
    uint16_t start_delay_seconds; // The duration for which the unit will wait to startup.
    uint8_t enable; // This DCDC board is enabled and allowed to engage in DCDC conversion. ( 2: Testmode, 1: Enabled, 0: Disabled)
    uint8_t always_bootup_in_shutdown_state; // Will cause the DCDC to shutdown on all resets
    
    // Absolute maximum values
    uint16_t primary_ovp_volts; // Over Voltage Protection (OVP) level for setting OVP_OUTPUT bit
    uint16_t secondary_ovp_volts; // Over Voltage Protection (OVP) level for setting OVP_OUTPUT bit
    float primary_ocp_amps; // Over Current Protection (OCP) level for setting OCP_OUTPUT bit
    float secondary_ocp_amps;
    float board_otp_deg_C; // shutdown on unfiltered temperature and set the flag
    float transistor_otp_deg_C; // shutdown on unfiltered temperature and set the flag
    
    // Voltage and temperature limits to enter startup
    uint16_t primary_startup_min_volts; // Minimum input voltage allowed to go to and stay in STARTUP
    uint16_t primary_startup_max_volts; // Maximum input voltage allowed to go to and stay in STARTUP
    uint16_t secondary_startup_min_volts; // Minimum output voltage allowed to go to and stay in STARTUP
    uint16_t secondary_startup_max_volts; // Maximum output voltage allowed to go to STARTUP
    uint16_t transistor_startup_max_deg_C; // Maximum transistor temperature allowed to go to STARTUP
    uint16_t board_startup_max_deg_C; // Maximum MCU temperature allowed to go to STARTUP
    
    // Current, voltage and temperature limits to stop running
    float primary_run_max_amps; // Input current limit for going to STANDBY
    float secondary_run_max_amps; // Output current limit for going to STANDBY
    uint16_t primary_run_min_volts; // Minimum input voltage the device can read before going to STANDBY
    uint16_t primary_run_max_volts; // Maximum input voltage the device can read before going to STANDBY
    uint16_t secondary_run_min_volts ; // Minimum output voltage the device can read before going to STANDBY
    uint16_t secondary_run_max_volts; // Maximum output voltage the device can read before going to STANDBY
    uint16_t transistor_run_max_deg_C; // Maximum transistor temperature the device can read before going to STANDBY
    uint16_t board_run_max_deg_C; // Maximum MCU temperature the device can read before going to STANDBY
    
    // Impedance limits
    float ground_fault_min_kohms; // UPDATE THIS DESCRIPTION
    
    // Control loop parameters
    uint32_t control_loop_short_period_ms; // Short period for whatever controls are being used
    uint32_t control_loop_long_period_ms; // Long period for whatever controls are being used
    
    // Setpoint limits
    float setpoint_charge_max_amps; // Max allowable current setpoint for charging battery
    float setpoint_discharge_max_amps; // Max allowable current setpoint for discharging battery
    uint16_t setpoint_charge_max_volts; // Max allowable voltage setpoint for charging battery
    uint16_t setpoint_discharge_min_volts; // Max allowable voltage setpoint for discharging battery
/// @todo reconsider setpoint limits

    // Universal startup parameters
    float forward_startup_max_energy_amps; // Current limit that should be exceeded when an inverter is in RUN
    uint16_t forward_startup_max_energy_timeout_ms; // Time above current limit before transitioning to RUN
    uint16_t forward_startup_max_target_volts; // Max output target voltage
    uint16_t forward_startup_min_target_volts; // Min output target voltage 
    uint16_t forward_startup_max_charge_volts; // High output voltage universal startup uses to stop overshoots when attempting to go to RUN
    float reverse_startup_max_energy_amps;
    uint16_t reverse_startup_max_energy_timeout_ms;
    uint16_t reverse_startup_max_target_volts;
    uint16_t reverse_startup_min_target_volts;
    uint16_t reverse_startup_max_charge_volts;
    float startup_overshoot_prevention_multiplier; // Percentage of target voltage that should not be exceeded during STARTUP (ex. voltage should not exceed 110% of target)
    
    // Unused parameters for development
    uint16_t mppt_small_step_hz;
    uint16_t mppt_large_step_hz;
    uint16_t forward_output_undervoltage_curtailment_start_volts;
    uint16_t forward_output_undervoltage_curtailment_end_volts;
    uint16_t reverse_output_undervoltage_curtailment_start_volts;
    uint16_t reverse_output_undervoltage_curtailment_end_volts;
    uint16_t leak_locator_enable;
    uint16_t estop_enable;
    uint16_t gate_driver_pulse_mode_enable;
    uint16_t forward_bus_charge_enable;
    
    // Other parameters
    union u_control_mode control_mode; // 16-bit
    uint16_t fan_turn_on_deg_C; // Disable an if 0, otherwise turn fan on when MCU temp > setpoint
    
    union u_curtailment_enable curtailment_enable; // 32-bit
    union u_curtailment_options curtailment_options; // 16-bit
    uint16_t forward_input_voltage_curtailment_start_volts;
    uint16_t forward_input_voltage_curtailment_end_volts;
    float forward_input_voltage_curtailment_start_amps;
    uint16_t forward_output_voltage_curtailment_start_volts;
    uint16_t forward_output_voltage_curtailment_end_volts;
    float forward_output_voltage_curtailment_start_amps;
    float forward_input_current_curtailment_amps;
    float forward_output_current_curtailment_amps;
    uint16_t forward_input_power_curtailment_watts;
    uint16_t forward_output_power_curtailment_watts;
    uint16_t forward_input_temperature_curtailment_start_deg_C;
    uint16_t forward_input_temperature_curtailment_end_deg_C;
    uint16_t forward_output_temperature_curtailment_start_deg_C; 
    uint16_t forward_output_temperature_curtailment_end_deg_C;
    uint16_t board_temperature_curtailment_start_deg_C;
    uint16_t board_temperature_curtailment_end_deg_C;
    float curtailment_min_current_amps;
    float dpdt_inversion_curtailment_start_watt_per_us;
    float dpdt_inversion_curtailment_end_watt_per_us;
    float dpdt_inversion_didt_curtailment_amps_per_second;
    uint16_t reverse_input_voltage_curtailment_start_volts;
    uint16_t reverse_input_voltage_curtailment_end_volts;
    float reverse_input_voltage_curtailment_start_amps;
    uint16_t reverse_output_voltage_curtailment_start_volts;
    uint16_t reverse_output_voltage_curtailment_end_volts;
    float reverse_output_voltage_curtailment_start_amps;
    float reverse_input_current_curtailment_amps;
    float reverse_output_current_curtailment_amps;
    uint16_t reverse_input_power_curtailment_watts;
    uint16_t reverse_output_power_curtailment_watts;
    uint16_t reverse_input_temperature_curtailment_start_deg_C;
    uint16_t reverse_input_temperature_curtailment_end_deg_C;
    uint16_t reverse_output_temperature_curtailment_start_deg_C;
    uint16_t reverse_output_temperature_curtailment_end_deg_C;
} dcdc_config_t;

typedef struct {
    uint32_t minimum_frequency_hz;
    uint32_t maximum_frequency_hz;
    uint16_t minimum_step_hz;
    uint16_t maximum_step_hz;
    float high_current_frequency_limit_amps;
    uint32_t high_current_frequency_limit_hz;
    uint32_t minimum_stuck_frequency_hz;
    uint32_t maximum_stuck_frequency_hz;
    uint32_t stuck_time_limit_ms;
    float minimum_search_good_current_amps;
    uint32_t minimum_search_good_voltage_offset; ///< if input voltage drops below "run_min_volts - minimum_search_good_voltage_offset" then seach is good.
}control_config_t;

typedef struct {
    uint32_t resonant_frequency_hz;
    float transformer_ratio;
}resonant_circuit_data_t;

typedef struct {
    uint16_t frequency_scale_factor_scalar;
    uint16_t charge_to_run_scale_factor;
    uint16_t precharge_scale_factor;
    uint16_t enter_voltage_follower_percent;
    uint16_t precharge_max_dvdt;
    uint16_t average_update_period;
}universal_startup_t;

typedef struct {
    uint16_t minimum_dfdi;
    uint16_t update_alpha_percent;
    uint16_t update_radius_percent;
    uint16_t initial_frequency_lookup;
    uint16_t update_stabilization_time_ms;
}smart_constant_current_t;

typedef struct {
    uint16_t curtailment_start;
    uint16_t curtailment_end;
    uint16_t run_limit;
    uint16_t startup_limit;
}transformer_temperature_curtailment_t;

typedef struct {
    float Lm;
    float Lr;
    float steinmetz_k;
    float steinmetz_a;
    float steinmetz_b;
    float core_volume;
    float dc_resistance_primary;
    float dc_resistance_secondary;
    uint16_t primary_turns;
    uint16_t secondary_turns;
    float k_straight_acr_primary;  /// k_straight_acr * primary_length ^ 3 (power of 3)
    float k_straight_acr_secondary;  /// k_straight_acr * secondary_length ^ 3
    float k_wound_acr_primary;  /// k_wound_acr * primary_turns ^ 2
    float k_wound_acr_secondary;  /// k_wound_acr * secondary_turns ^ 2
    float thermal_resistance;
    float thermal_entropy;
} transformer_parameters_t;

/// @todo determine if the method of estimating flux for Tesla works at a broader scope of if the equations need to be restructured
typedef struct {
    float a_p3;
    float a_p2;
    float a_p1;
    float a_p0;
    float b_p3;
    float b_p2;
    float b_p1;
    float b_p0;
    float c_p1;
    float c_p0;
    float c_v_secondary_normalized;
    float i_pri_p00;
    float i_pri_p10;
    float i_pri_p01;
    float i_pri_p20;
    float i_pri_p11;
    float i_pri_p02;
    float i_sec_p00;
    float i_sec_p10;
    float i_sec_p01;
    float i_sec_p20;
    float i_sec_p11;
    float i_sec_p02;
} transformer_flux_density_parameters_t;

/* TRANSFORMER_FLUX_DENSITY_PARAMETERS
TBD
*/

/* Defines the application specific user-defined Miwi message types.
These are constants instead of enum because explicit values were chosen
in order to make them maskable.
*/
#define SPOT_COMMAND_MSG 1

#define SPOT_STATUS_MSG 2

#define SPOT_QUERY_DEVICES_MSG 4

#define SPOT_FW_UPDATE_MSG 8
//used for bootloader/firmware update msgs
#define SPOT_DIAGNOSTIC_REPORT_MSG 16
//used for the spot debug model report
//#define SPOT_FFT_REPORT_MSG 32 no longer in use
//used for the spot/dcdc configuration
#define SPOT_CONFIG_DATA_MSG 64
//used for the spot fft report
#define SPOT_TEST_PATTERN_MSG 4096
//used for miwi comms test pattern
#define SPOT_LOG_MSG 32768
// used for testing and diagnostics

/** Sensor Calibration NVM Object */
struct sensor_calibration_s {
    float primary_voltage_offset;
    float primary_voltage_gain;
    float secondary_voltage_offset;
    float secondary_voltage_gain;
    float primary_current_offset;
    float primary_current_gain;
    float secondary_current_offset;
    float secondary_current_gain;
    uint16_t primary_B;
    uint16_t primary_R0;
    uint16_t secondary_B;
    uint16_t secondary_R0;
};

///////////////////// CUBE TYPES /////////////////////////////

/** Configuration structure for CUBE board
*
* Max size supported is 1024 bytes.
*
* IMPORTANT:
* DCDCConfig_Write() in DCDCConfig.c must be updated when making any changes to this struct
*/
typedef struct {
    // Board information
    uint16_t config_version; // Must always be first element in struct
    uint16_t modbus_address; // Modbus slave ID, should be second element in struct (read only via Modbus)
    uint32_t board_revision; // read only via Modbus
    uint16_t comms_timeout_seconds; // Will trigger DCDC shutdown when the time between messages is exceeded
    uint16_t start_timeout_seconds; // The duration for which the unit will continue attempting to start before returning to a disabled state.
    uint32_t baud_rate; // WARNING MUST BE 12 BYTES FROM START OF CONFIG. Baud rate for UART
    uint16_t start_delay_seconds; // The duration for which the unit will wait to startup.
    uint8_t enable; // This DCDC board is enabled and allowed to engage in DCDC conversion. ( 2: Testmode, 1: Enabled, 0: Disabled)
    uint8_t always_bootup_in_shutdown_state; // Will cause the DCDC to shutdown on all resets
    
    // Absolute maximum values
    uint16_t primary_ovp_volts; // Over Voltage Protection (OVP) level for setting OVP_OUTPUT bit
    uint16_t secondary_ovp_volts; // Over Voltage Protection (OVP) level for setting OVP_OUTPUT bit
    float primary_ocp_amps; // Over Current Protection (OCP) level for setting OCP_OUTPUT bit
    float secondary_ocp_amps;
    float board_otp_deg_C; // shutdown on unfiltered temperature and set the flag
    float transistor_otp_deg_C; // shutdown on unfiltered temperature and set the flag
    
    // Voltage and temperature limits to enter startup
    uint16_t primary_startup_min_volts; // Minimum input voltage allowed to go to and stay in STARTUP
    uint16_t primary_startup_max_volts; // Maximum input voltage allowed to go to and stay in STARTUP
    uint16_t secondary_startup_min_volts; // Minimum output voltage allowed to go to and stay in STARTUP
    uint16_t secondary_startup_max_volts; // Maximum output voltage allowed to go to STARTUP
    uint16_t transistor_startup_max_deg_C; // Maximum transistor temperature allowed to go to STARTUP
    uint16_t board_startup_max_deg_C; // Maximum MCU temperature allowed to go to STARTUP
    
    // Current, voltage and temperature limits to stop running
    float primary_run_max_amps; // Input current limit for going to STANDBY
    float secondary_run_max_amps; // Output current limit for going to STANDBY
    uint16_t primary_run_min_volts; // Minimum input voltage the device can read before going to STANDBY
    uint16_t primary_run_max_volts; // Maximum input voltage the device can read before going to STANDBY
    uint16_t secondary_run_min_volts ; // Minimum output voltage the device can read before going to STANDBY
    uint16_t secondary_run_max_volts; // Maximum output voltage the device can read before going to STANDBY
    uint16_t transistor_run_max_deg_C; // Maximum transistor temperature the device can read before going to STANDBY
    uint16_t board_run_max_deg_C; // Maximum MCU temperature the device can read before going to STANDBY
    
    // Control loop parameters
    uint32_t control_loop_short_period_ms; // Short period for whatever controls are being used
    uint32_t control_loop_long_period_ms; // Long period for whatever controls are being used

    // Setpoint limits
    float setpoint_charge_max_amps; // Max allowable current setpoint for charging battery
    float setpoint_discharge_max_amps; // Max allowable current setpoint for discharging battery
    uint16_t setpoint_charge_max_volts; // Max allowable voltage setpoint for charging battery
    uint16_t setpoint_discharge_min_volts; // Max allowable voltage setpoint for discharging battery
    /// @todo reconsider setpoint limits

    // Universal startup parameters
    float forward_startup_max_energy_amps; // Current limit that should be exceeded when an inverter is in RUN
    uint16_t forward_startup_max_energy_timeout_ms; // Time above current limit before transitioning to RUN
    uint16_t forward_startup_max_target_volts; // Max output target voltage
    uint16_t forward_startup_min_target_volts; // Min output target voltage 
    uint16_t forward_startup_max_charge_volts; // High output voltage universal startup uses to stop overshoots when attempting to go to RUN
    float reverse_startup_max_energy_amps;
    uint16_t reverse_startup_max_energy_timeout_ms;
    uint16_t reverse_startup_max_target_volts;
    uint16_t reverse_startup_min_target_volts;
    uint16_t reverse_startup_max_charge_volts;
    float startup_overshoot_prevention_multiplier; // Percentage of target voltage that should not be exceeded during STARTUP (ex. voltage should not exceed 110% of target)
    
    // Other parameters
    union u_control_mode control_mode; // 16-bit
    uint16_t fan_turn_on_deg_C; // Disable an if 0, otherwise turn fan on when MCU temp > setpoint
    
    union u_curtailment_enable curtailment_enable; // 32-bit
    union u_curtailment_options curtailment_options; // 16-bit
    uint16_t forward_input_voltage_curtailment_start_volts;
    uint16_t forward_input_voltage_curtailment_end_volts;
    float forward_input_voltage_curtailment_start_amps;
    uint16_t forward_output_voltage_curtailment_start_volts;
    uint16_t forward_output_voltage_curtailment_end_volts;
    float forward_output_voltage_curtailment_start_amps;
    float forward_input_current_curtailment_amps;
    float forward_output_current_curtailment_amps;
    uint16_t forward_input_power_curtailment_watts;
    uint16_t forward_output_power_curtailment_watts;
    uint16_t forward_input_temperature_curtailment_start_deg_C;
    uint16_t forward_input_temperature_curtailment_end_deg_C;
    uint16_t forward_output_temperature_curtailment_start_deg_C; 
    uint16_t forward_output_temperature_curtailment_end_deg_C;
    uint16_t board_temperature_curtailment_start_deg_C;
    uint16_t board_temperature_curtailment_end_deg_C;
    float curtailment_min_current_amps;
    float dpdt_inversion_curtailment_start_watt_per_us;
    float dpdt_inversion_curtailment_end_watt_per_us;
    float dpdt_inversion_didt_curtailment_amps_per_second;
    uint16_t reverse_input_voltage_curtailment_start_volts;
    uint16_t reverse_input_voltage_curtailment_end_volts;
    float reverse_input_voltage_curtailment_start_amps;
    uint16_t reverse_output_voltage_curtailment_start_volts;
    uint16_t reverse_output_voltage_curtailment_end_volts;
    float reverse_output_voltage_curtailment_start_amps;
    float reverse_input_current_curtailment_amps;
    float reverse_output_current_curtailment_amps;
    uint16_t reverse_input_power_curtailment_watts;
    uint16_t reverse_output_power_curtailment_watts;
    uint16_t reverse_input_temperature_curtailment_start_deg_C;
    uint16_t reverse_input_temperature_curtailment_end_deg_C;
    uint16_t reverse_output_temperature_curtailment_start_deg_C;
    uint16_t reverse_output_temperature_curtailment_end_deg_C;
} cube_config_t;

typedef struct {
    uint32_t minimum_pulse_width_ns;
    uint32_t maximum_pulse_width_ns;
    uint16_t minimum_step_ns;
    uint16_t maximum_step_ns;
    uint32_t default_frequency_hz;
    uint32_t default_active_diode_delay_ns;
    float minimum_search_good_current_amps;
    uint32_t minimum_search_good_voltage_offset; ///< if input voltage drops below "run_min_volts - minimum_search_good_voltage_offset" then seach is good.
} cube_control_config_t;

typedef struct {
    float input_current_max;
    float output_current_max;
    float input_voltage_max;
    float input_voltage_min;
    float output_voltage_max;
    float output_voltage_min;
    float power_max;
    float manual_max;  ///< not used yet
    float manual_min;  ///< not used yet
} setpoint_limit_set_t;

typedef struct {
    setpoint_limit_set_t forward;
    setpoint_limit_set_t reverse;
} setpoint_limits_t;

typedef struct {
    float input_current;  ///< amps / s
    float output_current;  ///< amps / s
    float input_voltage;  ///< volts / s
    float output_voltage;  ///< volts / s
    float power;  ///< watts / s
    float manual;  ///< arbitrary / s
} slew_rates_t;

/** slew rates for different modes and different directions and whether the setpoint is
 * increasing or decreasing in magnitude.
 * 
 * All slew rates should be represented as positive values
 */
typedef struct {
    slew_rates_t forward_increase;
    slew_rates_t forward_decrease;
    slew_rates_t reverse_increase;
    slew_rates_t reverse_decrease;
} setpoint_slew_rates_t;

enum product_code_e {
    PRODUCT_CODE_SPOT = 1,
    PRODUCT_CODE_BOSS_CLLC,
    PRODUCT_CODE_GARD,
    PRODUCT_CODE_AFD,
    PRODUCT_CODE_BOSS_DAB,
    PRODUCT_CODE_CUBE  // changed from 4 to 6 on 8/31/2022
};

/* END OF FILE */

#endif
