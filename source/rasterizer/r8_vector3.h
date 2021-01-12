/*
 * r8_vector3.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_VECTOR3_H
#define R8_VECTOR3_H


#include "r8_types.h"


typedef struct R8Vector3
{
    R8float x;
    R8float y;
    R8float z;
}
R8Vector3;

void r8_vector_normalize3(R8Vector3* vec);

R8_INLINE R8float r8_vector3_dot(R8Vector3 a, R8Vector3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


#endif
