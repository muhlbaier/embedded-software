/** DCDCBoard
*	@addtogroup DCDCBoard
*	@{
*/

/** hardware
*	@addtogroup hardware
*	@{
*   
*   @brief Board independent hardware functions
*
*   pinout.h must include the following definitions:
#define HARDWARE_ID_1 x
#define HARDWARE_ID_2 x
#define HARDWARE_ID_3 x
#define HARDWARE_ID_PORT PORTx
#define HARDWARE_ID_GPIO PTx
*
*   Current implementation only supports Hardware ID pins on the same port.
*/

/* ***** Inclusion Gaurd ****** */
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <stdint.h>

enum board_hw_id {
    HW_ID_GARD_2_0 = 0,
    HW_ID_SPOT600_4_X = 0, // not supported
    HW_ID_CUBE_0_0 = 1,
    HW_ID_SPOT600_3_X = 1, // not supported
    HW_ID_GARD_2_1 = 1, 
    HW_ID_SPOT1500_5_0 = 2, // not supported
    HW_ID_CUBE_1_0 = 2,
    HW_ID_GARD_2_2 = 2,
    // HW_ID_SPOT1500_6_0 = 3, // not supported anymore
    HW_ID_CUBE_1_0_ALT = 3,
    HW_ID_SPOT1500_6_2 = 3,
    HW_ID_GARD_2_2_1 = 3,
    HW_ID_DAB_7_0 = 4,
    HW_ID_SPOT_6_6_HO25 = 5,
    HW_ID_SPOT_7_0 = 6,
    HW_ID_BUCK_BOOST_7_1 = 7,
    HW_ID_SPOT_6_5 = 8,
    HW_ID_DAB_6_1 = 9,
    HW_ID_SPOT600_6_2 = 10,
    HW_ID_SPOT1500_6_1 = 11,
    HW_ID_SPOT600_6_1 = 12,
    HW_ID_BOSS_1_0 = 13,
    HW_ID_BOSS_1_1 = 14,
    HW_ID_BOSS_7_0 = 15,
    HW_ID_DCDC_7_1 = 16,
    HW_ID_DCDC_7_2 = 25
};

enum hardware_cpu_id {
    HW_CPU_ID_UNDEFINED = 0,
    HW_CPU_ID_MKV31 = 31,
    HW_CPU_ID_MKV56 = 56,
    HW_CPU_ID_MKV58 = 58,
    HW_CPU_ID_RT1021 = 121,
};

/** @brief maping of MCU part numbers to combined MCU codes
 */
enum hardware_mcu_combined {
    HW_MCU_MKV31F256VLH12 = 0x851Fu, ///< standard for AFD
    HW_MCU_MKV31F256VLL12 = 0x881Fu, ///< standard for SPOT / BOSS / CUBE / GARD
    HW_MCU_MKV31F512VLL12 = 0x981Fu,
    HW_MCU_MKV56F512VLL24 = 0x9838u,
    HW_MCU_MKV58F512VLL24 = 0x983Au,
    HW_MCU_MKV56F1M0VLL24 = 0xA838u,
    HW_MCU_MKV58F1M0VLL24 = 0xA83Au  ///< original new MCU for SPOT / BOSS
};

//#pragma anon_unions
typedef struct {
    enum hardware_cpu_id cpu_id;
    uint8_t pin_count;
    uint8_t flash_kb_exp; // flash size bytes = 2^(flash_kb_exp + 10)
    uint8_t rev_id;
    union {
        uint16_t mcu_code;
        struct {
            uint16_t cpu_id: 8;
            uint16_t pin_count_code: 4;
            uint16_t flash_kb_exp: 4;
        };
    } combined;
} hardware_mcu_info_t;


/** @brief Initialize the clocksarm core and return the hardware version */
enum board_hw_id hardware_init(void);

uint64_t Hardware_GetGUID(void);

hardware_mcu_info_t * Hardware_GetMCU(void);

