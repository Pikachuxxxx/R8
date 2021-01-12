/*
 * r8_framebuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_framebuffer.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_state_machine.h"
#include "r8_color_palette.h"

#include <stdlib.h>
#include <string.h>


R8FrameBuffer* r8_framebuffer_create(R8uint width, R8uint height)
{
    if (width == 0 || height == 0)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create framebuffer
    R8FrameBuffer* frameBuffer = R8_MALLOC(R8FrameBuffer);

    frameBuffer->width = width;
    frameBuffer->height = height;
    frameBuffer->pixels = R8_CALLOC(R8Pixel, width*height);
    frameBuffer->scanlinesStart = R8_CALLOC(R8ScalineSide, height);
    frameBuffer->scanlinesEnd = R8_CALLOC(R8ScalineSide, height);

    // Initialize framebuffer
    memset(frameBuffer->pixels, 0, width*height*sizeof(R8Pixel));

    r8_ref_add(frameBuffer);

    return frameBuffer;
}

void r8_framebuffer_delete(R8FrameBuffer* frameBuffer)
{
    if (frameBuffer != NULL)
    {
        r8_ref_release(frameBuffer);

        R8_FREE(frameBuffer->pixels);
        R8_FREE(frameBuffer->scanlinesStart);
        R8_FREE(frameBuffer->scanlinesEnd);
        R8_FREE(frameBuffer);
    }
}

void r8_framebuffer_clear(R8FrameBuffer* frameBuffer, R8float clearDepth, R8bitfield clearFlags)
{
    if (frameBuffer != NULL && frameBuffer->pixels != NULL)
    {
        // Convert depth (32-bit) into pixel depth (16-bit or 8-bit)
        R8DepthBuffer depth = r8_pixel_write_depth(clearDepth);

        // Get clear color from state machine (and optionally its color index)
        R8ColorBuffer clearColor = R8_STATE_MACHINE.clearColor;

        // Iterate over the entire framebuffer
        R8Pixel* dst = frameBuffer->pixels;
        R8Pixel* dstEnd = dst + (frameBuffer->width * frameBuffer->height);

        if ((clearFlags & R8_COLOR_BUFFER_BIT) != 0 && (clearFlags & R8_DEPTH_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->colorIndex = clearColor;
                dst->depth      = depth;
                ++dst;
            }
        }
        else if ((clearFlags & R8_COLOR_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->colorIndex = clearColor;
                ++dst;
            }
        }
        else if ((clearFlags & R8_DEPTH_BUFFER_BIT) != 0)
        {
            while (dst != dstEnd)
            {
                dst->depth = depth;
                ++dst;
            }
        }
    }
    else
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
}

void r8_framebuffer_setup_scanlines(
    R8FrameBuffer* frameBuffer, R8ScalineSide* sides, R8RasterVertex start, R8RasterVertex end)
{
    R8int pitch = (R8int)frameBuffer->width;
    R8int len = end.y - start.y;

    if (len <= 0)
    {
        sides[start.y].offset = start.y * pitch + start.x;
        return;
    }

    // Compute offsets (need doubles for offset for better r8ecision, because the range is larger)
    R8double offsetStart = (R8double)(start.y * pitch + start.x);
    R8double offsetEnd   = (R8double)(end.y * pitch + end.x);
    R8double offsetStep  = (offsetEnd - offsetStart) / len;

    R8interp zStep       = (end.z - start.z) / len;
    R8interp uStep       = (end.u - start.u) / len;
    R8interp vStep       = (end.v - start.v) / len;

    // Fill scanline sides
    R8ScalineSide* sidesEnd = &(sides[end.y]);

    for (sides += start.y; sides <= sidesEnd; ++sides)
    {
        // Setup scanline side
        sides->offset = (R8int)(offsetStart + 0.5);
        sides->z = start.z;
        sides->u = start.u;
        sides->v = start.v;

        // Next step
        offsetStart += offsetStep;
        start.z += zStep;
        start.u += uStep;
        start.v += vStep;
    }
}

