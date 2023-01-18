/**
*	@file hardware.c
*	@brief
*	@date 12/3/2021
*   @author Michael Muhlbaier
*
*  Copyright (c) 2022, Alencon Systems
* All rights reserved.
*
*/

#include "hardware.h"
#ifdef DCDC_BOARD
#ifdef CUBE_BOARD
#include "cube_pinout.h"
#else
//#include "pinout.h"  // includes register definitions for the selected CPU
#endif
#include "pin_mux.h"
#elif defined GARD_BOARD
#include "gard_pinout.h"  // includes register definitions for the selected CPU
#include "gard_pin_mux.h"
#else
#error "invalid board"
#endif
////#include "fsl_edma.h"
//#include "fsl_edma_driver.h"
//#include "int_def.h"
#include "string.h"
#include "timing.h"
#include "mcuid.h"


//static edma_state_t edmaState;

enum board_hw_id hardware_init(void) {
    enum board_hw_id hw_id;
    
    ///* Enable clock/power for io ports */
    //HAL_Hardware_EnablePortClocks();

    hw_id = Hardware_GetID();
    
    ///* Set up pin-muxing for board */
    //configure_gpio_pins();
    //configure_pwm_pins();
    //configure_i2c_pins();
    //configure_uart_pins();
    //configure_spi_pins();
    //configure_adc_pins();
    
    ///* Init board clock */
    //HAL_Hardware_ClockRunInit(); // 80Mhz from 8Mhz crystal

    ///*Init and configure the EDMA controller*/
    ///*edma_config_t edma_config = {
        //.enableContinuousLinkMode = true,
        //.enableHaltOnError = false,
        //.enableRoundRobinArbitration = true,
        //.enableDebugMode = false
    //};
    //EDMA_Init(DMA0, &edma_config);*/
    //edma_user_config_t edma_config = {
        //.chnArbitration     = kEDMAChnArbitrationRoundrobin, 
//#if defined(FSL_FEATURE_EDMA_CHANNEL_GROUP_COUNT) && (FSL_FEATURE_EDMA_CHANNEL_GROUP_COUNT > 1)
        //.groupArbitration   = kEDMAGroupArbitrationFixedPriority,
        //.groupPriority      = kEDMAGroup0PriorityHighGroup1PriorityLow, //kEDMAGroup0PriorityLowGroup1PriorityHigh,
//#endif
        //.notHaltOnError     = true//= false 
    //};
    //EDMA_DRV_Init(&edmaState, &edma_config);
    
    //// configure all unused pins to be output and have logic low out
    //configure_unused_pins();
    return hw_id;
}

uint64_t Hardware_GetGUID(void) {
	//uint32_t uid[4];
	union64_t guid;
	
    //memcpy(uid, (uint32_t*)(0x40048054), 16);
    //guid.double_word[0] = uid[0] ^ uid[2];
	//guid.double_word[1] = uid[1] ^ uid[3];
	
	guid.double_word[0] = *(uint32_t *)(DEVICE_ID_REG_ADDR);
	guid.double_word[1] = *(uint32_t *)(DEVICE_ID_REG_ADDR + 0x10);
	
#define FAKE_GUID_FORCED	
#ifdef FAKE_GUID_FORCED
	//guid.double_word[0] = 0x29564E63;
	//guid.double_word[1] = 0x001A9F4E;
	//guid.double_word[0] = 0x4E632956;
	//guid.double_word[1] = 0x9F4E001A;
	guid.double_word[0] = 0x001A9F4E;
	guid.double_word[1] = 0x29564E63;
	//guid.double_word[0] = 0x9F4E001A;
	//guid.double_word[1] = 0x4E632956;
#endif	

	return guid.quad_word;
}

