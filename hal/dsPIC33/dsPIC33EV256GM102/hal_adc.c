#include "hal_adc.h"
#include "adc.h"

void hal_ADC_Init() {
    AD1CON1 = 0;
    AD1CON1bits.AD12B = 1; // 12 bit
    AD1CON1bits.SSRC = 7; // Internal counter ends sampling and starts convert
    
    AD1CON2 = 0;
    
    AD1CON3 = 0;
    AD1CON3bits.ADRC = 1; // Internal conversion clock select
    AD1CON3bits.SAMC = 0x1F; // Auto sample time 31 Tad
    
    AD1CON4 = 0;
    
    AD1CSSH = 0;
    AD1CSSL = 0;
    
    IPC3bits.AD1IP = 2; // Interrupt priority
    IFS0bits.AD1IF = 0; // Interrupt clear
    IEC0bits.AD1IE = 1; // Interrupt enable
    
    AD1CON1bits.ADON = 1; // Turn on ADC
}

void hal_ADC_StartChannel(uint8_t channel) {
    AD1CHS0 = channel & 0x3F; // Only 64 channels available
    AD1CON1bits.SAMP = 1; // Start sample and conversion
}

void __attribute__((interrupt, auto_psv)) _AD1Interrupt() {
    IFS0bits.AD1IF = 0; // Interrupt clear
    ADC_ProcessMeasurementFromISR(ADC1BUF0);
}
