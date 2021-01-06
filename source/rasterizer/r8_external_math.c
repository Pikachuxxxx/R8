/* r8_external_math.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_external_math.h"

// Fast and accurate sine approximation
// See https://gist.github.com/geraldyeo/988116
R8float r8ApproxSin(R8float x)
{
    //always wrap input angle to -PI..PI
    if (x < -3.14159265)
        x += 6.28318531;
    else if (x > 3.14159265)
        x -= 6.28318531;

    //compute sine
    if (x < 0)
        return (1.27323954 * x + .405284735 * x * x);
    else
        return (1.27323954 * x - 0.405284735 * x * x);
}

// Fast and accurate cosine approximation
// See https://gist.github.com/geraldyeo/988116
R8float r8ApproxCos(R8float x)
{
    //always wrap input angle to -PI..PI
    if (x < -3.14159265)
        x += 6.28318531;
    else if (x > 3.14159265)
        x -= 6.28318531;

    //compute cosine: sin(x + PI/2) = cos(x)
    x += 1.57079632;
    if (x > 3.14159265)
        x -= 6.28318531;

    if (x < 0)
        return (1.27323954 * x + 0.405284735 * x * x);
    else
        return (1.27323954 * x - 0.405284735 * x * x);
}

// Fast inverse square root from "Quake III Arena"
// See http://en.wikipedia.org/wiki/Fast_inverse_square_root
R8float r8ApproxFastInvSqrt(R8float x)
{
    float x2 = x * 0.5f;
    float y = x;
    long i = *(long*)(&y);          // Evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);      // Magic number
    y = *(float*)(&i);
    y = y * (1.5f - (x2 * y * y));  // 1st iteration
    return y;
}
