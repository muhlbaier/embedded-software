#include <xc.h>
#include <stdint.h>
#include "project_settings.h"
#include "spi.h"
#include "macros.h"
#include "hal_general.h"

crap_t crap;

#define Set_SDO1_PPS(pin) SetPPS_Out(pin) = 7 // SDO1
#define Set_SDO2_PPS(pin) SetPPS_Out(pin) = 10 // SDO2
#define Set_SDO3_PPS(pin) SetPPS_Out(pin) = 32 // SDO3
#define Set_SCK1_PPS(pin) SetPPS_Out(pin) = 8 // SCK1OUT
#define Set_SCK2_PPS(pin) SetPPS_Out(pin) = 11 // SCK2OUT
#define Set_SCK3_PPS(pin) SetPPS_Out(pin) = 33 // SCK3OUT
#define Set_SDI1_PPS(pin) RPINR20bits.SDI1R = pin
#define Set_SDI2_PPS(pin) RPINR22bits.SDI2R = pin
#define Set_SDI3_PPS(pin) RPINR28bits.SDI3R = pin

#ifdef USE_SPI1
#ifndef SPI1_MOSI_PIN
#error "Need to have define for SPI1_MOSI_PIN ex. #define SPI1_MOSI_PIN 19"
#endif
#ifndef SPI1_MISO_PIN
#error "Need to have define for SPI1_MISO_PIN ex. #define SPI1_MISO_PIN 19"
#endif
#ifndef SPI1_SCK_PIN
#error "Need to have define for SPI1_SCK_PIN ex. #define SPI1_SCK_PIN 19"
#endif
#endif
#ifdef USE_SPI2
#ifndef SPI2_MOSI_PIN
#error "Need to have define for SPI2_MOSI_PIN ex. #define SPI2_MOSI_PIN 19"
#endif
#ifndef SPI2_MISO_PIN
#error "Need to have define for SPI2_MISO_PIN ex. #define SPI2_MISO_PIN 19"
#endif
#ifndef SPI2_SCK_PIN
#error "Need to have define for SPI2_SCK_PIN ex. #define SPI2_SCK_PIN 19"
#endif
#endif
#ifdef USE_SPI3
#ifndef SPI3_MOSI_PIN
#error "Need to have define for SPI3_MOSI_PIN ex. #define SPI3_MOSI_PIN 19"
#endif
#ifndef SPI3_MISO_PIN
#error "Need to have define for SPI3_MISO_PIN ex. #define SPI3_MISO_PIN 19"
#endif
#ifndef SPI3_SCK_PIN
#error "Need to have define for SPI3_SCK_PIN ex. #define SPI3_SCK_PIN 19"
#endif
#endif

#ifndef PERIPHERAL_CLOCK
#define PERIPHERAL_CLOCK FCPU
#endif

#define SPI_INTERRUPT_PRIORITY 2

static uint16_t CalcPrescaleBits(uint32_t bit_rate);

void hal_SPI_Init(spi_settings_t* settings) {
    uint8_t x;
    uint8_t ch;
    ch = settings->channel;
    // Disable
    hal_SPI_DisableTxInterrupt(ch);
    hal_SPI_DisableRxInterrupt(ch);
    hal_SPI_Disable(ch);
    // clear RX buffer
    while(hal_SPI_DataAvailable(ch)) x = hal_SPI_RxByte(ch);
    hal_SPI_ClearRxIF(ch);
    hal_SPI_ClearTxIF(ch);
    switch (ch) {
        case SPI_1:
#ifdef USE_SPI1
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI1_MOSI_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI1_MISO_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI1_SCK_PIN));
            SetPinAsInput(RP_PinToGPIO_Pin(SPI1_MISO_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI1_MOSI_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI1_SCK_PIN));
            Set_SDO1_PPS(SPI1_MOSI_PIN);
            Set_SCK1_PPS(SPI1_SCK_PIN);
            Set_SDI1_PPS(SPI1_MISO_PIN);
            SPI1STAT = 0;
            hal_SPI_EnableRxInterrupt(ch);
            IPC2bits.SPI1IP = SPI_INTERRUPT_PRIORITY; // 2->8, 3->22
            SPI1CON1 = CalcPrescaleBits(settings->bit_rate);
            // master mode on
            SPI1CON1bits.MSTEN = 1;
            SPI1CON1bits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI1CON1bits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI1CON1bits.SMP = 1; // 1 complies with normal SPI modes
            // set interrupt mode to interrupt when RX buffer is not empty
            SPI1STAT = 0x0004;
            // enhanced buffer mode on
            SPI1CON2 = 0x0001;