hardware_mcu_info_t * Hardware_GetMCU(void) {
    static hardware_mcu_info_t mcu_info;
    
#ifdef CPU_MIMXRT1021DAG5A 
	mcu_info.cpu_id = HW_CPU_ID_RT1021;
	mcu_info.combined.cpu_id = mcu_info.cpu_id;
	mcu_info.pin_count = 144;
	mcu_info.combined.pin_count_code = mcu_info.pin_count;
	mcu_info.rev_id = 0;
	mcu_info.flash_kb_exp = 13; // 8192KB
	
#endif
    
    
    //switch(SIM->SDID & (SIM_SDID_FAMILYID_MASK | SIM_SDID_SUBFAMID_MASK | SIM_SDID_SERIESID_MASK)) {
        //case 0x31600000:
            //mcu_info.cpu_id = HW_CPU_ID_MKV31;
            //break;
        //case 0x56600000:
            //mcu_info.cpu_id = HW_CPU_ID_MKV56;
            //break;
        //case 0x58600000:
            //mcu_info.cpu_id = HW_CPU_ID_MKV58;
            //break;
        //default:
            //mcu_info.cpu_id = HW_CPU_ID_UNDEFINED;
            //break;
    //}
    //mcu_info.combined.cpu_id = mcu_info.cpu_id;
    //switch(SIM->SDID & SIM_SDID_PINID_MASK) {
        //case 0x4:
            //mcu_info.pin_count = 48;
            //break;
        //case 0x5:
            //mcu_info.pin_count = 64;
            //break;
        //case 0x6:
            //mcu_info.pin_count = 80;
            //break;
        //case 0x8:
            //mcu_info.pin_count = 100;
            //break;
        //case 0x9:
            //mcu_info.pin_count = 121;
            //break;
        //case 0xA:
            //mcu_info.pin_count = 144;
            //break;
        //default:
            //mcu_info.pin_count = 0;
            //break;
    //}
    //mcu_info.combined.pin_count_code = SIM->SDID & SIM_SDID_PINID_MASK;
    //mcu_info.rev_id = (SIM->SDID & SIM_SDID_REVID_MASK) >> SIM_SDID_REVID_SHIFT;
    //switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK) >> SIM_FCFG1_PFSIZE_SHIFT) {
        //case 3:
            //mcu_info.flash_kb_exp = 5; // 32KB
            //break;
        //case 5:
            //mcu_info.flash_kb_exp = 6; // 64KB
            //break;
        //case 7:
            //mcu_info.flash_kb_exp = 7; // 128KB
            //break;
        //case 9:
        //case 15:
            //mcu_info.flash_kb_exp = 8; // 256KB
            //break;
        //case 11:
            //mcu_info.flash_kb_exp = 9; // 512KB
            //break;
        //case 13:
            //mcu_info.flash_kb_exp = 10; // 1025KB
            //break;
        //default:
            //mcu_info.flash_kb_exp = 0;
            //break;
    //}
    //mcu_info.combined.flash_kb_exp = mcu_info.flash_kb_exp;
    return &mcu_info;
}

#define HARDWARE_ID_1_MASK (1<<HARDWARE_ID_1)
#define HARDWARE_ID_2_MASK (1<<HARDWARE_ID_2)
#define HARDWARE_ID_3_MASK (1<<HARDWARE_ID_3)

// ID values
// ID - Pin1  Pin2 Pin3
// 0  - NC    NC   NC
// 1  - GND   NC   NC
// 2  - NC    GND  NC
// 3  - GND   GND  NC
// 4  - VCC   NC   NC
// 5  - NC    VCC  NC
// 6  - VCC   VCC  NC
// 7  - VCC   GND  NC
// 8  - GND   VCC  NC
// 9  - NC    NC   GND
// 10 - GND   NC   GND
// 11 - NC    GND  GND
// 12 - GND   GND  GND
// 13 - VCC   NC   GND
// 14 - NC    VCC  GND
// 15 - VCC   VCC  GND
// 16 - VCC   GND  GND
// 17 - GND   VCC  GND
// 18 - NC    NC   VCC
// 19 - GND   NC   VCC
// 20 - NC    GND  VCC
// 21 - GND   GND  VCC
// 22 - VCC   NC   VCC
// 23 - NC    VCC  VCC
// 24 - VCC   VCC  VCC
// 25 - VCC   GND  VCC
// 26 - GND   VCC  VCC

enum board_hw_id Hardware_GetID(void) {
    static uint8_t hardware_id = 0;
    //if(hardware_id != 0xFF && hardware_id != 0x00) {
        //return (enum board_hw_id) hardware_id;
    //}
    //// it is assumed PIOA already has power
    //uint32_t input_pull_down, input_pull_up;

