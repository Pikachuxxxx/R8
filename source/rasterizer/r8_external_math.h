/* r8_external_math.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_EXTERNAL_MATH_H
#define R_8_EXTERNAL_MATH_H

#include "r8_types.h"
#include "r8_config.h"
#include "r8_constants.h"

#define R8_MAX(a, b)        ((a) < (b) ? (b) : (a))
#define R8_MIN(a, b)        ((a) < (b) ? (a) : (b))

#define R8_CLAMP(x, a, b)   ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

#define R8_SQR(x)           x * x

#define R8_SWAP(T, a, b)    \
    {                       \
        T _tmp = a;         \
        a = b;              \
        b = _tmp;           \
    }

// The bitwise XOR operator can be used to swap two non-floating point numbers
#define R8_SWAP_INT(a, b)   \
    {                       \
        a ^= b;             \
        b ^= a;             \
        a ^= b              \
                            \
    }

#ifdef R8_FAST_MATH
#   define R8_SIN(x)        r8ApproxSin(x)
#   define R8_COS(x)        r8ApproxCos(x)
#   define R8_TAN(x)        (r8ApproxSin(x) / r8ApproxCos(x))
#   define R8_INV_SQRT(x)   r8ApproxFastInvSqrt(x)
#else
#   define R8_SIN(x)        sinf(x)
#   define R8_COS(x)        cosf(x)
#   define R8_TAN(x)        tanf(x)
#   define R8_INV_SQRT(x)   (1.0f / sqrt(x))
#endif

/// Calculate the approximated and fast value for sin
R8float r8ApproxSin(R8float x);

/// Calculate the approximated and fast value for cos
R8float r8ApproxCos(R8float x);

/// Calculate the approximated and fast value for inverse square root
R8float r8ApproxFastInvSqrt(R8float x);

#endif