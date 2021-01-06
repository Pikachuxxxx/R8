/* r8_color.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_COLOR_H
#define R_8_COLOR_H

#include "r8_types.h"
#include "r8_config.h"

#ifdef R8_BGR_COLOR_FORMAT

    typedef struct R8ColorBGR
    {
        R8ubyte b;
        R8ubyte g;
        R8ubyte r;
    }R8ColorBGR;

    typedef R8ColorBGR R8Color;

#else

    typedef struct R8ColorRGB
    {
        R8ubyte r;
        R8ubyte g;
        R8ubyte b;
    }R8ColorRGB;

    typedef R8ColorRGB R8Color;

#endif

/*
 *  8-bit color encoding:
 *  Bit     7 6 5 4 3 2 1 0
 *  Color   R R R G G G B B
 *  For dithering: scale R and G with 36 and scale B with 85
 *  For color index selection: scale R and G with 32 and B with 64
 */
typedef R8ubyte R8ColorBuffer;


#endif 