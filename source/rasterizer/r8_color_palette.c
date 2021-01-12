/*
 * r8_color_palette.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#include "r8_color_palette.h"
#include "r8_error.h"


/*
8-bit color encoding:

Bit     7 6 5 4 3 2 1 0
Color   R R R G G G B B

For dithering: scale R and G with 36 and scale B with 85
For color index selection: scale R and G with 32 and B with 64
*/
void r8_color_palette_fill_r3g3b2(R8ColorPalette* colorPalette)
{
    if (colorPalette == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }

    R8Color* clr = colorPalette->colors;

    // Color palettes for 3- and 2 bit color components
    const R8ubyte palette3Bit[8] = { 0, 36, 73, 109, 146, 182, 219, 255 };
    const R8ubyte palette2Bit[4] = { 0, 85, 170, 255 };

    for (R8ubyte r = 0; r < 8; ++r)
    {
        for (R8ubyte g = 0; g < 8; ++g)
        {
            for (R8ubyte b = 0; b < 4; ++b)
            {
                clr->r = palette3Bit[r];
                clr->g = palette3Bit[g];
                clr->b = palette2Bit[b];
                ++clr;
            }
        }
    }
}

R8ColorBuffer r8_color_to_colorindex(R8ubyte r, R8ubyte g, R8ubyte b)
{
    return
        ((r / R8_COLORINDEX_SELECT_RED  ) << 5) |
        ((g / R8_COLORINDEX_SELECT_GREEN) << 2) |
        ( b / R8_COLORINDEX_SELECT_BLUE       );
}

