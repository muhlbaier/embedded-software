/**
 * @defgroup render_engine
 * @file render_engine.h
 * @author Nate Hoffman
 */

#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <stdint.h>

// Precision of the engine
typedef float rounding_t;

struct vector {
    rounding_t x;
    rounding_t y;
    rounding_t z;
};

struct camera {
    uint16_t status;
    struct vector location;
    struct vector rotation;
    int fovHorizontal;
    int fovVertical;
};

struct world {
    uint8_t backgroundColor;
    int numVerticies;
    struct vector *verticies;
};

struct frameBuffer {
    uint16_t width;
    uint16_t height;
    uint8_t *buffer;
};

void Render_Engine_Init();
void Render_Engine_RenderFrame(struct world *world, struct camera *camera,
        struct frameBuffer *frameBuffer);
void Render_Engine_DisplayFrame(uint8_t channel, struct frameBuffer *frameBuffer);

#endif // RENDER_ENGINE_H
