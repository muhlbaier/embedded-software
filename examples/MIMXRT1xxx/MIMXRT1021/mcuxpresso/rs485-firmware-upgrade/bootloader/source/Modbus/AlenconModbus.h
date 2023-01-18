#ifndef _ALENCON_MODBUS_H_
#define _ALENCON_MODBUS_H_

#include <stdint.h>
#include "mcuid.h"
#include "DCDCModel.h"

void vMBPortTimersService( void );

#define MB_DEFAULT_ADDRESS 1
//#define MB_DEFAULT_ADDRESS 247


#define MB_PORT_SERVICE_TIMERS() vMBPortTimersService()
#define MB_RTU_RECEIVE_FSM() xMBRTUReceiveFSM()

#define MODBUS_MAX_INPUT_SOURCES 16
#define MODBUS_MAX_HOLDING_REGISTERS 64

// note FreeModbus auto increments the received address so 0 becomes 1
#define MODBUS_HOLDING_REGISTERS_BASE   40000
#define MODBUS_INPUT_BASE               30000

enum sunspec_model_id {
    SUNSPEC_COMMON_ID = 1,
    SUNSPEC_SOLAR_MODULE_ID = 501,
    SUNSPEC_AGGRAGATE_INFORMATION_MODEL = 65001,
    SUNSPEC_DCDC_AGGRAGATE_DCDC_CONVERTER_ID = 65501,
    SUNSPEC_DCDC_AGGRAGEGE_DCDC_CONTROL_ID = 65503,
    SUNSPEC_DCDC_CONTROL_ID = 65503, // SUNSPEC_DCDC_AGGRAGEGE_DCDC_CONTROL with N = 0
    SUNSPEC_DCDC_EXTENDED_TELEMETRY_ID = 65504,
    SUNSPEC_END_OF_MAP_ID = 65535
};

enum sunspec_common_index {
    COMMON_ID = 0,
    COMMON_LENGTH,
    COMMON_DATA_BLOCK, // this represents the entire data block
    COMMON_MANUFACTURE = 2,
    COMMON_MODEL = 18,
    COMMON_OPTIONS = 34,
    COMMON_VERSION = 42,
    COMMON_SERIAL_NUMBER = 50,
    COMMON_DEVICE_ADDRESS = 66,
    COMMON_PADDING
};

enum sunspec_solar_module_index {
    SOLAR_MODULE_ID = 0,
    SOLAR_MODULE_LENGTH,
    SOLAR_MODULE_STATUS,
    SOLAR_MODULE_VENDOR_STATUS,
    SOLAR_MODULE_EVENTS,
    SOLAR_MODULE_VENDOR_EVENTS = 6,
    SOLAR_MODULE_CONTROL = 8,
    SOLAR_MODULE_VENDOR_CONTROL,
    SOLAR_MODULE_CONTROL_VALUE = 11,
    SOLAR_MODULE_TIMESTAMP = 13,
    SOLAR_MODULE_OUTPUT_CURRENT = 15,
    SOLAR_MODULE_OUTPUT_VOLTAGE = 17,
    SOLAR_MODULE_OUTPUT_ENERGY = 19,
    SOLAR_MODULE_OUTPUT_POWER = 21,
    SOLAR_MODULE_TEMPERATURE = 23,
    SOLAR_MODULE_INPUT_CURRENT = 25,
    SOLAR_MODULE_INPUT_VOLTAGE = 27,
    SOLAR_MODULE_INPUT_ENERGY = 29,
    SOLAR_MODULE_INPUT_POWER = 31
};

enum sunspec_dcdc_extended_telemetry_index {
    EXTENDED_TELEMETRY_ID = 0,
    EXTENDED_TELEMETRY_LENGTH,
    EXTENDED_TELEMETRY_MODULE_1_TEMPERATURE = 2,
    EXTENDED_TELEMETRY_PRIMARY_TEMPERATURE = 2,
    EXTENDED_TELEMETRY_MODULE_2_TEMPERATURE = 4,
    EXTENDED_TELEMETRY_SECONDARY_TEMPERATURE = 4,
    EXTENDED_TELEMETRY_MCU_TEMPERATURE = 6,
    EXTENDED_TELEMETRY_BOARD_TEMPERATURE = 8,
    EXTENDED_TELEMETRY_HUMIDITY = 10,
    EXTENDED_TELEMETRY_LEAK_LOCATION,
    EXTENDED_TELEMETRY_LEAK_IMPEDANCE,
    EXTENDED_TELEMETRY_3V3 = 14,
    EXTENDED_TELEMETRY_5V,
    EXTENDED_TELEMETRY_LOG_MESSAGES,
    EXTENDED_TELEMETRY_MCU_TIME,
    EXTENDED_TELEMETRY_RESERVED = 19,
    EXTENDED_TELEMETRY_N = 27
};

