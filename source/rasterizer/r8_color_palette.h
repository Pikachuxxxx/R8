/*
 * r8_color_palette.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#ifndef R8_COLOR_PALETTE_H
#define R8_COLOR_PALETTE_H


#include "r8_color.h"


#define R8_COLORINDEX_SCALE_RED     36
#define R8_COLORINDEX_SCALE_GREEN   36
#define R8_COLORINDEX_SCALE_BLUE    85

#define R8_COLORINDEX_SELECT_RED    32
#define R8_COLORINDEX_SELECT_GREEN  32
#define R8_COLORINDEX_SELECT_BLUE   64


/// Color palette for 8-bit color indices.
typedef struct R8ColorPalette//_r3g3b2
{
    R8Color colors[256];
}
R8ColorPalette;


/// Fills the specified color palette with the encoding R3G3B2.
void r8_color_palette_fill_r3g3b2(R8ColorPalette* colorPalette);

/// Converts the specified RGB color into a color index with encoding R3G3B2.
R8ColorBuffer r8_color_to_colorindex(R8ubyte r, R8ubyte g, R8ubyte b);


#endif
