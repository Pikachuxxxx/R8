/*
 * r8_vector3.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#include "r8_vector3.h"
#include "r8_external_math.h"

#include <math.h>


void r8_vector_normalize3(R8Vector3* vec)
{
    R8float len = R8_SQ(vec->x) + R8_SQ(vec->y) + R8_SQ(vec->z);
    if (len != 0.0f && len != 1.0f)
    {
        len = R8_INV_SQRT(len);
        vec->x *= len;
        vec->y *= len;
        vec->z *= len;
    }
}

