/* r8_viewport.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_VIEWPORT_H
#define R_8_VIEWPORT_H

#include "r8_types.h"

typedef struct R8Viewport
{
    R8float x;
    R8float y;
    R8float halfWidth;
    R8float halfHeight;
    R8float minDepth;
    R8float maxDepth;
    R8float depthSize;  // (maxDepth - minDepth)
}R8Viewport;

R8void r8InitViewport(R8Viewport* viewport);

#endif