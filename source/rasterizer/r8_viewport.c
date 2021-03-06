/*
 * r8_viewport.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_viewport.h"
#include "r8_framebuffer.h"
#include "r8_error.h"


void r8_viewport_init(R8Viewport* viewport)
{
    if (viewport != NULL)
    {
        viewport->x             = 0.0f;
        viewport->y             = 0.0f;
        viewport->halfWidth     = 0.0f;
        viewport->halfHeight    = 0.0f;
        viewport->minDepth      = 0.0f;
        viewport->maxDepth      = 1.0f;
        viewport->depthSize     = 1.0f;
    }
    else
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
}