    //// set pins as input
    //HARDWARE_ID_GPIO->PDDR &= ~(HARDWARE_ID_1_MASK | HARDWARE_ID_2_MASK | HARDWARE_ID_3_MASK);
    
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_1] = 0x00000102; // enable pull down
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_2] = 0x00000102; // enable pull down
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_3] = 0x00000102; // enable pull down
    //DelayUs(10);  // tiny delay to give time for voltage to change
    //input_pull_down = HARDWARE_ID_GPIO->PDIR;
    
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_1] = 0x00000103; // enable pull up
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_2] = 0x00000103; // enable pull up
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_3] = 0x00000103; // enable pull up
    //DelayUs(10);  // tiny delay to give time for voltage to change
    //input_pull_up = HARDWARE_ID_GPIO->PDIR;
    
    //// disable pull resistors
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_1] = 0x00000100;
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_2] = 0x00000100;
    //HARDWARE_ID_PORT->PCR[HARDWARE_ID_3] = 0x00000100;
    
    //// calculate ID based on ID 1 and ID 2
    //// is Pin1 NC (if values are different then it is not connected)
    //if((input_pull_up & HARDWARE_ID_1_MASK) != (input_pull_down & HARDWARE_ID_1_MASK)) {
        //if((input_pull_up & HARDWARE_ID_2_MASK) != (input_pull_down & HARDWARE_ID_2_MASK)) { // NC NC
            //hardware_id = 0;
        //}else if(input_pull_up & HARDWARE_ID_2_MASK) { // NC VCC
            //hardware_id = 5;
        //}else { // NC GND
            //hardware_id = 2;
        //}
    //}else if(input_pull_up & HARDWARE_ID_1_MASK) { // check for power
        //if((input_pull_up & HARDWARE_ID_2_MASK) != (input_pull_down & HARDWARE_ID_2_MASK)) { // VCC NC
            //hardware_id = 4;
        //}else if(input_pull_up & HARDWARE_ID_2_MASK) { // VCC VCC
            //hardware_id = 6;
        //}else {
            //hardware_id = 7; // VCC GND
        //}
    //}else {
        //if((input_pull_up & HARDWARE_ID_2_MASK) != (input_pull_down & HARDWARE_ID_2_MASK)) { // GND NC
            //hardware_id = 1;
        //}else if(input_pull_up & HARDWARE_ID_2_MASK) { // GND VCC
            //hardware_id = 8;
        //}else {
            //hardware_id = 3; // GND GND
        //}
    //}
    //if(hardware_id == 0 || hardware_id == 2 || hardware_id == 5) {
        //// make pin 1 output low since it is not connected
        //HARDWARE_ID_GPIO->PDDR |= HARDWARE_ID_1_MASK;
        //HARDWARE_ID_GPIO->PCOR = HARDWARE_ID_1_MASK;
    //}
    //if(hardware_id == 0 || hardware_id == 1 || hardware_id == 4) {
        //// make ID 2 output low since it is not connected
        //HARDWARE_ID_GPIO->PDDR |= HARDWARE_ID_2_MASK;
        //HARDWARE_ID_GPIO->PCOR = HARDWARE_ID_2_MASK;
    //}
    //// add 9 if ID3 is GND
    //if((input_pull_up & HARDWARE_ID_3_MASK) == 0 && (input_pull_down & HARDWARE_ID_3_MASK) == 0) { // check for GND
        //hardware_id += 9;
    //}
    //// add 18 if ID3 is VCC
    //else if((input_pull_up & HARDWARE_ID_3_MASK) && (input_pull_down & HARDWARE_ID_3_MASK)) { // check for power
        //hardware_id += 18;
    //}else {
        //// make ID 3 output low since it is not connected
        //HARDWARE_ID_GPIO->PDDR |= HARDWARE_ID_3_MASK;
        //HARDWARE_ID_GPIO->PCOR = HARDWARE_ID_3_MASK;
    //}
    return (enum board_hw_id) hardware_id;
    //return (enum board_hw_id) 5;
}