#endif
            break;
        case SPI_2:
#ifdef USE_SPI2
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI2_MOSI_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI2_MISO_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI2_SCK_PIN));
            SetPinAsInput(RP_PinToGPIO_Pin(SPI2_MISO_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI2_MOSI_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI2_SCK_PIN));
            Set_SDO1_PPS(SPI2_MOSI_PIN);
            Set_SCK1_PPS(SPI2_SCK_PIN);
            Set_SDI1_PPS(SPI2_MISO_PIN);
            SPI2STAT = 0;
            hal_SPI_EnableRxInterrupt(ch);
            IPC8bits.SPI2IP = SPI_INTERRUPT_PRIORITY; // 2->8, 3->22
            SPI2CON1 = CalcPrescaleBits(settings->bit_rate);
            // master mode on
            SPI2CON1bits.MSTEN = 1;
            SPI2CON1bits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI2CON1bits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI2CON1bits.SMP = 1; // 1 complies with normal SPI modes
            // set interrupt mode to interrupt when RX buffer is not empty
            SPI2STAT = 0x0004;
            // enhanced buffer mode on
            SPI2CON2 = 0x0001;
#endif
            break;
        case SPI_3:
#ifdef USE_SPI3
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI3_MOSI_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI3_MISO_PIN));
            SetPinAsDigital(RP_PinToGPIO_Pin(SPI3_SCK_PIN));
            SetPinAsInput(RP_PinToGPIO_Pin(SPI3_MISO_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI3_MOSI_PIN));
            SetPinAsOutput(RP_PinToGPIO_Pin(SPI3_SCK_PIN));
            Set_SDO3_PPS(SPI3_MOSI_PIN);
            Set_SCK3_PPS(SPI3_SCK_PIN);
            Set_SDI3_PPS(SPI3_MISO_PIN);
            SPI3STAT = 0;
            hal_SPI_EnableRxInterrupt(ch);
            IPC22bits.SPI3IP = SPI_INTERRUPT_PRIORITY; // 2->8, 3->22
            SPI3CON1 = CalcPrescaleBits(settings->bit_rate);
            // master mode on
            SPI3CON1bits.MSTEN = 1;
            SPI3CON1bits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI3CON1bits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI3CON1bits.SMP = 1; // 1 complies with normal SPI modes
            // set interrupt mode to interrupt when RX buffer is not empty
            SPI3STAT = 0x0004;
            // enhanced buffer mode on
            SPI3CON2 = 0x0001;
#endif
            break;
    }
    hal_SPI_Enable(ch);
}

void hal_SPI_Enable(uint8_t channel) {
    switch (channel) {
        case SPI_1: SPI1STATbits.SPIEN = 1; break;
        case SPI_2: SPI2STATbits.SPIEN = 1; break;
        case SPI_3: SPI3STATbits.SPIEN = 1; break;
        default: break;
    }
}

void hal_SPI_Disable(uint8_t channel) {
    switch (channel) {
        case SPI_1: SPI1STATbits.SPIEN = 0; break;
        case SPI_2: SPI2STATbits.SPIEN = 0; break;
        case SPI_3: SPI3STATbits.SPIEN = 0; break;
        default: break;
    }
}

void hal_SPI_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC0bits.SPI1IE = 1; break;
        case SPI_2: IEC2bits.SPI2IE = 1; break;
        case SPI_3: IEC5bits.SPI3IE = 1; break;
        default: break;
    }
}

// there is only one interrupt flag for SPI on this processor
void hal_SPI_EnableTxInterrupt(uint8_t channel) {

}

void hal_SPI_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC0bits.SPI1IE = 0; break;
        case SPI_2: IEC2bits.SPI2IE = 0; break;
        case SPI_3: IEC5bits.SPI3IE = 0; break;
        default: break;
    }
}

