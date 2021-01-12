/*
 * r8_framebuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_FRAMEBUFFER_H
#define R8_FRAMEBUFFER_H


#include "r8_pixel.h"
#include "r8_macros.h"
#include "r8_raster_vertex.h"


/// Raster scanline side structure
typedef struct R8ScalineSide
{
    R8int       offset; // Pixel offset in framebuffer
    R8interp    z;
    R8interp    u;
    R8interp    v;
}
R8ScalineSide;

/// Framebuffer structure
typedef struct R8FrameBuffer
{
    R8uint              width;
    R8uint              height;
    #ifdef R8_MERGE_COLOR_AND_DEPTH_BUFFERS
    R8Pixel*           pixels;
    #else
    R8ubyte*            colors;
    R8DepthBuffer         depths;
    #endif
    R8ScalineSide* scanlinesStart; // Start offsets to scanlines
    R8ScalineSide* scanlinesEnd;   // End offsets to scanlines
}
R8FrameBuffer;


R8FrameBuffer* r8_framebuffer_create(R8uint width, R8uint height);
void r8_framebuffer_delete(R8FrameBuffer* frameBuffer);

void r8_framebuffer_clear(R8FrameBuffer* frameBuffer, R8float clearDepth, R8bitfield clearFlags);

/// Sets the start and end offsets of the specified scanlines.
void r8_framebuffer_setup_scanlines(
    R8FrameBuffer* frameBuffer, R8ScalineSide* sides, R8RasterVertex start, R8RasterVertex end
);

R8_INLINE void r8_framebuffer_plot(R8FrameBuffer* frameBuffer, R8uint x, R8uint y, R8ColorBuffer colorIndex)
{
    #ifdef R8_MERGE_COLOR_AND_DEPTH_BUFFERS
    frameBuffer->pixels[y * frameBuffer->width + x].colorIndex = colorIndex;
    #else
    frameBuffer->colors[y * frameBuffer->width + x] = colorIndex;
    #endif
}


#endif
