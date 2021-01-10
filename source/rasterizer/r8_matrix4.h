/* r8_matrix4.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_MATRIX_4_H
#define R_8_MATRIX_4_H

#include "r8_structs.h"

typedef struct R8Mat4
{
    // column (c) and row (r)
    //               c  r
    R8float elements[4][4];
}R8Mat4;

R8void r8Mat4LoadIdentity(R8Mat4* matrix);
R8void r8Mat4Copy(R8Mat4* dst, R8Mat4* src);

R8void r8Mat4MultiplyFloat3(R8float* result, const R8Mat4* lhs, const R8float* rhs);
R8void r8Mat4MultiplyFloat4(R8float* result, const R8Mat4* lhs, const R8float* rhs);

R8void r8Mat4MultiplyVector3(R8Vec3* result, const R8Mat4* lhs, R8Vec3* rhs);
R8void r8Mat4MultiplyVector4(R8Vec4* result, const R8Mat4* lhs, R8Vec4* rhs);

R8void r8Mat4MultiplyMatrix(R8Mat4* result, const R8Mat4* lhs, R8Mat4* rhs);

R8void r8Mat4Translation(R8Mat4* result, R8float x, R8float y, R8float z);
R8void r8Mat4Rotate(R8Mat4* result, R8float x, R8float y, R8float z, R8float angle);
R8void r8Mat4Scale(R8Mat4* result, R8float x, R8float y, R8float z);

R8void r8Mat4Perspective(R8Mat4* result, R8float fov, R8float aspectRatio, R8float nearPlane, R8float farPlane);
R8void r8Mat4Orthographic(R8Mat4* result, R8float left, R8float right, R8float top, R8float bottom, R8float nearPlane, R8float farPlane);

#endif