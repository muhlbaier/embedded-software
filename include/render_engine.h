/**
 * @defgroup render_engine Render Engine
 * @file render_engine.h
 * @author Nate Hoffman
 * @version 1
 * 
 * Created on March 21, 2019
 * 
 * Use this module to perform 3D rendering on the processor. Processing can take
 * quite some time and is blocking. Little error checking is performed yet.
 * 
 * Calculations may not be 100% accurate, but is sufficient for the character
 * terminal based output. Normal usage is:
 * - Render_Engine_RenderFrame() to populate a framebuffer with the image
 * - Render_Engine_DisplayFrame() to send a framebuffer out over a UART channel
 * 
 * @section Example
 * 
 * The following code can be used to display a pyramid onscreen. The camera
 * rotates around it to show all the sides. This example can easily be modified
 * to rotate the camera inside the pyramid.
 * 
 @code
#include "project_settings.h"
#include "subsystem.h"
#include "task.h"
#include "uart.h"
#include "terminal.h"
#include "hal_general.h"
#include <math.h>
#include "render_engine.h"

int main() {
    DisableInterrupts();
    UART_Init(SUBSYSTEM_UART);
    UART_ReconfigureBaud(SUBSYSTEM_UART, 115200);
    EnableInterrupts();
    
    Terminal_HideCursor(SUBSYSTEM_UART);
    Terminal_SetColor(SUBSYSTEM_UART, BackgroundBlack);
    Terminal_ClearScreen(SUBSYSTEM_UART);
    
    // Test render engine
    world_t worldA;
    camera_t cam;
    cam.fovHorizontal = 100;
    cam.fovVertical = 75;
    cam.location.x = 0;
    cam.location.y = 0;
    cam.location.z = 5;
    cam.rotation.x = 0;
    cam.rotation.y = -50;
    cam.rotation.z = 0;
    
    framebuffer_t buf;
    buf.width = 80;
    buf.height = 24;
    uint8_t bufAlloc[buf.width * buf.height];
    buf.buffer = bufAlloc;
    
    worldA.numTriangles = 4;
    worldA.backgroundColor = Blue;
    vector_t backTop = {0, 0, 3};
    vector_t back1 = {-1, -1, 0};
    vector_t back2 = {-1, 1, 0};
    triangle_t back = {backTop, back1, back2, Red};
    vector_t rightTop = {0, 0, 3};
    vector_t right1 = {-1, 1, 0};
    vector_t right2 = {1, 1, 0};
    triangle_t right = {rightTop, right1, right2, Cyan};
    vector_t leftTop = {0, 0, 3};
    vector_t left1 = {-1, -1, 0};
    vector_t left2 = {1, -1, 0};
    triangle_t left = {leftTop, left1, left2, Magenta};
    vector_t frontTop = {0, 0, 3};
    vector_t front1 = {1, 1, 0};
    vector_t front2 = {1, -1, 0};
    triangle_t front = {frontTop, front1, front2, Green};
    triangle_t triangles[worldA.numTriangles];
    triangles[0] = back;
    triangles[1] = left;
    triangles[2] = right;
    triangles[3] = front;
    worldA.triangles = triangles;
    
    int16_t rot = 180;
    while (true) {
        cam.rotation.z = rot;
        cam.location.x = 3 * -cos(rot  * (3.14159 / 180.0));
        cam.location.y = 3 * sin(-rot  * (3.14159 / 180.0));
        Render_Engine_RenderFrame(&worldA, &cam, &buf);
        Render_Engine_DisplayFrame(SUBSYSTEM_UART, &buf);
        if (rot <= -179) {
            rot = 180;
        } else {
            rot--;
        }
    }
    
    while (1) {
        SystemTick();
    }
    
    return 0;
}
@endcode
 * @{
 */

#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <stdint.h>

// Precision of the engine
typedef float rounding_t;

// Colors
enum pixel_color {
    Black = 40,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
};

typedef struct point {
    rounding_t x;
    rounding_t y;
} point_t;

typedef struct vector {
    rounding_t x;
    rounding_t y;
    rounding_t z;
} vector_t;

typedef struct camera {
    uint16_t status;
    vector_t location;
    vector_t rotation;
    int fovHorizontal;
    int fovVertical;
} camera_t;

typedef struct triangle {
    vector_t p1;
    vector_t p2;
    vector_t p3;
    uint8_t color;
} triangle_t;

typedef struct world {
    uint8_t backgroundColor;
    uint16_t numTriangles;
    triangle_t *triangles;
} world_t;

typedef struct framebuffer {
    uint16_t width;
    uint16_t height;
    uint8_t *buffer;
} framebuffer_t;

/** @brief Render a frame
 * 
 * Renders a frame of data based on a list of triangles in the world object.
 * Make sure the array in the framebuffer has been created as this will not
 * create the needed array for you. This method is blocking during the rendering
 * process.
 * 
 * @param world World data that contains the list of triangles in 3D space to
 * render.
 * @param camera Camera data that contains the location and direction of the
 * camera.
 * @param framebuffer Output of the rendering process populates an existing
 * framebuffer.
 */
void Render_Engine_RenderFrame(world_t *world, camera_t *camera, framebuffer_t *framebuffer);

/** @brief Display a frame
 * 
 * Output the contents of a framebuffer over a UART channel. Before writing,
 * this function makes sure the UART buffer is empty. As there is so much data
 * sent over UART, buffers get in the way of operation. This directly accesses
 * the HAL UART code to get around the buffer of the UART code. This method is
 * blocking during the writing process.
 * 
 * @param channel UART channel to output the framebuffer over.
 * @param framebuffer Framebuffer to display on the console.
 */
void Render_Engine_DisplayFrame(uint8_t channel, framebuffer_t *framebuffer);

/** @} */
#endif // RENDER_ENGINE_H