uint8_t Hardware_HasSecondaryDriverBoard(void) {
    #ifdef DCDC_BOARD
    #ifndef CUBE_BOARD
    static int32_t has_secondary_driver_board = -1;
    //if(has_secondary_driver_board >= 0) {
        //return has_secondary_driver_board;
    //}
    //int i;
    //const enum board_hw_id hw_id = Hardware_GetID();
    //uint32_t pull_down, floating;
    //if(hw_id == HW_ID_DCDC_7_1) {
        //// make input
        //GATE_EN_N_GPIO->PDDR &= ~(1 << GATE_EN_N_PIN);
        //GATE_EN_N_PORT->PCR[GATE_EN_N_PIN] = 0x00000102; // enable pull down
        //for(i = 0; i < 10; i++); // tiny delay to give time for voltage to change
        //pull_down = GATE_EN_N_GPIO->PDIR & (1 << GATE_EN_N_PIN);
        //// disable pull resistors
        //GATE_EN_N_PORT->PCR[GATE_EN_N_PIN] = 0x00000100;
        //for(i = 0; i < 10; i++); // tiny delay to give time for voltage to change
        //floating = GATE_EN_N_GPIO->PDIR & (1 << GATE_EN_N_PIN);
        //// turn back to output
        //GATE_EN_N_GPIO->PDDR |= 1 << GATE_EN_N_PIN;
        //if(pull_down && floating) {
            //has_secondary_driver_board = 1;
        //}else {
            //has_secondary_driver_board = 0;
        //}
    //}else if(hw_id == HW_ID_DCDC_7_2) {
        //// make input
        //ALT_GATE_EN_N_GPIO->PDDR &= ~(1 << ALT_GATE_EN_N_PIN);
        //ALT_GATE_EN_N_PORT->PCR[ALT_GATE_EN_N_PIN] = 0x00000102; // enable pull down
        //for(i = 0; i < 10; i++); // tiny delay to give time for voltage to change
        //pull_down = ALT_GATE_EN_N_GPIO->PDIR & (1 << ALT_GATE_EN_N_PIN);
        //// disable pull resistors
        //ALT_GATE_EN_N_PORT->PCR[ALT_GATE_EN_N_PIN] = 0x00000100;
        //for(i = 0; i < 10; i++); // tiny delay to give time for voltage to change
        //floating = ALT_GATE_EN_N_GPIO->PDIR & (1 << ALT_GATE_EN_N_PIN);
        //// turn back to output
        //ALT_GATE_EN_N_GPIO->PDDR |= 1 << ALT_GATE_EN_N_PIN;
        //if(pull_down && floating) {
            //has_secondary_driver_board = 1;
        //}else {
            //has_secondary_driver_board = 0;
        //}
    //}else {
        //has_secondary_driver_board = 0;
    //}
    return has_secondary_driver_board;
    #else
    return 0;
    #endif
    #else
    return 0;
    #endif
}

uint8_t Hardware_HasHumiditySensor(void) {
    return (Hardware_GetID() >= HW_ID_SPOT1500_6_2);
}

uint8_t Hardware_HasOVP_Internal_Comparator(void) {
    #ifdef CUBE_BOARD
		return 0;
	#else
		return (Hardware_GetID() >= HW_ID_SPOT1500_5_0);
	#endif
}

uint8_t Hardware_HasSecondOVP(void) {
    #ifdef DCDC_BOARD
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id >= HW_ID_SPOT1500_6_2 || hw_id == HW_ID_CUBE_0_0 || hw_id == HW_ID_CUBE_1_0);
    #else
    return 1;
    #endif
}

uint8_t Hardware_HasSpotV6PinoutCMP(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    #ifdef CUBE_BAORD
    return 0;
    #else
    return (hw_id == HW_ID_SPOT1500_6_2 || 
			hw_id == HW_ID_SPOT_6_5 || 
            hw_id == HW_ID_SPOT_6_6_HO25 ||
			hw_id == HW_ID_SPOT600_6_2 || 
			hw_id == HW_ID_SPOT1500_6_1 || 
			hw_id == HW_ID_SPOT600_6_1);
    #endif
}

uint8_t Hardware_HasDiagnosticUART(void) {
    #ifdef GARD_BOARD
    return (Hardware_GetID() >= HW_ID_GARD_2_0);
    #elif defined CUBE_BOARD
    return 1;
    #else
    return (Hardware_GetID() >= HW_ID_SPOT1500_5_0);
    #endif
}

uint8_t Hardware_HasBatteryGateFaultDetection(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id >= HW_ID_BOSS_1_0) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_DAB_6_1) ||(hw_id == HW_ID_DAB_7_0));
}

uint8_t Hardware_HasBidirectionalDCDC(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    if(hw_id == HW_ID_DCDC_7_1 || hw_id == HW_ID_DCDC_7_2) {
        return Hardware_HasSecondaryDriverBoard();
    }
    return ((hw_id >= HW_ID_BOSS_1_0) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0));
}

uint8_t Hardware_HasExternalFan(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id >= HW_ID_BOSS_1_1) || (hw_id == HW_ID_SPOT1500_6_1) || (hw_id == HW_ID_SPOT1500_6_2) || (hw_id == HW_ID_SPOT600_6_2) || 
            (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_SPOT_6_5) || (hw_id == HW_ID_SPOT_6_6_HO25) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0));
}

uint8_t Hardware_HasSecondExternalFan(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_BUCK_BOOST_7_1) || (hw_id == HW_ID_DCDC_7_2));
}

