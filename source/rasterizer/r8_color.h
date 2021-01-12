/*
 * r8_color.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_COLOR_H
#define R8_COLOR_H


#include "r8_config.h"


#ifdef R8_BGR_COLOR_OUTPUT

#include "r8_color_bgr.h"
typedef R8ColorBGR R8Color;

#else

#include "r8_color_rgb.h"
typedef R8ColorRGB R8Color;

#endif


/****************************************************
 *                                                  *
 *             8 - Bit Part of the Renderer         *
 *                                                  *
 ****************************************************
 *  8-bit color encoding:
 *  Bit     7 6 5 4 3 2 1 0
 *  Color   R R R G G G B B
 *  For dithering: scale R and G with 36 and scale B with 85
 *  For color index selection: scale R and G with 32 and B with 64
 */
typedef R8ubyte R8ColorBuffer;

#endif
