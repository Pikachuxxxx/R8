/*
 * r8_vector2.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_VECTOR2_H
#define R8_VECTOR2_H


#include "r8_types.h"


typedef struct R8Vector2
{
    R8float x;
    R8float y;
}
R8Vector2;


R8_INLINE R8float r8_vector2_dot(R8Vector2 a, R8Vector2 b)
{
    return a.x*b.x + a.y*b.y;
}


#endif
