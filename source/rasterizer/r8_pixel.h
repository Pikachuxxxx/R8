/* r8_pixel.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_PIXEL_H
#define R_8_PIXEL_H

#include "r8_types.h"
#include "r8_color.h"
#include "r8_config.h"

#define R8_DEPTH_MAX 0xffff // 2^16 = 65536 
typedef R8ushort R8DepthBuffer; // 16-Bit depth buffer

typedef struct R8Pixel
{
    R8ColorBuffer colorBuffer;
    R8DepthBuffer depthBuffer;
}R8Pixel;


/// Writes the real depth value to pixel depth value
inline R8DepthBuffer r8WriteRealDepthToBuffer(R8interp z)
{
    return (R8DepthBuffer)(z * (R8interp)R8_DEPTH_MAX);
}

/// Read the depth buffer value in terms of real depth value 
inline R8interp r8ReadDepthBufferToRealValue(R8DepthBuffer buffer)
{
    return (R8interp)(buffer / (R8DepthBuffer)R8_DEPTH_MAX);
}

#endif