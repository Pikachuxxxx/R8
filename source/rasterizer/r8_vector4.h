/*
 * r8_vector4.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#ifndef R8_VECTOR4_H
#define R8_VECTOR4_H


#include "r8_types.h"


typedef struct R8Vector4
{
    R8float x;
    R8float y;
    R8float z;
    R8float w;
}
R8Vector4;


R8_INLINE R8float r8_vector4_dot(R8Vector4 a, R8Vector4 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}


#endif
