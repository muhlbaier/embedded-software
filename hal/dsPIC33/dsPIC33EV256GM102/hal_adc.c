#include "hal_adc.h"
#include "adc.h"

void hal_ADC_Init() {
    AD1CON1 = 0;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.SSRC = 7;
    
    AD1CON2 = 0;
    
    AD1CON3 = 0;
    AD1CON3bits.SAMC = 0x1F;
    AD1CON3bits.ADRC = 1;
    
    AD1CON4 = 0;
    
    AD1CHS0 = 0;
    
    AD1CSSH = 0;
    AD1CSSL = 0;
    
    IPC3bits.AD1IP = 2;
    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;
    
    AD1CON1bits.ADON = 1;
}

void hal_ADC_StartChannel(uint8_t channel) {
    AD1CON1bits.SAMP = 1;
}

void __attribute__((interrupt, auto_psv)) _AD1Interrupt() {
    IFS0bits.AD1IF = 0;
    uint16_t value = ADC1BUF0;
    ADC_ProcessMeasurementFromISR(value);
}
