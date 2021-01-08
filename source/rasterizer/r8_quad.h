/* r8_quad.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_QUAD_H
#define R_8_QUAD_H

#include "r8_types.h"

typedef struct R8Quad
{
    R8int left;
    R8int top;
    R8int right;
    R8int bottom;
}R8Quad;

R8void InitQuad(R8Quad* quad);

#endif