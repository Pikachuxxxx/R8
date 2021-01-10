/* r8_image.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_IMAGE_H
#define R_8_IMAGE_H

#include "r8_types.h"
#include "r8_color.h"

typedef struct R8Image 
{
    R8int width;        // Image width
    R8int height;       // Image height
    R8int format;       // Color format (1, 2, 3 or 4)
    R8bool defFree;     // True if colors will be deleted with default method
    R8ubyte* colors;    // Colors array
}R8Image;


R8Image* r8ImageLoadFromFile(const char* fileName);

R8Image* r8ImageGenerate(R8int width, R8int height, R8int format);
R8void r8ImageDelete(R8Image* image);

R8void r8ImageColorToColorBuffer(R8ColorBuffer* dstColors, const R8Image* srcImage, R8bool dither);

#endif