/** @brief Gets the hardware ID (0-9 value)
 *
 * ID value is based on two ID pins. Each pin can either be not connected (NC)
 * connected to GND via a pull down resistor or connected to VCC via a pull up resistor.
 * The ID has the following relationship to the ID pins:
 * ID - Pin1  Pin2  Pin3  Board Version
 * 0  - NC    NC    NC    GARD 2.0 / DCDC 600/1000 4.x Not supported by this firmware
 * 1  - GND   NC    NC    GARD 2.1 / CUBE 0.0 / DCDC 600/1000 3.0 Not supported by this firmware
 * 2  - NC    GND   NC    CUBE 1.0 / GARD 2.2 / DCDC 1500 5.0 (not supported)
 * 3  - GND   GND   NC    SPOT 1500     6.2 (also 6.0 but none of those were deployed) / GARD 2.2.1
 * 4  - VCC   NC    NC    BOSS DAB      7.0
 * 5  - NC    VCC   NC    SPOT 1000		6.6.2 HO25 LEMs
 * 6  - VCC   VCC   NC    SPOT     		7.0
 * 7  - VCC   GND   NC    BUCK BOOST    7.1
 * 8  - GND   VCC   NC    SPOT 1000		6.5/6.6/6.6.2 HO15 LEMs
 * 9  - NC    NC   GND    BOSS DAB 1500 6.1 (DAB BOSS 1000 1.1 / DAB BOSS 1000 1.2)
 * 10 - GND   NC   GND    SPOT 600/1000 6.2
 * 11 - NC    GND  GND    SPOT 1500 	6.1
 * 12 - GND   GND  GND    SPOT 600/1000 6.1
 * 13 - VCC   NC   GND    BOSS CLLC     1.0
 * 14 - NC    VCC  GND    BOSS CLLC     1.1 (BOSS 1500 6.1 / BOSS 1000 1.1 / BOSS 1000 1.2)
 * 15 - VCC   VCC  GND    BOSS CLLC     7.0
 * 16 - VCC   GND  GND    SPOT/BOSS DAB 7.1 (SPOT can autodetect via driver presence)
 * 17 - GND   VCC  GND
 * 18 - NC    NC   VCC    SPOT/BOSS     7.3
 * 19 - GND   NC   VCC
 * 20 - NC    GND  VCC
 * 21 - GND   GND  VCC
 * 22 - VCC   NC   VCC
 * 23 - NC    VCC  VCC
 * 24 - VCC   VCC  VCC
 * 25 - VCC   GND  VCC    SPOT/BOSS     7.2 
 * 26 - GND   VCC  VCC
 */
enum board_hw_id Hardware_GetID(void);

// check for certain hardware features
/** @brief Check if the secondary gate driver board is installed
 * 
 * On the first call the function will (assuming a compatible HW ID) 
 * turn the enable output into an input with a pull down resistor to
 * check if the signal remains high due to the pull up on the gate driver
 * daughter board. On all subsequent calls the function returns the result
 * of the initial check.
 *  
 * This function should be called before turning on 24V power to
 * gate drivers as it will turn on a pull down resistor that could
 * enable the gate driver.
 *
 * @return 0 if no secondary gate driver board detected or if the main board
 *         doesn't support gate driver baords. 1 if board detected.
 */
uint8_t Hardware_HasSecondaryDriverBoard(void);

uint8_t Hardware_HasHumiditySensor(void);
uint8_t Hardware_HasOVP_Internal_Comparator(void);
uint8_t Hardware_HasSecondOVP(void);
uint8_t Hardware_HasSpotV6PinoutCMP(void);
uint8_t Hardware_HasDiagnosticUART(void);
uint8_t Hardware_HasBatteryGateFaultDetection(void);
uint8_t Hardware_HasBidirectionalDCDC(void);
uint8_t Hardware_HasExternalFan(void);
uint8_t Hardware_HasSecondExternalFan(void);
uint8_t Hardware_HasNewGateDriver(void);
uint8_t Hardware_HasDabTopology(void);
uint8_t Hardware_HasGroundFaultDetector(void);
uint8_t Hardware_HasLatestVoltageGain(void);
uint8_t Hardware_HasHO25CurrentSensor(void);
uint8_t Hardware_HasV3ADC0(void);
uint8_t Hardware_HasV4ADC0(void);
uint8_t Hardware_Has1_65VrefOnPTC0(void);
uint8_t Hardware_HasRJ45_Shutdown(void);
uint8_t Hardware_HasVincotechThermistor(void);
uint8_t Hardware_HasAuxUVP(void);
uint8_t Hardware_HasTransformerTemperatureSensor(void);
uint8_t Hardware_HasBuckboostTopology(void);
uint8_t Hardware_HasFaultyZeroCurrentOffset(void);
uint8_t Hardware_HasMKV5Pinout(void);
uint8_t Hardware_HasResonantTopology(void);

#ifdef GARD_BOARD
uint8_t Hardware_HasOldEmergencySwitchDetect(void);
uint8_t Hardware_HasOldExternalFaultClear(void);
uint8_t Hardware_HasLED_GPIO_Expander(void);
uint8_t Hardware_HasAltDebugUART(void);
uint8_t Hardware_HasDifferentialCurrentSensors(void);
uint8_t HardwareHasLeakLocatorReference(void);
#endif

#ifdef CUBE_BOARD
uint8_t Hardware_HasDoubleEndedCurrentSensor(void);
#endif

/** @brief HAL Function to initialize clock to run mode. */
void HAL_Hardware_ClockRunInit(void);

/** @brief HAL Function to enable clocks for GPIO ports. */
void HAL_Hardware_EnablePortClocks(void);

#endif // _HARDWARE_H_

/** hardware
*	@}
*/

/** DCDCBoard
*	@}
*/
