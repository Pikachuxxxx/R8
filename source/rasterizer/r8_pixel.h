/*
 * r8_pixel.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_PIXEL_H
#define R8_PIXEL_H


#include "r8_types.h"
#include "r8_config.h"
#include "r8_color.h"

#include <limits.h>


#ifdef R8_DEPTH_BUFFER_8BIT
#define R8_DEPTH_MAX UCHAR_MAX
typedef R8ubyte R8DepthBuffer;
#else
#define R8_DEPTH_MAX USHRT_MAX
typedef R8ushort R8DepthBuffer;
#endif

//! Frame buffer pixel structure.
typedef struct R8Pixel
{
    R8ColorBuffer    colorIndex; // Colors are stored as 8-bit unsigned integer.
    R8DepthBuffer     depth;      // Depth values are stored as 16-bit unsigned integer.
}
R8Pixel;


/**
Writes the specified real depth value to a pixel depth.
\param[in] z Specifies the real z value. This must be in the range [0.0 .. 1.0].
*/
R8_INLINE R8DepthBuffer r8_pixel_write_depth(R8interp z)
{
    return (R8DepthBuffer)(z * (R8interp)R8_DEPTH_MAX);
}

/// Reads the specified pixel depth to a real depth value.
R8_INLINE R8interp r8_pixel_read_depth(R8DepthBuffer z)
{
    return ((R8interp)z) / ((R8DepthBuffer)R8_DEPTH_MAX);
}


#endif
