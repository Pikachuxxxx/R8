/* r8_color_palette.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_COLOR_PALETTE_H
#define R_8_COLOR_PALETTE_H

#include "r8_color.h"

typedef struct R8ColorPalette
{
    R8Color* colors[256];
}R8ColorPalette;

R8void r8FillColorPalette(R8ColorPalette* palette);

R8ColorBuffer r8RGBToColorBuffer(R8Color color);

#endif