uint8_t Hardware_HasNewGateDriver(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
#ifdef CUBE_BOARD
    return (hw_id == HW_ID_CUBE_0_0 || hw_id == HW_ID_CUBE_1_0 || hw_id == HW_ID_CUBE_1_0_ALT);
#else
    return ((hw_id >= HW_ID_BOSS_1_0) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0));
#endif
}

uint8_t Hardware_HasDabTopology(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    #ifdef CUBE_BOARD
    return 0;
    #else
    if((hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_DCDC_7_2)) {
        return Hardware_HasSecondaryDriverBoard();
    }
    return ((hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0));
    #endif
}

uint8_t Hardware_HasResonantTopology(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    #ifdef CUBE_BOARD
    return 0;
    #else
    return (!Hardware_HasDabTopology() && hw_id != HW_ID_BUCK_BOOST_7_1);
    #endif
}

uint8_t Hardware_HasGroundFaultDetector(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id >= HW_ID_BOSS_1_1) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0));
}

uint8_t Hardware_HasLatestVoltageGain(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id >= HW_ID_SPOT1500_6_2 || hw_id == HW_ID_CUBE_0_0 || hw_id == HW_ID_CUBE_1_0);
}

uint8_t Hardware_HasHO25CurrentSensor(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
	#ifdef GARD_BOARD
    return ((hw_id >= HW_ID_BOSS_1_0) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_GARD_2_1));
	#else
	return ((hw_id >= HW_ID_BOSS_1_0) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0) || (hw_id == HW_ID_SPOT_6_6_HO25) || (hw_id == HW_ID_SPOT1500_6_2));
    #endif
}

uint8_t Hardware_HasV3ADC0(void) {
    return 0;
}

uint8_t Hardware_HasV4ADC0(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id >= HW_ID_SPOT1500_6_2);
}

uint8_t Hardware_Has1_65VrefOnPTC0(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id == HW_ID_BOSS_1_1) || (hw_id == HW_ID_BOSS_7_0) || (hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_DAB_6_1) || (hw_id == HW_ID_DAB_7_0) || (hw_id == HW_ID_DCDC_7_1));
}

uint8_t Hardware_HasRJ45_Shutdown(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_BOSS_7_0) || (hw_id == HW_ID_DAB_7_0) || (hw_id == HW_ID_SPOT_6_6_HO25) || 
        (hw_id == HW_ID_SPOT1500_6_2) || (hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_DCDC_7_2));
}

uint8_t Hardware_HasVincotechThermistor(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return ((hw_id == HW_ID_SPOT_7_0) || (hw_id == HW_ID_BOSS_7_0) || (hw_id == HW_ID_DAB_7_0) || (hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_DCDC_7_2));
}

uint8_t Hardware_HasAuxUVP(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    #ifdef CUBE_BOARD
    return 0;
    #else
    return ((hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_DCDC_7_2));
    #endif
}

uint8_t Hardware_HasTransformerTemperatureSensor(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_DCDC_7_2);
}

uint8_t Hardware_HasBuckboostTopology(void) {
    #ifdef CUBE_BOARD
	return 1;
    #else
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_BUCK_BOOST_7_1);
    #endif
}

uint8_t Hardware_HasFaultyZeroCurrentOffset(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_DCDC_7_1) || (hw_id == HW_ID_BUCK_BOOST_7_1);
}

uint8_t Hardware_HasMKV5Pinout(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_DCDC_7_2);
}

#ifdef GARD_BOARD
uint8_t Hardware_HasOldEmergencySwitchDetect(void){
	const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_0);
}

uint8_t Hardware_HasOldExternalFaultClear(void){
	const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_0);
}

uint8_t Hardware_HasLED_GPIO_Expander(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_0 || hw_id == HW_ID_GARD_2_1);
}

uint8_t Hardware_HasAltDebugUART(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_2 || hw_id == HW_ID_GARD_2_2_1);
}

uint8_t Hardware_HasDifferentialCurrentSensors(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_2 || hw_id == HW_ID_GARD_2_2_1);
}

uint8_t HardwareHasLeakLocatorReference(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_GARD_2_2 || hw_id == HW_ID_GARD_2_2_1);
}
#endif

#ifdef CUBE_BOARD
uint8_t Hardware_HasDoubleEndedCurrentSensor(void) {
    const enum board_hw_id hw_id = Hardware_GetID();
    return (hw_id == HW_ID_CUBE_1_0 || hw_id == HW_ID_CUBE_1_0_ALT);
}
#endif
