#include "project_settings.h"
#include "hal_general.h"
#include "int_def.h"
#include "hal_keyfob_receiver.h"
#include "keyfob_receiver.h"

extern uint16_t keyfob_min_time_unit;

// hard coded to use RPF5 on IC4 and use TMR2/3 as timer source

void hal_Keyfob_Init(void) {
    // set IC4 to RPF5
    IC4R = 2;
    IC4CON = 0x00008181; // TMR2 is source, capture every edge, 32-bit mode
    T2CON = 0x00008068; // 32-bit timer, 1:64 prescale
    PR2 = 0xFFFFFFFF;
    TMR2 = 0;
    IPC4bits.IC4IP = 3;
    IFS0bits.IC4IF = 0;
    IEC0bits.IC4IE = 1;
}

// hal interrupt
void __attribute__((vector(_INPUT_CAPTURE_4_VECTOR), interrupt(), nomips16)) _IC4_ISR(void) {
    uint16_t time;
    static uint32_t prev_count = 0;
    uint32_t count;

    while (IC4CONbits.ICBNE) {
        // get time
        count = IC4BUF;
        
        // whenever the timer rolls we will have a bad time value
        // worst case (80MHz) this happens every 57 minutes which is acceptable
        time = (count - prev_count);
        
        prev_count = count;
        // reject noise here for faster interrupt processing
        if(time >= keyfob_min_time_unit) {
            Keyfob_ProcessPulse(time);
        }
    }
    IFS0bits.IC4IF = 0;
}
