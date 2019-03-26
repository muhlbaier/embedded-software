/**
 * @defgroup render_engine Render Engine
 * @file render_engine.h
 * Created on March 21, 2019
 * @author Nate Hoffman
 * 
 * Use this module to perform 3D rendering on the processor. Processing can take
 * quite some time and is blocking. Little error checking is performed yet.
 * 
 * Calculations may not be 100% accurate, but is sufficient for the character
 * terminal based output. Normal usage is:
 * - Render_Engine_RenderFrame() to populate a framebuffer with the image
 * - Render_Engine_DisplayFrame() to send a framebuffer out over a UART channel
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
 * @param world - World data that contains the list of triangles in 3D space to
 * render.
 * @param camera - Camera data that contains the location and direction of the
 * camera.
 * @param framebuffer - Output of the rendering process populates an existing
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
 * @param channel - UART channel to output the framebuffer over.
 * @param framebuffer - Framebuffer to display on the console.
 */
void Render_Engine_DisplayFrame(uint8_t channel, framebuffer_t *framebuffer);

/** @} */
#endif // RENDER_ENGINE_H