enum sunspec_solar_module_status {
    SOLAR_MODULE_STATUS_OFF = 1,
    SOLAR_MODULE_STATUS_SLEEPING,
    SOLAR_MODULE_STATUS_STARTING,
    SOLAR_MODULE_STATUS_MPPT,
    SOLAR_MODULE_STATUS_THROTTLED,
    SOLAR_MODULE_STATUS_SHUTTING_DOWN,
    SOLAR_MODULE_STATUS_FAULT,
    SOLAR_MODULE_STATUS_STANDBY,
    SOLAR_MODULE_STATUS_TEST,
    SOLAR_MODULE_STATUS_OTHER
};

typedef enum
{
    mbRtu_enc_reg_1, // 40092
    mbRtu_enc_reg_2, // 40093
    mbRtu_enc_reg_3, // 40094
    mbRtu_enc_reg_4, // 40095
    mbRtu_enc_reg_5, // 40096
    mbRtu_enc_reg_6, // 40097
    mbRtu_enc_reg_7, // 40098
    mbRtu_enc_reg_8, // 40099
    mbRtu_command, // 40100
    mbRtu_status, // 40101
    mbRtu_primary_error_code_high, // 40102
    mbRtu_primary_error_code_low, // 40103
    mbRtu_input_voltage, // 40104
    mbRtu_input_current, // 40105
    mbRtu_output_voltage, // 40106
    mbRtu_output_current, // 40107
    mbRtu_transistor_temperature, // 40108
    mbRtu_onboard_temperature, // 40109
    mbRtu_ground_leak_current, // 40110
    mbRtu_dcdc_gate_freq_hz, // 40111, removed in v18
    mbRtu_aux_12volts, // 40112, removed in v18
    mbRtu_groundfault_resistance, // 40113
    mbRtu_groundfault_location, // 40114
    mbRtu_battery_temperature, // 40115
    mbRtu_humidity_sensor_temperature, // 40116
    mbRtu_humidity, // 40117
    mbRtu_time_high, // 40118
    mbRtu_time_low, // 40119
    mbRtu_timestamp_high, // 40120
    mbRtu_timestamp_low, // 40121
    mbRtu_secondary_error_code_high, // 40122
    mbRtu_secondary_error_code_low, // 40123
    mbRtu_number_messages, // 40124
	mbRtu_primary_case_temperature, // 40125
	mbRtu_secondary_case_temperature, // 40126
	mbRtu_primary_ambient_temperature, // 40127
	mbRtu_secondary_ambient_temperature, // 40128
    mbRtu_transformer_temperature // 40129
}mbRtuTelemetryIndex_t;

typedef enum
{
    mbRtu_id, // 40200
    mbRtu_len, // 40201
    mbRtu_control_mode, // 40202
    mbRtu_direction, // 40203
    mbRtu_setpoint, // 40204
    mbRtu_setpoint_high = 4, // 40204
    mbRtu_setpoint_low, // 40205
    mbRtu_time_since, // 40206
    mbRtu_time_out, // 40207
    mbRtu_reserved_1, // 40207
    mbRtu_reserved_2, // 40208
    mbRtu_reserved_3, // 40209
    mbRtu_reserved_4, // 40210
    mbRtu_reserved_5, // 40211
    mbRtu_reserved_6, // 40212
    mbRtu_reserved_7, // 40213
    mbRtu_reserved_8, // 40214
    mbRtu_reserved_9, // 40215
    mbRtu_reserved_10, // 40216
    mbRtu_reserved_11 // 40217
}mbRtuControlIndex_t;

// #define MODBUS_TEST 1

void Modbus_Routine(void);

void Modbus_AddInputSource(uint16_t address, uint16_t * source);

// address - can only be accessed by this address
// holding_reg - pointer to base of this register sequence
// write_callback - to be called when written with pointer to each holding register written and the address
// read_callback - to be called when read, once per read regardless of address and length
// length_limit - maximum number of registers that can be read or written from this base address
void Modbus_AddHoldingRegister(uint16_t address, uint16_t * holding_reg, void(*callback)(uint16_t *, uint16_t), void(*read_callback)(void), uint16_t length_limit);

void AlenconModbus_Init(void);
void AlenconModbus_Stop(void);
// return 1 to jump to user app, 0 to stay in bootloader
uint8_t AlenconModbus_InitBootloader(void);
//void AlenconModbus_BindDCDCModel(dcdc_model_handle_t model);

void Modbus_ChangeSlaveAddress(uint16_t address);
uint8_t Modbus_GetSlaveAddress(void);
void Modbus_SetBaudRate(uint32_t baud_rate);

void Modbus_Reset(uint16_t address);

// functions from spot_logging_dcdc.c needed for modbus log messages
uint16_t SPOT_Log_PendingMessageCount(void);
void SPOT_Log_LoadModbusMessage(uint16_t * registers);

// helper functions to convert data types to registers
void Modbus_LongToRegisters(uint32_t value, uint16_t * register_ptr);
void Modbus_FloatToRegisters(float value, uint16_t * register_ptr);
float Modbus_RegistersToFloat(uint16_t * register_ptr);

#endif
