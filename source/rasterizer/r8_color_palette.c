/* r8_color_palette.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_color_palette.h"

R8void r8FillColorPalette(R8ColorPalette* palette)
{
    R8Color* color = palette->colors;

    // Color palettes for 3 and 2 bit color components
    const R8ubyte palette3Bit[8] = { 0, 16, 32, 64, 96, 128, 192, 255 };
    const R8ubyte palette2Bit[4] = { 0, 64, 128, 255 };

    for (R8ubyte r = 0; r < 8; ++r)
    {
        for (R8ubyte g = 0; g < 8; ++g)
        {
            for (R8ubyte b = 0; b < 4; ++b)
            {
                color->r = palette3Bit[r];
                color->g = palette3Bit[g];
                color->b = palette2Bit[b];
                ++color;
            }
        }
    }
}

// WTF is happening here?
R8ColorBuffer r8RGBToColorBuffer(R8Color color)
{
    return ((color.r / 32) << 5) | ((color.g / 32) << 2) | (color.b / 64);
}
