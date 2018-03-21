#include "timing.h"
#include "hal_general.h"

volatile uint32_t time_ms, rollover_time;

/*************************************
 * HAL Function Declarations
 *************************************/
void hal_Timing_Init(void);

#define TimingISR() time_ms++

/* Including the hal_timing.c file here allows the ISR defined in that module
 * to have access to the time variable without extern-ing it.  It allows the ISR
 * to not have to make a function call. */
#include "hal_timing.c"

void Timing_Init(void) {
    // use flag so module only gets initialized once
    static uint8_t init_flag = 0;
    if(init_flag) return;
    time_ms = 0;
    rollover_time = TIME_MAX;
    hal_Timing_Init();
    init_flag = 1;
}

uint32_t TimeNow(void) {
    return time_ms;
}

uint32_t TimeSince(uint32_t t) {
    if (time_ms >= t) {
        return (time_ms - t);
    } else {
        // The time variable has rolled over
        return (time_ms + (1 + (rollover_time - t)));
    }
}

void DelayMs(uint32_t delay) {
    uint32_t temp;

    temp = TimeNow();
    while (TimeSince(temp) <= delay) {
        Nop();
    }
}

void Timing_Roll(void) {
    rollover_time = time_ms;
    time_ms = 0;
}

uint32_t TimeNowUs(void) {
    return time_ms * 1000 + TimingUsNow();
}

void DelayUs(uint32_t delay) {
    // get the time
    uint32_t timestamp;
    timestamp = TimeNowUs();
    while(TimeSinceUs(timestamp) < delay);
}

uint32_t TimeSinceUs(uint32_t t) {
    uint32_t time_us;
    time_us = time_ms * 1000 + TimingUsNow();
    if (time_us >= t) {
        return (time_us - t);
    } else {
        // The time variable has rolled over
        return (time_us + (1 + (0xFFFFFFFF - t)));
    }
}
