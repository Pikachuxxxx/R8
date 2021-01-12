/*
 * r8_image.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_IMAGE_H
#define R8_IMAGE_H


#include "r8_types.h"
#include "r8_color.h"


/// Simple 2D image.
typedef struct R8Image
{
    R8int width;        // Image width.
    R8int height;       // Image height.
    R8int format;       // Color format (1, 2, 3 or 4).
    R8boolean defFree;  // True if colors will be deleted with default method.
    R8ubyte* colors;    // Colors array.
}
R8Image;


/// Loads an RGB image from file.
R8Image* r8_image_load_from_file(const char* filename);
/**
Creates a new image.
\param[in] width Specifies the image width. This must be greater than 0.
\param[in] height Specifies the image width. This must be greater than 0.
\param[in] format Specifies the image format. This must be 1, 2, 3 or 4.
*/
R8Image* r8_image_create(R8int width, R8int height, R8int format);
/// Deletes the specifies image.
void r8_image_delete(R8Image* image);

/**
Converts the specified 24-bit RGB colors 'src' into 8-bit color indices 'dst'.
\param[out] dst Pointer to the destination color indices.
This must already be allocated with the specified size: width*height*sizeof(R8ubyte)!
\param[in] src Pointer to the source colors.
\param[in] width Specifies the image width.
\param[in] height Specifies the image height.
\param[in] format Specifies the source color format. Must be 1, 2, 3 or 4.
\param[in] dither Specifies whether the image is to be converted with or without dithering.
*/
void r8_image_color_to_colorindex(R8ColorBuffer* dstColors, const R8Image* srcImage, R8boolean dither);


#endif
