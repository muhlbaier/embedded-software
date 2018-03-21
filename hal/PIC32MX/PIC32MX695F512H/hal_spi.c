#include <xc.h>
#include <stdint.h>
#include "project_settings.h"
#include "spi.h"

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
        case SPI_2:
            // master mode, RxIF set when buffer not empty, TxIF set when buffer is empty
            // enhanced buffer mode on
            SPI2CON = 0x00010025;
            SPI2CONbits.CKE = (settings->mode & 1) ? 0 : 1; // CKE = ~mode bit 0
            SPI2CONbits.CKP = (settings->mode & 2) ? 1 : 0; // CKP = mode bit 1
            SPI2CONbits.SMP = 1; // 1 complies with normal SPI modes
            SPI2STAT = 0;
            SPI2BRG = PERIPHERAL_CLOCK / settings->bit_rate / 2;
            if(SPI2BRG) SPI2BRG -= 1;
            break;
        case SPI_3:
            SPI3CON = 0x00010025;
            SPI3CONbits.CKE = (settings->mode & 1) ? 0 : 1;
            SPI3CONbits.CKP = (settings->mode & 2) ? 1 : 0;
            SPI3CONbits.SMP = 1; // 1 complies with normal SPI modes
            SPI3STAT = 0;
            SPI3BRG = PERIPHERAL_CLOCK / settings->bit_rate / 2;
            if(SPI3BRG) SPI3BRG -= 1;
            break;
        case SPI_4:
            SPI4CON = 0x00010025;
            SPI4CONbits.CKE = (settings->mode & 1) ? 0 : 1;
            SPI4CONbits.CKP = (settings->mode & 2) ? 1 : 0;
            SPI4CONbits.SMP = 1; // 1 complies with normal SPI modes
            SPI4STAT = 0;
            SPI4BRG = PERIPHERAL_CLOCK / settings->bit_rate / 2;
            if(SPI4BRG) SPI4BRG -= 1;
            break;
    }
    hal_SPI_Enable(ch);
    hal_SPI_EnableRxInterrupt(ch);
}

void hal_SPI_Enable(uint8_t channel) {
    switch (channel) {
        case SPI_2: SPI2CONbits.ON = 1; break;
        case SPI_3: SPI3CONbits.ON = 1; break;
        case SPI_4: SPI4CONbits.ON = 1; break;
        default: break;
    }
}

void hal_SPI_Disable(uint8_t channel) {
    switch (channel) {
        case SPI_2: SPI2CONbits.ON = 0; break;
        case SPI_3: SPI3CONbits.ON = 0; break;
        case SPI_4: SPI4CONbits.ON = 0; break;
        default: break;
    }
}

void hal_SPI_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_2: IEC1bits.SPI2RXIE = 1; break;
        case SPI_3: IEC0bits.SPI3RXIE = 1; break;
        case SPI_4: IEC1bits.SPI4RXIE = 1; break;
        default: break;
    }
}

void hal_SPI_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_2: IEC1bits.SPI2TXIE = 1; break;
        case SPI_3: IEC0bits.SPI3TXIE = 1; break;
        case SPI_4: IEC1bits.SPI4TXIE = 1; break;
        default: break;
    }
}

void hal_SPI_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_2: IEC1bits.SPI2RXIE = 0; break;
        case SPI_3: IEC0bits.SPI3RXIE = 0; break;
        case SPI_4: IEC1bits.SPI4RXIE = 0; break;
        default: break;
    }
}

void hal_SPI_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case SPI_2: IEC1bits.SPI2TXIE = 0; break;
        case SPI_3: IEC0bits.SPI3TXIE = 0; break;
        case SPI_4: IEC1bits.SPI4TXIE = 0; break;
        default: break;
    }
}

void hal_SPI_TxByte(uint8_t channel, uint8_t b) {
    switch (channel) {
        case SPI_2: SPI2BUF = b; break;
        case SPI_3: SPI3BUF = b; break;
        case SPI_4: SPI4BUF = b; break;
        default: break;
    }
}

uint8_t hal_SPI_RxByte(uint8_t channel) {
    switch (channel) {
        case SPI_2: return SPI2BUF;
        case SPI_3: return SPI3BUF;
        case SPI_4: return SPI4BUF;
        default:    return 0;
    }
}

void hal_SPI_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case SPI_2: IFS1bits.SPI2RXIF = 0; break;
        case SPI_3: IFS0bits.SPI3RXIF = 0; break;
        case SPI_4: IFS1bits.SPI4RXIF = 0; break;
        default: break;
    }
}

void hal_SPI_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case SPI_2: IFS1bits.SPI2TXIF = 0; break;
        case SPI_3: IFS0bits.SPI3TXIF = 0; break;
        case SPI_4: IFS1bits.SPI4TXIF = 0; break;
        default: break;
    }
}

uint8_t hal_SPI_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_2: return !SPI2STATbits.SPITBF;
        case SPI_3: return !SPI3STATbits.SPITBF;
        case SPI_4: return !SPI4STATbits.SPITBF;
        default:    return 0;
    }
}

uint8_t hal_SPI_DataAvailable(uint8_t channel) {
    switch (channel) {
        case SPI_2: return !SPI2STATbits.SPIRBE;
        case SPI_3: return !SPI3STATbits.SPIRBE;
        case SPI_4: return !SPI4STATbits.SPIRBE;
        default:    return 0;
    }
}
// SPI2 and UART3 share a interrupt vector so if both are used the ISR servicing will happen in UART3 ISR
#if !defined(USE_UART3) && defined(USE_SPI2)
void __attribute__((vector(_SPI_2_VECTOR), interrupt(), nomips16)) _SPI2_ISR(void) {
    SPI_Rx_Handler(SPI_2);
    hal_SPI_ClearRxIF(SPI_2);
    SPI_Tx_Handler(SPI_2);
}
#endif
// SPI3 and UART1 share a interrupt vector so if both are used the ISR servicing will happen in UART1 ISR
#if !defined(USE_UART1) && defined(USE_SPI3)
void __attribute__((vector(_SPI_3_VECTOR), interrupt(), nomips16)) _SPI3_ISR(void) {
    SPI_Rx_Handler(SPI_3);
    hal_SPI_ClearRxIF(SPI_3);
    SPI_Tx_Handler(SPI_3);
}
#endif
// SPI4 and UART2 share a interrupt vector so if both are used the ISR servicing will happen in UART2 ISR
#if !defined(USE_UART2) && defined(USE_SPI4)
void __attribute__((vector(_SPI_4_VECTOR), interrupt(), nomips16)) _SPI4_ISR(void) {
    SPI_Rx_Handler(SPI_4);
    hal_SPI_ClearRxIF(SPI_4);
    SPI_Tx_Handler(SPI_4);
}
#endif
