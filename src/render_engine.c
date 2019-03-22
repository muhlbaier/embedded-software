#include "render_engine.h"
#include <math.h>
#include "subsystem.h"
#include "uart.h"
#include "terminal.h"

void Render_Engine_Init() {
    
}

void Render_Engine_Render_Frame(struct world *world, struct camera *camera,
        struct frameBuffer *frame) {
//    Subsystem_printf("fov: %d\r\n", camera->fovHorizontal);
    int bufLength = frame->width * frame->height;
    int i;
    for (i = 0; i < bufLength; i++) {
        frame->buffer[i] = BackgroundRed;
    }
    frame->buffer[0] = BackgroundBlue;
    frame->buffer[1] = BackgroundGreen;
    frame->buffer[2] = BackgroundWhite;
}

void Render_Engine_Display_Frame(struct frameBuffer *frame) {
    // Wait for the transmit buffer to clear
    while (UART_IsTransmitting(SUBSYSTEM_UART));
    
    uint16_t i;
    uint8_t lastColor = 0;
    
    // Access the UART through the HAL directly to get around the buffer
    for (i = 0; i < (frame->width * frame->height); i++) {
        // Increase speed by only changing the selected color when needed
        if (lastColor != frame->buffer[i]) {
            // Change the current color
            lastColor = frame->buffer[i];
            
            while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
            hal_UART_TxByte(SUBSYSTEM_UART, 0x1B);
            while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
            hal_UART_TxByte(SUBSYSTEM_UART, '[');
            while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
            hal_UART_TxByte(SUBSYSTEM_UART, (frame->buffer[i] / 10) + '0');
            while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
            hal_UART_TxByte(SUBSYSTEM_UART, (frame->buffer[i] % 10) + '0');
            while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
            hal_UART_TxByte(SUBSYSTEM_UART, 'm');
        }
        
        // Output a color block
        while (!hal_UART_SpaceAvailable(SUBSYSTEM_UART));
        hal_UART_TxByte(SUBSYSTEM_UART, ' ');
    }
}
