/* r8_color_palette.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_COLOR_PALETTE_H
#define R_8_COLOR_PALETTE_H

#include "r8_color.h"
#include "r8_error_codes.h"
#include "r8_error.h"

#define R8_COLORINDEX_SCALE_RED     36
#define R8_COLORINDEX_SCALE_GREEN   36
#define R8_COLORINDEX_SCALE_BLUE    85

#define R8_COLORINDEX_SELECT_RED    32
#define R8_COLORINDEX_SELECT_GREEN  32
#define R8_COLORINDEX_SELECT_BLUE   64

typedef struct R8ColorPalette
{
    R8Color* colors[256];
}R8ColorPalette;

R8void r8FillColorPalette(R8ColorPalette* palette);

R8ColorBuffer r8RGBToColorBuffer(R8ubyte r, R8ubyte g, R8ubyte b);

#endif