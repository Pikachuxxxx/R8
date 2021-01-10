/* r8_framebuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_framebuffer.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_state_machine.h"
#include "r8_color_palette.h"

#include <string.h> // memset

R8FrameBuffer* r8FrameBufferGenerate(R8uint width, R8uint height)
{
    if (width == 0 || height == 0)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    R8FrameBuffer* framebuffer = R8_MALLOC(R8FrameBuffer);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->pixels = R8_CALLOC(R8Pixel, width * height);
    framebuffer->scanlinesStart = R8_CALLOC(R8SideScanline, height);
    framebuffer->scanlinesEnd = R8_CALLOC(R8SideScanline, height);

    memset(framebuffer->pixels, 0, sizeof(R8Pixel) * width * height);

    return framebuffer;
}

R8void r8FrameBufferDelete(R8FrameBuffer* framebuffer)
{
    if (framebuffer != NULL)
    {
        R8_FREE(framebuffer->pixels);
        R8_FREE(framebuffer->scanlinesStart);
        R8_FREE(framebuffer->scanlinesEnd);
        R8_FREE(framebuffer);
    }
}

R8void r8FrameBufferClear(R8FrameBuffer* framebuffer, R8float clearDepth, R8bit clearFlags)
{
    if (framebuffer != NULL && framebuffer->pixels != NULL)
    {
        // Convert depth (32-bit) into pixel depth (16-bit or 8-bit)
        R8DepthBuffer depth = r8WriteRealDepthToBuffer(clearDepth);

        // Get clear color from state machine (and optionally its color index)
        //PRcolorindex clearColor = PR_STATE_MACHINE.clearColor;

        // Iterate over the entire framebuffer
        R8Pixel* dst = framebuffer->pixels;
        R8Pixel* dstEnd = dst + (framebuffer->width * framebuffer->height);

        if ((clearFlags & R8_COLOR_BUFFER_BIT) != 0 && (clearFlags & R8_DEPTH_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->colorBuffer = 0xff;    // clear color
                dst->depthBuffer = depth;
                ++dst;
            }
        }
        else if ((clearFlags & R8_COLOR_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->colorBuffer = 0xff;    // clear color
                ++dst;
            }
        }
        else if ((clearFlags & R8_DEPTH_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->depthBuffer = depth;
                ++dst;
            }
        }
    }
    else
        R8_ERROR(R8_ERROR_NULL_POINTER);
}

R8void r8FrameBufferSetupScanlines(R8FrameBuffer* framebuffer, R8SideScanline* sides, R8RasterVertex* start, R8RasterVertex* end)
{
    R8int pitch = (R8int)framebuffer->width;
    R8int len = end->y - start->y;

    if (len <= 0)
    {
        sides[start->y].pixelOffset = start->y * pitch + start->x;
        return;
    }

    // Compute offsets (need doubles for offset for better precision, because the range is larger)
    R8double offsetStart = (R8double)(start->y * pitch + start->x);
    R8double offsetEnd = (R8double)(end->y * pitch + end->x);
    R8double offsetStep = (offsetEnd - offsetStart) / len;

    R8interp zStep = (end->z - start->z) / len;
    R8interp uStep = (end->u - start->u) / len;
    R8interp vStep = (end->v - start->v) / len;

    // Fill scan line sides
    R8SideScanline* sidesEnd = &(sides[end->y]);

    for (sides += start->y; sides <= sidesEnd; ++sides)
    {
        // Setup scan line side
        sides->pixelOffset = (R8int)(offsetStart + 0.5);
        sides->z = start->z;
        sides->u = start->u;
        sides->v = start->v;

        // Next step
        offsetStart += offsetStep;
        start->z += zStep;
        start->u += uStep;
        start->v += vStep;
    }
}
