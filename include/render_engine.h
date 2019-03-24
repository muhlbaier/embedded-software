/**
 * @defgroup render_engine
 * @file render_engine.h
 * @author Nate Hoffman
 */

#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <stdint.h>

// TODO Standardize variable names (e.g. my_var_x)

// Precision of the engine
typedef float rounding_t;

// Colors
enum pixelColor {
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

typedef struct frameBuffer {
    uint16_t width;
    uint16_t height;
    uint8_t *buffer;
} frameBuffer_t;

void Render_Engine_Init();
void Render_Engine_RenderFrame(world_t *world, camera_t *camera, frameBuffer_t *frameBuffer);
void Render_Engine_DisplayFrame(uint8_t channel, frameBuffer_t *frameBuffer);

#endif // RENDER_ENGINE_H
