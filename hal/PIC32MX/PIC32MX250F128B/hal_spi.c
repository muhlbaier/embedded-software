#include <xc.h>
#include <stdint.h>
#include "project_settings.h"
#include "spi.h"
#include "macros.h"

crap_t crap;

#define Set_SDO1_PPS(pin) CAT3(RP, pin, R) = 3 // SDO1
#define Set_SDO2_PPS(pin) CAT3(RP, pin, R) = 4 // SDO2
#define Set_SDI1_PPS(pin) SDI1R = CAT2(IN_PIN_PPS2_RP, pin)
#define Set_SDI2_PPS(pin) SDI2R = CAT2(IN_PIN_PPS3_RP, pin)

// defaults for PPS if not set in hal_settings.h
#ifndef SPI1_MOSI_PIN
#define SPI1_MOSI_PIN B13
#endif
#ifndef SPI1_MISO_PIN
#define SPI1_MISO_PIN A1
#endif
#ifndef SPI2_MOSI_PIN
#define SPI2_MOSI_PIN B5
#endif
#ifndef SPI2_MISO_PIN
#define SPI2_MISO_PIN A2
#endif

#define SPI_INTERRUPT_PRIORITY 2

void hal_SPI_Init(spi_settings_t* settings) {
    uint32_t x;
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
            // master mode, RxIF set when buffer not empty, TxIF set when buffer is empty
            // enhanced buffer mode on
            SPI1CON = 0x00010025;
            SPI1CONbits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI1CONbits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI1CONbits.SMP = 1; // 1 complies with normal SPI modes
            SPI1STAT = 0;
            SPI1BRG = PERIPHERAL_CLOCK / settings->bit_rate / 2;
            IPC7bits.SPI1IP = SPI_INTERRUPT_PRIORITY;
            if(SPI1BRG) SPI1BRG -= 1;
            SetPinAsDigital(SPI1_MOSI_PIN);
            SetPinAsDigital(SPI1_MISO_PIN);
            SetPinAsInput(SPI1_MISO_PIN);
            SetPinAsOutput(SPI1_MOSI_PIN);
            Set_SDO1_PPS(SPI1_MOSI_PIN);
            Set_SDI1_PPS(SPI1_MISO_PIN);
            break;
        case SPI_2:
            // master mode, RxIF set when buffer not empty, TxIF set when buffer is empty
            // enhanced buffer mode on
            SPI2CON = 0x00010025;
            SPI2CONbits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI2CONbits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI2CONbits.SMP = 1; // 1 complies with normal SPI modes
            SPI2STAT = 0;
            SPI2BRG = PERIPHERAL_CLOCK / settings->bit_rate / 2;
            IPC9bits.SPI2IP = SPI_INTERRUPT_PRIORITY;
            if(SPI2BRG) SPI2BRG -= 1;
            SetPinAsDigital(SPI2_MOSI_PIN);
            SetPinAsDigital(SPI2_MISO_PIN);
            SetPinAsInput(SPI2_MISO_PIN);
            SetPinAsOutput(SPI2_MOSI_PIN);
            Set_SDO2_PPS(SPI2_MOSI_PIN);
            Set_SDI2_PPS(SPI2_MISO_PIN);
            break;
    }
    hal_SPI_Enable(ch);
    hal_SPI_EnableRxInterrupt(ch);
}

void hal_SPI_Enable(uint8_t channel) {
    switch (channel) {
        case SPI_1: SPI1CONbits.ON = 1; break;
        case SPI_2: SPI2CONbits.ON = 1; break;
        default: break;
    }
}

void hal_SPI_Disable(uint8_t channel) {
    switch (channel) {
        case SPI_1: SPI1CONbits.ON = 0; break;
        case SPI_2: SPI2CONbits.ON = 0; break;
        default: break;
    }
}

void hal_SPI_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC1bits.SPI1RXIE = 1; break;
        case SPI_2: IEC1bits.SPI2RXIE = 1; break;
        default: break;
    }
}

void hal_SPI_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC1bits.SPI1TXIE = 1; break;
        case SPI_2: IEC1bits.SPI2TXIE = 1; break;
        default: break;
    }
}

void hal_SPI_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC1bits.SPI1RXIE = 0; break;
        case SPI_2: IEC1bits.SPI2RXIE = 0; break;
        default: break;
    }
}

void hal_SPI_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_1: IEC1bits.SPI1TXIE = 0; break;
        case SPI_2: IEC1bits.SPI2TXIE = 0; break;
        default: break;
    }
}

void hal_SPI_TxByte(uint8_t channel, uint8_t b) {
    switch (channel) {
        case SPI_1: SPI1BUF = b; break;
        case SPI_2: SPI2BUF = b; break;
        default: break;
    }
}

uint8_t hal_SPI_RxByte(uint8_t channel) {
    switch (channel) {
        case SPI_1: return SPI1BUF;
        case SPI_2: return SPI2BUF;
        default:    return 0;
    }
}

void hal_SPI_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case SPI_1: IFS1bits.SPI1RXIF = 0; break;
        case SPI_2: IFS1bits.SPI2RXIF = 0; break;
        default: break;
    }
}

void hal_SPI_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case SPI_1: IFS1bits.SPI1TXIF = 0; break;
        case SPI_2: IFS1bits.SPI2TXIF = 0; break;
        default: break;
    }
}

uint8_t hal_SPI_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_1: return !SPI1STATbits.SPITBF;
        case SPI_2: return !SPI2STATbits.SPITBF;
        default:    return 0;
    }
}

uint8_t hal_SPI_DataAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_1: return !SPI1STATbits.SPIRBE;
        case SPI_2: return !SPI2STATbits.SPIRBE;
        default:    return 0;
    }
}

uint8_t hal_SPI_RxIntStatus(uint8_t channel) {
    switch (channel) {
        case SPI_1: return IFS1bits.SPI1RXIF;
        case SPI_2: return IFS1bits.SPI2RXIF;
        default:    return 0;
    }
}

uint8_t hal_SPI_TxIntStatus(uint8_t channel) {
    switch (channel) {
        case SPI_1: return IFS1bits.SPI1TXIF;
        case SPI_2: return IFS1bits.SPI2TXIF;
        default:    return 0;
    }
}

uint8_t hal_SPI_IsTxIntEnabled(uint8_t channel) {
    switch (channel) {
        case SPI_1: return IEC1bits.SPI1TXIE;
        case SPI_2: return IEC1bits.SPI2TXIE;
        default:    return 0;
    }
}



#ifdef USE_SPI1
void __attribute__((vector(_SPI_1_VECTOR), interrupt(), nomips16)) _SPI1_ISR(void) {
    SPI_ISR(SPI_1);
}
#endif

#ifdef USE_SPI2
void __attribute__((vector(_SPI_2_VECTOR), interrupt(), nomips16)) _SPI2_ISR(void) {
    SPI_ISR(SPI_2);
}
#endif
