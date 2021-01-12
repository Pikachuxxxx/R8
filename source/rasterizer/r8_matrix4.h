/*
 * r8_matrix4.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#ifndef R8_MATRIX4_H
#define R8_MATRIX4_H


#include "r8_types.h"
#include "r8_vector3.h"
#include "r8_vector4.h"


typedef struct R8Matrix4
{
    // column (c) and row (r)
    //      c  r
    R8float m[4][4];
}
R8Matrix4;


void r8_matrix_load_identity(R8Matrix4* matrix);
void r8_matrix_copy(R8Matrix4* dst, const R8Matrix4* src);

void r8_matrix_mul_float3(R8float* result, const R8Matrix4* lhs, const R8float* rhs);
void r8_matrix_mul_float4(R8float* result, const R8Matrix4* lhs, const R8float* rhs);

void r8_matrix_mul_vector3(R8Vector3* result, const R8Matrix4* lhs, const R8Vector3* rhs);
void r8_matrix_mul_vector4(R8Vector4* result, const R8Matrix4* lhs, const R8Vector4* rhs);

void r8_matrix_mul_matrix(R8Matrix4* result, const R8Matrix4* lhs, const R8Matrix4* rhs);

void r8_matrix_translate(R8Matrix4* result, R8float x, R8float y, R8float z);
void r8_matrix_rotate(R8Matrix4* result, R8float x, R8float y, R8float z, R8float angle);
void r8_matrix_scale(R8Matrix4* result, R8float x, R8float y, R8float z);

void r8_matrix_build_perspective(R8Matrix4* result, R8float aspectRatio, R8float nearPlane, R8float farPlane, R8float fov);
void r8_matrix_build_orthogonal(R8Matrix4* result, R8float width, R8float height, R8float nearPlane, R8float farPlane);


#endif
