#include "render_engine.h"
#include <math.h>
#include "subsystem.h"
#include "uart.h"
#include "terminal.h"

#define M_PI 3.14159265358979323846

// Rendering helper functions
point_t pointToScreen(vector_t point, vector_t camera,
        rounding_t camHAngle, rounding_t camVAngle,
        rounding_t angleHPixel, rounding_t angleVPixel,
        uint8_t halfWidth, uint8_t halfHeight);
void paintPixel(frameBuffer_t *frame, uint16_t x, uint16_t y, uint8_t color);
void paintPixelf(frameBuffer_t *frame, rounding_t x, rounding_t y, uint8_t color);

// UART helper functions
void changeTerminalCursorLocation(uint8_t channel, uint8_t x, uint8_t y);
void writeTerminalNumber(uint8_t channel, uint8_t number);
void changeTerminalColor(uint8_t channel, uint8_t color);
void writeTerminalBlock(uint8_t channel, uint8_t data);

void Render_Engine_Init() {
    
}

void Render_Engine_RenderFrame(world_t *world, camera_t *camera, frameBuffer_t *frame) {
    uint16_t bufLength = frame->width * frame->height;
    uint8_t halfWidth = frame->width / 2;
    uint8_t halfHeight = frame->height / 2;
    rounding_t anglePerPixelHorizontal = (camera->fovHorizontal * M_PI) /
            (frame->width * 180.0);
    rounding_t anglePerPixelVertical = (camera->fovVertical * M_PI) /
            (frame->height * 180.0);
    rounding_t cameraHorizontalAngle = (camera->rotation.z * M_PI) / 180.0;
    rounding_t cameraVerticalAngle = (camera->rotation.y * M_PI) / 180.0;
    vector_t cameraDirection = {cos(cameraHorizontalAngle),
            sin(cameraHorizontalAngle),
            ((cameraVerticalAngle <= -90) || (cameraVerticalAngle >= 90)) ? tan(cameraVerticalAngle) : ((cameraVerticalAngle > 0) - (cameraVerticalAngle < 0)) * 10000};
//    Subsystem_printf("apph: %f\r\n", anglePerPixelHorizontal);
//    Subsystem_printf("appv: %f\r\n", anglePerPixelVertical);
//    Subsystem_printf("cha: %f\r\n", cameraHorizontalAngle);
//    Subsystem_printf("cva: %f\r\n", cameraVerticalAngle);
//    Subsystem_printf("cd: %f %f %f\r\n", cameraDirection.x, cameraDirection.y, cameraDirection.z);
    uint16_t i;
    
    // Blank the framebuffer
    for (i = 0; i < bufLength; i++) {
        frame->buffer[i] = world->backgroundColor;
    }
    
    // Go through all triangles
    point_t p1, p2, p3;
    uint8_t leftSel, rightSel;
    point_t left, right, center;
    for (i = 0; i < world->numTriangles; i++) {
        // Calculate the screen coordinates
        p1 = pointToScreen(world->triangles[i].p1, camera->location,
                cameraHorizontalAngle, cameraVerticalAngle,
                anglePerPixelHorizontal, anglePerPixelVertical,
                halfWidth, halfHeight);
        p2 = pointToScreen(world->triangles[i].p2, camera->location,
                cameraHorizontalAngle, cameraVerticalAngle,
                anglePerPixelHorizontal, anglePerPixelVertical,
                halfWidth, halfHeight);
        p3 = pointToScreen(world->triangles[i].p3, camera->location,
                cameraHorizontalAngle, cameraVerticalAngle,
                anglePerPixelHorizontal, anglePerPixelVertical,
                halfWidth, halfHeight);
        
        // Determine the left point of the triangle
        left = p1;
        leftSel = 1;
        if (left.x > p2.x) {
            left = p2;
            leftSel = 2;
        }
        if (left.x > p3.x) {
            left = p3;
            leftSel = 3;
        }
        
        // Determine the right point of the triangle
        right = p3;
        rightSel = 3;
        if (((right.x < p2.x) || (leftSel == 3)) && (leftSel != 2)) {
            right = p2;
            rightSel = 2;
        }
        if ((right.x < p1.x) && (leftSel != 1)) {
            right = p1;
            rightSel = 1;
        }
        
        // Determine the center point of the triangle
        if ((leftSel + rightSel) == 3) {
            center = p3;
        } else if ((leftSel + rightSel) == 4) {
            center = p2;
        } else if ((leftSel + rightSel) == 5) {
            center = p1;
        }
        
        Subsystem_printf("left: %f %f\r\n", left.x, left.y);
        Subsystem_printf("center: %f %f\r\n", center.x, center.y);
        Subsystem_printf("right: %f %f\r\n", right.x, right.y);
        
        // Determine the number of triangles to paint
        if ((left.x == center.x) && (center.x == right.x)) {
            // One vertical line
            rounding_t max = p1.y;
            if (max < p2.y) {
                max = p2.y;
            }
            if (max < p3.y) {
                max = p3.y;
            }
            rounding_t min = p1.y;
            if (min > p2.y) {
                min = p2.y;
            }
            if (min > p3.y) {
                min = p3.y;
            }
            
            rounding_t y;
            for (y = max; y > min; y--) {
                paintPixelf(frame, center.x, y, world->triangles[i].color);
            }
        } else if ((left.x == center.x) || (center.x == right.x) || (left.x == right.x)) {
            // Two points are in line vertically
            paintPixelf(frame, left.x, left.y, Red);
            paintPixelf(frame, center.x, center.y, Green);
            paintPixelf(frame, right.x, right.y, Magenta);
        } else {
            // Points are not directly in line vertically
            paintPixelf(frame, left.x, left.y, Red);
            paintPixelf(frame, center.x, center.y, Green);
            paintPixelf(frame, right.x, right.y, Magenta);
        }
    }
}

void Render_Engine_DisplayFrame(uint8_t channel, frameBuffer_t *frame) {
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

// Rendering helper functions
point_t pointToScreen(vector_t point, vector_t camera,
        rounding_t camHAngle, rounding_t camVAngle,
        rounding_t angleHPixel, rounding_t angleVPixel,
        uint8_t halfWidth, uint8_t halfHeight) {
    rounding_t dx, dy, dz;
    rounding_t angleHorizontal, angleVertical;
    point_t screen;
    
    // TODO Fix calculations when the point is behind the camera
    
    // Calculate the offset to the point from the camera
    dx = point.x - camera.x;
    dy = point.y - camera.y;
    dz = point.z - camera.z;
    
    // Horizontal position onscreen
    angleHorizontal = atan(dy / dx);
    screen.x = halfWidth - ((angleHorizontal - camHAngle) / angleHPixel);
    
    // Vertical position onscreen
    angleVertical = atan(dz / sqrt((dx * dx) + (dy * dy)));
    screen.y = halfHeight - ((angleVertical - camVAngle) / angleVPixel);
    
    return screen;
}

void paintPixel(frameBuffer_t* frame, uint16_t x, uint16_t y, uint8_t color) {
    frame->buffer[x + (y * frame->width)] = color;
}

void paintPixelf(frameBuffer_t* frame, rounding_t x, rounding_t y, uint8_t color) {
    paintPixel(frame, (uint16_t) x, (uint16_t) y, color);
}

// UART helper functions
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
