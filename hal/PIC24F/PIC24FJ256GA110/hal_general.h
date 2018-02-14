#ifndef HAL_GENERAL_
#define HAL_GENERAL_
#include <xc.h>
#include <stdint.h>
// Nop() is defined by the XC16 compiler and is not needed here
// use XC16 enable and disable interrupt routines
#define DisableInterrupts() SET_CPU_IPL(7)
#define EnableInterrupts() RESTORE_CPU_IPL(0)
#define BlockInterrupts() int current_ipl; SET_AND_SAVE_CPU_IPL(current_ipl, 7)
#define RestoreInterrupts() RESTORE_CPU_IPL(current_ipl)
#define hal_Watchdog_Clear() ClrWdt()
#define hal_Watchdog_Enable() RCONbits.SWDTEN = 1

// helper macros for GPIO related stuff
#define SetPinAsDigital(pin) CAT2(_ANS, pin) = 0
#define SetPinAsAnalog(pin) CAT2(_ANS, pin) = 1
#define SetPinAsInput(pin) CAT2(_TRIS, pin) = 1
#define SetPinAsOutput(pin) CAT2(_TRIS, pin) = 0

#define OUTPUT_PIN_TO_REG_NUM(pin) CAT2(OUTPUT_PIN_TO_REG_NUM_PIN, pin)
#define OUTPUT_PIN_TO_REG_NUM_PIN0 0
#define OUTPUT_PIN_TO_REG_NUM_PIN1 0
#define OUTPUT_PIN_TO_REG_NUM_PIN2 1
#define OUTPUT_PIN_TO_REG_NUM_PIN3 1
#define OUTPUT_PIN_TO_REG_NUM_PIN4 2
#define OUTPUT_PIN_TO_REG_NUM_PIN5 2
#define OUTPUT_PIN_TO_REG_NUM_PIN6 3
#define OUTPUT_PIN_TO_REG_NUM_PIN7 3
#define OUTPUT_PIN_TO_REG_NUM_PIN8 4
#define OUTPUT_PIN_TO_REG_NUM_PIN9 4
#define OUTPUT_PIN_TO_REG_NUM_PIN10 5
#define OUTPUT_PIN_TO_REG_NUM_PIN11 5
#define OUTPUT_PIN_TO_REG_NUM_PIN12 6
#define OUTPUT_PIN_TO_REG_NUM_PIN13 6
#define OUTPUT_PIN_TO_REG_NUM_PIN14 7
#define OUTPUT_PIN_TO_REG_NUM_PIN15 7
#define OUTPUT_PIN_TO_REG_NUM_PIN16 8
#define OUTPUT_PIN_TO_REG_NUM_PIN17 8
#define OUTPUT_PIN_TO_REG_NUM_PIN18 9
#define OUTPUT_PIN_TO_REG_NUM_PIN19 9
#define OUTPUT_PIN_TO_REG_NUM_PIN20 10
#define OUTPUT_PIN_TO_REG_NUM_PIN21 10
#define OUTPUT_PIN_TO_REG_NUM_PIN22 11
#define OUTPUT_PIN_TO_REG_NUM_PIN23 11
#define OUTPUT_PIN_TO_REG_NUM_PIN24 12
#define OUTPUT_PIN_TO_REG_NUM_PIN25 12
#define OUTPUT_PIN_TO_REG_NUM_PIN26 13
#define OUTPUT_PIN_TO_REG_NUM_PIN27 13
#define OUTPUT_PIN_TO_REG_NUM_PIN28 14
#define OUTPUT_PIN_TO_REG_NUM_PIN29 14
#define OUTPUT_PIN_TO_REG_NUM_PIN30 15
#define OUTPUT_PIN_TO_REG_NUM_PIN31 15

