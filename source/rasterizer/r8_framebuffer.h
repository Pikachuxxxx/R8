/* r8_framebuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_FRAME_BUFFER_H
#define R_8_FRAME_BUFFER_H

#include "r8_pixel.h"
#include "r8_macros.h"
#include "r8_config.h"
#include "r8_raster_vertex.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_color_palette.h"

typedef struct R8SideScanline
{
    R8int       pixelOffset; // Pixel offset in framebuffer
    R8interp    z;
    R8interp    u;
    R8interp    v;
}R8SideScanline;

typedef struct R8FrameBuffer
{
    R8uint width;
    R8uint height;
    #ifdef R8_MERGE_COLOR_AND_DEPTH_BUFFERS
    R8Pixel* pixels;
    #else
    R8ColorBuffer* colors;
    R8DepthBuffer depths;
    #endif // R8_MERGE_COLOR_AND_DEPTH_BUFFERS
    R8SideScanline* scanlinesStart; // Start offsets to scan lines
    R8SideScanline* scanlinesEnd;   // End offsets to scan lines
}R8FrameBuffer;

R8FrameBuffer* r8FrameBufferGenerate(R8uint width, R8uint height);
R8void r8FrameBufferDelete(R8FrameBuffer* framebuffer);

R8void r8FrameBufferClear(R8FrameBuffer* framebuffer, R8float clearDepth, R8bit clearFlags);

R8void r8FrameBufferSetupScanlines(R8FrameBuffer* framebuffer, R8SideScanline* sides, R8RasterVertex* start, R8RasterVertex* end);

inline R8void r8FrameBufferPlot(R8FrameBuffer* framebuffer, R8uint x, R8uint y, R8ColorBuffer colorbuffer)
{
    #ifdef R8_MERGE_COLOR_AND_DEPTH_BUFFERS
    framebuffer->pixels[y * framebuffer->width + x].colorBuffer = colorbuffer
    #else
    framebuffer->colors[y * framebuffer->width + x] = colorbuffer;
    #endif // R8_MERGE_COLOR_AND_DEPTH_BUFFERS

}

#endif