#include "render_engine.h"
#include <math.h>
#include "subsystem.h"
#include "uart.h"
#include "terminal.h"

#define M_PI 3.14159265358979323846

// UART helper functions
void changeTerminalCursorLocation(uint8_t channel, uint8_t x, uint8_t y);
void writeTerminalNumber(uint8_t channel, uint8_t number);
void changeTerminalColor(uint8_t channel, uint8_t color);
void writeTerminalBlock(uint8_t channel, uint8_t data);

void Render_Engine_Init() {
    
}

void Render_Engine_RenderFrame(struct world *world, struct camera *camera,
        struct frameBuffer *frame) {
    int bufLength = frame->width * frame->height;
    int halfWidth = frame->width / 2;
    int halfHeight = frame->height / 2;
    rounding_t anglePerPixelHorizontal = (camera->fovHorizontal * M_PI) /
            (frame->width * 180.0);
    rounding_t anglePerPixelVertical = (camera->fovVertical * M_PI) /
            (frame->height * 180.0);
    rounding_t cameraHorizontalAngle = (camera->rotation.z * M_PI) / 180.0;
    rounding_t cameraVerticalAngle = (camera->rotation.y * M_PI) / 180.0;
    struct vector cameraDirection = {cos(cameraHorizontalAngle),
            sin(cameraHorizontalAngle),
            ((cameraVerticalAngle <= -90) || (cameraVerticalAngle >= 90)) ? tan(cameraVerticalAngle) : ((cameraVerticalAngle > 0) - (cameraVerticalAngle < 0)) * 10000};
//    Subsystem_printf("apph: %f\r\n", anglePerPixelHorizontal);
//    Subsystem_printf("appv: %f\r\n", anglePerPixelVertical);
//    Subsystem_printf("cha: %f\r\n", cameraHorizontalAngle);
//    Subsystem_printf("cva: %f\r\n", cameraVerticalAngle);
//    Subsystem_printf("cd: %f %f %f\r\n", cameraDirection.x, cameraDirection.y, cameraDirection.z);
    int i;
    
    // Blank the framebuffer
    for (i = 0; i < bufLength; i++) {
        frame->buffer[i] = world->backgroundColor;
    }
    
    rounding_t dx, dy, dz;
    rounding_t pointDotCamera;
    rounding_t angleHorizontal, angleVertical;
    int16_t indexHorizontal, indexVertical, index;
    // Go through all verticies
    for (i = 0; i < world->numVerticies; i++) {
        // Calculate the offset to the point from the camera
        dx = world->verticies[i].x - camera->location.x;
        dy = world->verticies[i].y - camera->location.y;
        dz = world->verticies[i].z - camera->location.z;
        
        // Make sure the point is in front of the camera
        pointDotCamera = (dx * cameraDirection.x) + (dy * cameraDirection.y) +
                (dz * cameraDirection.z);
//        Subsystem_printf("%f\r\n", pointDotCamera);
        if (pointDotCamera <= 0) {
            continue;
        }
        
        // Horizontal position onscreen
        angleHorizontal = atan(dy / dx);
//        Subsystem_printf("%f\r\n", angleHorizontal);
        indexHorizontal = halfWidth - ((angleHorizontal - cameraHorizontalAngle)
                / anglePerPixelHorizontal);
        
        // Make sure the point is visible
        if ((indexHorizontal < 0) || (indexHorizontal >= frame->width)) {
            continue;
        }
        
        // Vertical position onscreen
        angleVertical = atan(dz / sqrt((dx * dx) + (dy * dy)));
//        Subsystem_printf("%f\r\n", angleVertical);
        indexVertical = halfHeight - ((angleVertical - cameraVerticalAngle) /
                anglePerPixelVertical);
        
        // Make sure the point is visible
        if ((indexVertical < 0) || (indexVertical >= frame->height)) {
            continue;
        }
        
//        Subsystem_printf("%d %d\r\n", indexHorizontal, indexVertical);
        
        // Calculate the framebuffer index
        index = indexHorizontal + (indexVertical * frame->width);
        frame->buffer[index] = BackgroundRed;
    }
}

void Render_Engine_DisplayFrame(uint8_t channel, struct frameBuffer *frame) {
    // Wait for the transmit buffer to clear
    while (UART_IsTransmitting(channel));
    
    // Set the cursor to the origin so the new frame tiles across the old frame
    changeTerminalCursorLocation(channel, 0, 0);
    
    uint16_t i;
    uint8_t lastColor = 0;
    
    // Access the UART through the HAL directly to get around the buffer
    for (i = 0; i < (frame->width * frame->height); i++) {
        // Increase speed by only changing the selected color when needed
        if (lastColor != frame->buffer[i]) {
            // Change the current color
            lastColor = frame->buffer[i];
            changeTerminalColor(channel, frame->buffer[i]);
        }
        
        // Output a color block
        writeTerminalBlock(channel, ' ');
    }
}

void changeTerminalCursorLocation(uint8_t channel, uint8_t x, uint8_t y) {
    writeTerminalBlock(channel, '\e');
    writeTerminalBlock(channel, '[');
    writeTerminalNumber(channel, y + 1);
    writeTerminalBlock(channel, ';');
    writeTerminalNumber(channel, x + 1);
    writeTerminalBlock(channel, 'H');
}

void writeTerminalNumber(uint8_t channel, uint8_t number) {
    uint8_t atDigits = 0;
    
    // Look for hundreds
    uint8_t hundreds = (number / 100) % 10;
    if (hundreds > 0) {
        atDigits = 1;
        writeTerminalBlock(channel, hundreds + '0');
    }
    
    // Look for tens
    uint8_t tens = (number / 10) % 10;
    if ((tens > 0) || atDigits) {
        atDigits = 1;
        writeTerminalBlock(channel, tens + '0');
    }
    
    // Look for ones
    uint8_t ones = number % 10;
    if ((ones > 0) || atDigits) {
        atDigits = 1;
        writeTerminalBlock(channel, ones + '0');
    }
}

void changeTerminalColor(uint8_t channel, uint8_t color) {
    // TODO Support wider range of colors than default terminal colors
    
    writeTerminalBlock(channel, '\e');
    writeTerminalBlock(channel, '[');
    writeTerminalNumber(channel, color);
    writeTerminalBlock(channel, 'm');
}

void writeTerminalBlock(uint8_t channel, uint8_t data) {
    while (!hal_UART_SpaceAvailable(channel));
    hal_UART_TxByte(channel, data);
}