#define PPS_Unlock() __builtin_write_OSCCONL(OSCCON & 0xbf)
#define PPS_Lock() __builtin_write_OSCCONL(OSCCON | 0x40)
#define SetPPS_Out(pin) CAT5(RPOR, OUTPUT_PIN_TO_REG_NUM(pin), bits.RP, pin, R) 

typedef struct {
    uint8_t crap_bit:1;
} crap_t;
extern crap_t crap;
// ANSEL bit defines
#define _ANSA0 crap.crap_bit
#define _ANSA1 crap.crap_bit
#define _ANSA2 crap.crap_bit
#define _ANSA3 crap.crap_bit
#define _ANSA4 crap.crap_bit
#define _ANSA5 crap.crap_bit
#define _ANSA6 crap.crap_bit
#define _ANSA7 crap.crap_bit
#define _ANSA8 crap.crap_bit
#define _ANSA9 crap.crap_bit
#define _ANSA10 crap.crap_bit
#define _ANSA11 crap.crap_bit
#define _ANSA12 crap.crap_bit
#define _ANSA13 crap.crap_bit
#define _ANSA14 crap.crap_bit
#define _ANSA15 crap.crap_bit
#define _ANSB0 AD1PCFGLbits.PCFG0
#define _ANSB1 AD1PCFGLbits.PCFG1
#define _ANSB2 AD1PCFGLbits.PCFG2
#define _ANSB3 AD1PCFGLbits.PCFG3
#define _ANSB4 AD1PCFGLbits.PCFG4
#define _ANSB5 AD1PCFGLbits.PCFG5
#define _ANSB6 AD1PCFGLbits.PCFG6
#define _ANSB7 AD1PCFGLbits.PCFG7
#define _ANSB8 AD1PCFGLbits.PCFG8
#define _ANSB9 AD1PCFGLbits.PCFG9
#define _ANSB10 AD1PCFGLbits.PCFG10
#define _ANSB11 AD1PCFGLbits.PCFG11
#define _ANSB12 AD1PCFGLbits.PCFG12
#define _ANSB13 AD1PCFGLbits.PCFG13
#define _ANSB14 AD1PCFGLbits.PCFG14
#define _ANSB15 AD1PCFGLbits.PCFG15
#define _ANSC0 crap.crap_bit
#define _ANSC1 crap.crap_bit
#define _ANSC2 crap.crap_bit
#define _ANSC3 crap.crap_bit
#define _ANSC4 crap.crap_bit
#define _ANSC5 crap.crap_bit
#define _ANSC6 crap.crap_bit
#define _ANSC7 crap.crap_bit
#define _ANSC8 crap.crap_bit
#define _ANSC9 crap.crap_bit
#define _ANSC10 crap.crap_bit
#define _ANSC11 crap.crap_bit
#define _ANSC12 crap.crap_bit
#define _ANSC13 crap.crap_bit
#define _ANSC14 crap.crap_bit
#define _ANSC15 crap.crap_bit
#define _ANSD0 crap.crap_bit
#define _ANSD1 crap.crap_bit
#define _ANSD2 crap.crap_bit
#define _ANSD3 crap.crap_bit
#define _ANSD4 crap.crap_bit
#define _ANSD5 crap.crap_bit
#define _ANSD6 crap.crap_bit
#define _ANSD7 crap.crap_bit
#define _ANSD8 crap.crap_bit
#define _ANSD9 crap.crap_bit
#define _ANSD10 crap.crap_bit
#define _ANSD11 crap.crap_bit
#define _ANSD12 crap.crap_bit
#define _ANSD13 crap.crap_bit
#define _ANSD14 crap.crap_bit
#define _ANSD15 crap.crap_bit
#define _ANSE0 crap.crap_bit
#define _ANSE1 crap.crap_bit
#define _ANSE2 crap.crap_bit
#define _ANSE3 crap.crap_bit
#define _ANSE4 crap.crap_bit
#define _ANSE5 crap.crap_bit
#define _ANSE6 crap.crap_bit
#define _ANSE7 crap.crap_bit
#define _ANSE8 crap.crap_bit
#define _ANSE9 crap.crap_bit
#define _ANSF0 crap.crap_bit
#define _ANSF1 crap.crap_bit
#define _ANSF2 crap.crap_bit
#define _ANSF3 crap.crap_bit
#define _ANSF4 crap.crap_bit
#define _ANSF5 crap.crap_bit
#define _ANSF6 crap.crap_bit
#define _ANSF7 crap.crap_bit
#define _ANSF8 crap.crap_bit
#define _ANSF9 crap.crap_bit
#define _ANSF10 crap.crap_bit
#define _ANSF11 crap.crap_bit
#define _ANSF12 crap.crap_bit
#define _ANSF13 crap.crap_bit
#define _ANSG0 crap.crap_bit
#define _ANSG1 crap.crap_bit
#define _ANSG2 crap.crap_bit
#define _ANSG3 crap.crap_bit
#define _ANSG4 crap.crap_bit
#define _ANSG5 crap.crap_bit
#define _ANSG6 crap.crap_bit
#define _ANSG7 crap.crap_bit
#define _ANSG8 crap.crap_bit
#define _ANSG9 crap.crap_bit
#define _ANSG10 crap.crap_bit
#define _ANSG11 crap.crap_bit
#define _ANSG12 crap.crap_bit
#define _ANSG13 crap.crap_bit
#define _ANSG14 crap.crap_bit

