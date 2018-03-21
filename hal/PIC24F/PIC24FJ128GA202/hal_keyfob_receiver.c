#include "project_settings.h"
#include "hal_general.h"
#include "int_def.h"
#include "hal_keyfob_receiver.h"
#include "keyfob_receiver.h"

#define Set_IC1_PPS(pin) RPINR7bits.IC1R = pin
        
void hal_Keyfob_Init(void) {
    Set_IC1_PPS(IC1_PIN);
    IC1CON1 = 0x1C01; // system clock is the clock source, capture every edge
    IC1CON2 = 0x0100; // 32-bit cascade mode no synchronization to any other module
    IC2CON1 = 0x1C01; 
    IC2CON2 = 0x0100; 
    IPC0bits.IC1IP = 3;
    IFS0bits.IC1IF = 0;
    IEC0bits.IC1IE = 1;
}

// hal interrupt
void __attribute__((interrupt, auto_psv)) _IC1Interrupt(void) {
    uint16_t time;
    static uint32_t prev_count = 0;
    union32_t count;

    while (IC1CON1bits.ICBNE) {
        // get time
        count.word[0] = IC1BUF;
        count.word[1] = IC2BUF;
        
        if(count.phrase > prev_count){
            time = (count.phrase - prev_count) >> 4;
        }
        else{
            time = ((0xFFFFFFFF - prev_count) + count.phrase) >> 4;
        }
        prev_count = count.phrase;
        Keyfob_ProcessPulse(time);
    }
    IFS0bits.IC1IF = 0;
}
