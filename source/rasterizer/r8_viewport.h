/*
 * r8_viewport.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_VIEWPORT_H
#define R8_VIEWPORT_H


#include "r8_types.h"


 // The viewport has only floats, because they will be
 // used to transform the raster_vertex coordinates.
typedef struct R8Viewport
{
    R8float x;
    R8float y;
    R8float halfWidth;
    R8float halfHeight;
    R8float minDepth;
    R8float maxDepth;
    R8float depthSize;  // (maxDepth - minDepth)
}
R8Viewport;


void r8_viewport_init(R8Viewport* viewport);


#endif
