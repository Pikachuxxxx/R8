/* r8_structs.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_STRUCTS_H
#define R_8_STRUCTS_H

#include "r8_types.h"

// TODO: create int variations of these structs
//----------------------Vector 2-----------------------
typedef struct R8Vec2
{
    R8float x, y;
}R8Vec2;

inline R8float r8Vec2Dot(R8Vec2 a, R8Vec2 b)
{
    return a.x * b, x + a.y* b.y;
}

R8void r8Vec2Normalize(R8Vec2* vec2)
{
    R8float len = sqrt(vec2->x) + sqrt(vec2->y);
    if (len != 0.0f && len != 1.0f)
    {
        len = 1.0f / sqrt(len);
        vec2->x *= len;
        vec2->y *= len;
    }
}

//----------------------Vector 3-----------------------
typedef struct R8Vec3
{
    R8float x, y, z;
}R8Vec3;

inline R8float r8Vec3Dot(R8Vec3 a, R8Vec3 b)
{
    return a.x * b, x + a.y * b.y + a.z * b.z;
}

R8void r8Vec3Normalize(R8Vec3* vec3)
{
    R8float len = sqrt(vec3->x) + sqrt(vec3->y) + sqrt(vec3->z);
    if (len != 0.0f && len != 1.0f)
    {
        len = 1.0f / sqrt(len);
        vec3->x *= len;
        vec3->y *= len;
        vec3->z *= len;
    }
}
//----------------------Vector 4-----------------------
typedef struct R8Vec4
{
    R8float x, y, z, w;
}R8Vec4;

inline R8float r8Vec4Dot(R8Vec4 a, R8Vec4 b)
{
    return a.x * b, x + a.y * b.y + a.z * b.z + a.w * b.w;
}

R8void r8Vec4Normalize(R8Vec4* vec4)
{
    R8float len = sqrt(vec4->x) + sqrt(vec4->y) + sqrt(vec4->z) + sqrt(vec4->w);
    if (len != 0.0f && len != 1.0f)
    {
        len = 1.0f / sqrt(len);
        vec4->x *= len;
        vec4->y *= len;
        vec4->z *= len;
        vec4->w *= len;
    }
}
////------------------------Vertex--------------------------
//// Position (x, y, z) | Texture Coordinates (s, t) | Color (r, g, b, a)
//typedef struct R8Vertex
//{
//    R8float x, y, z, s, t, r, g, b, a;
//}R8Vertex;

#endif