// there is only one interrupt flag for SPI on this processor
void hal_SPI_DisableTxInterrupt(uint8_t channel) {
    
}

void hal_SPI_TxByte(uint8_t channel, uint8_t b) {
    switch (channel) {
        case SPI_1: SPI1BUF = b; break;
        case SPI_2: SPI2BUF = b; break;
        case SPI_3: SPI3BUF = b; break;
        default: break;
    }
}

uint8_t hal_SPI_RxByte(uint8_t channel) {
    switch (channel) {
        case SPI_1: return SPI1BUF;
        case SPI_2: return SPI2BUF;
        case SPI_3: return SPI3BUF;
        default:    return 0;
    }
}

// 
void hal_SPI_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case SPI_1: IFS0bits.SPI1IF = 0; break;
        case SPI_2: IFS2bits.SPI2IF = 0; break;
        case SPI_3: IFS5bits.SPI3IF = 0; break;
        default: break;
    }
}

// there is no TX interrupt, use the RX interrupt only.
void hal_SPI_ClearTxIF(uint8_t channel) {

}

uint8_t hal_SPI_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_1: return !SPI1STATbits.SPITBF;
        case SPI_2: return !SPI2STATbits.SPITBF;
        case SPI_3: return !SPI3STATbits.SPITBF;
        default:    return 0;
    }
}

uint8_t hal_SPI_DataAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_1: return !SPI1STATbits.SRXMPT;
        case SPI_2: return !SPI2STATbits.SRXMPT;
        case SPI_3: return !SPI3STATbits.SRXMPT;
        default:    return 0;
    }
}

// use data available since this chip doesn't have a rx interrupt
uint8_t hal_SPI_RxIntStatus(uint8_t channel) {
    return hal_SPI_DataAvailable(channel);
}

// use space available since this chip doesn't have a tx interrupt
uint8_t hal_SPI_TxIntStatus(uint8_t channel) {
    return hal_SPI_SpaceAvailable(channel);
}

uint8_t hal_SPI_IsTxIntEnabled(uint8_t channel) {
    switch (channel) {
        case SPI_1: return IEC0bits.SPI1IE;
        case SPI_2: return IEC2bits.SPI2IE;
        case SPI_3: return IEC5bits.SPI3IE;
        default:    return 0;
    }
}

static uint16_t CalcPrescaleBits(uint32_t bit_rate) {
    uint16_t divide_ratio;
    uint16_t prescale_settings;
    if(bit_rate < PERIPHERAL_CLOCK) {
        // determine what fraction the bit_rate is of the clock
        divide_ratio = PERIPHERAL_CLOCK / bit_rate;
        // determine the best primary prescale bits
        if(divide_ratio >= 128) {
            prescale_settings = 0; // 64:1
            divide_ratio >>= 6;
        }else if(divide_ratio >= 32) {
            prescale_settings = 1; // 16:1
            divide_ratio >>= 4;
        }else if(divide_ratio >= 8) {
            prescale_settings = 2; // 4:1
            divide_ratio >>= 2;
        }else {
            prescale_settings = 3; // 1:1
        }
        // the secondary prescale bits are just 8 - the remaining divide_ratio
        // 111 = 1:1
        // 110 = 2:1
        // ...
        // 000 = 8:1
        // make sure the divide ratio is within the valid limits
        if(divide_ratio < 1) divide_ratio = 1;
        if(divide_ratio > 8) divide_ratio = 8;
        prescale_settings |= (8-divide_ratio) << 2;
    }else {
        prescale_settings = 0x001F; // 1:1 x 1:1
    }
    return prescale_settings;
}

#ifdef USE_SPI1
void __attribute__((interrupt, auto_psv)) _SPI1Interrupt(void) {
    SPI_ISR(SPI_1);
}
#endif

#ifdef USE_SPI2
void __attribute__((interrupt, auto_psv)) _SPI2Interrupt(void) {
    SPI_ISR(SPI_2);
}
#endif

#ifdef USE_SPI3
void __attribute__((interrupt, auto_psv)) _SPI3Interrupt(void) {
    SPI_ISR(SPI_3);
}
#endif
