/*
 * r8_external_math.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_external_math.h"

#define _USE_MATH_DEFINES
#include <math.h>


// Fast inverse square root from "Quake III Arena"
// See http://en.wikipedia.org/wiki/Fast_inverse_square_root
R8float r8_apx_inv_sqrt(R8float x)
{
    float x2 = x * 0.5f;
    float y = x;
    long i = *(long*)(&y);      // Evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);  // Magic number
    y = *(float*)(&i);
    y = y * (1.5f - (x2*y*y));  // 1st iteration
    return y;
}

// Fast and accurate sine apr8oximation
// See http://lab.polygonal.de/?p=205
R8float r8_apx_sin(R8float x)
{
    // Always wrap input angle to [-PI .. PI]
    if (x < -R8_MATH_PI)
        x = fmodf(x - R8_MATH_PI, R8_MATH_PI*2.0f) + R8_MATH_PI;
    else if (x > R8_MATH_PI)
        x = fmodf(x + R8_MATH_PI, R8_MATH_PI*2.0f) - R8_MATH_PI;

    // Compute sine
    float y;

    if (x < 0)
    {
        y = 1.27323954f * x + 0.405284735f * x*x;

        if (y < 0)
            y = 0.225f * (y*(-y) - y) + y;
        else
            y = 0.225f * (y*y - y) + y;
    }
    else
    {
        y = 1.27323954f * x - 0.405284735f * x*x;

        if (y < 0)
            y = 0.225f * (y*(-y) - y) + y;
        else
            y = 0.225f * (y*y - y) + y;
    }

    return y;
}

R8float r8_apx_cos(R8float x)
{
    return r8_apx_sin(x + R8_MATH_PI*0.5f);
}

// See http://stackoverflow.com/questions/9411823/fast-log2float-x-implementation-c
R8float r8_apx_log2(R8float x)
{
    long* const expPtr = (long*)(&x);
    long y = *expPtr;
    const long lg2 = ((y >> 23) & 255) - 128;

    y &= ~(255 << 23);
    y += 127 << 23;

    *expPtr = y;

    x = ((-1.0f/3) * x + 2) * x - 2.0f/3;

    return (x + lg2);
}

R8float r8_apx_log(R8float x)
{
    return r8_apx_log2(x) * 0.69314718f;
}

R8int r8_int_log2(R8float x)
{
    #ifdef R8_FAST_MATH
    unsigned long* ix = (unsigned long*)(&x);
    unsigned long exp = ((*ix) >> 23) & 0xff;
    return (R8int)exp - 127;
    #else
    int y;
    frexpf(x, &y);
    return y - 1;
    #endif
}

