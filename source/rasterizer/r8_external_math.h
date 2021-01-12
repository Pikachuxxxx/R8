/*
 * r8_external_math.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_EXTERNAL_MATH_H
#define R8_EXTERNAL_MATH_H


#include "r8_types.h"
#include "r8_config.h"
#include "r8_constants.h"


#define R8_MIN(a, b)            ((a) < (b) ? (a) : (b))
#define R8_MAX(a, b)            ((a) > (b) ? (a) : (b))

#define R8_CLAMP(x, a, b)       ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

#define R8_CLAMP_LARGEST(x, c)  if ((x) < (c)) x = c
#define R8_CLAMP_SMALLEST(x, c) if ((x) > (c)) x = c

#define R8_SIGN(x)              (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)

#define R8_SQ(x)                ((x)*(x))

#define R8_SWAP(t, a, b)    \
    {                       \
        t _tmp = a;         \
        a = b;              \
        b = _tmp;           \
    }

#define R8_SWAP_INT(a, b)   \
    {                       \
        a ^= b;             \
        b ^= a;             \
        a ^= b;             \
    }

#ifdef R8_FAST_MATH
#   define R8_SIN(x)        r8_apx_sin(x)
#   define R8_COS(x)        r8_apx_cos(x)
#   define R8_TAN(x)        (r8_apx_sin(x)/r8_apx_cos(x))
#   define R8_INV_SQRT(x)   r8_apx_inv_sqrt(x)
#   define R8_LOG2(x)       r8_apx_log2(x)
#   define R8_LOG(x)        r8_apx_log(x)
#else
#   define R8_SIN(x)        sinf(x)
#   define R8_COS(x)        cosf(x)
#   define R8_TAN(x)        tanf(x)
#   define R8_INV_SQRT(x)   (1.0f / sqrtf(x))
#   define R8_LOG2(x)       log2f(x)
#   define R8_LOG(x)        logf(x)
#endif


/// Computes an approximated and fast recir8ocal square root.
R8float r8_apx_inv_sqrt(R8float x);

/// Computes an approximated and fast sine.
R8float r8_apx_sin(R8float x);

/// Computes an approximated and fast cosine.
R8float r8_apx_cos(R8float x);

/// Computes an approximated and fast logarithm of base 2.
R8float r8_apx_log2(R8float x);

/// Computes an approximated and fast logarithm of base e (natural logarithm 'ln').
R8float r8_apx_log(R8float x);

/// Computes the integral value of the logarithm of base 2.
R8int r8_int_log2(R8float x);


#endif