#define RP_PinToGPIO_Pin(pin) CAT2(RPPINTOGPIOPIN, pin)

#define RPPINTOGPIOPIN0  B0
#define RPPINTOGPIOPIN1  B1
#define RPPINTOGPIOPIN2  D8
#define RPPINTOGPIOPIN3  D10
#define RPPINTOGPIOPIN4  D9
#define RPPINTOGPIOPIN5  D15
#define RPPINTOGPIOPIN6  B6
#define RPPINTOGPIOPIN7  B7
#define RPPINTOGPIOPIN8  B8
#define RPPINTOGPIOPIN9  B9
#define RPPINTOGPIOPIN10 F4
#define RPPINTOGPIOPIN11 D0
#define RPPINTOGPIOPIN12 D11
#define RPPINTOGPIOPIN13 B2
#define RPPINTOGPIOPIN14 B14
#define RPPINTOGPIOPIN15 F8
#define RPPINTOGPIOPIN16 F3
#define RPPINTOGPIOPIN17 F5
#define RPPINTOGPIOPIN18 B5
#define RPPINTOGPIOPIN19 G8
#define RPPINTOGPIOPIN20 D5
#define RPPINTOGPIOPIN21 G6
#define RPPINTOGPIOPIN22 D3
#define RPPINTOGPIOPIN23 D2
#define RPPINTOGPIOPIN24 D1
#define RPPINTOGPIOPIN25 D4
#define RPPINTOGPIOPIN26 G7
#define RPPINTOGPIOPIN27 G9
#define RPPINTOGPIOPIN28 B4
#define RPPINTOGPIOPIN29 B15
#define RPPINTOGPIOPIN30 F2
#define RPPINTOGPIOPIN31 F13
#define RPPINTOGPIOPIN32 F12
#define RPPINTOGPIOPIN33 E8
#define RPPINTOGPIOPIN34 E9
#define RPPINTOGPIOPIN35 A15
#define RPPINTOGPIOPIN36 A14
#define RPPINTOGPIOPIN37 C14
#define RPPINTOGPIOPIN38 C1
#define RPPINTOGPIOPIN39 C2
#define RPPINTOGPIOPIN40 C3
#define RPPINTOGPIOPIN41 C4
#define RPPINTOGPIOPIN42 D12
#define RPPINTOGPIOPIN43 D14
#define RPPINTOGPIOPIN44 F7
#define RPPINTOGPIOPIN45 F6

#endif // HAL_GENERAL_
