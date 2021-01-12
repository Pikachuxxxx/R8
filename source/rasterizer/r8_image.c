/*
 * r8_image.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_image.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_config.h"
#include "r8_color_palette.h"

#ifdef R8_INCLUDE_PLUGINS
#   define STB_IMAGE_IMPLEMENTATION
#   include "plugins/stb/stb_image.h"
#endif


R8Image* r8_image_load_from_file(const char* filename)
{
    if (filename == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return NULL;
    }
    if (*filename == '\0')
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create image and load data from file
    R8Image* image = R8_MALLOC(R8Image);

    #ifdef R8_INCLUDE_PLUGINS

    image->width    = 0;
    image->height   = 0;
    image->format   = 0;
    const unsigned char* colorData = stbi_load(filename, &(image->width), &(image->height), &(image->format), 3);
    if (colorData == NULL)
    {
        printf("ERROR:: Cannot open the file");
        colorData = NULL;
    }
    image->colors = colorData;
    image->defFree  = R8_FALSE;

    if (image->colors == NULL)
    {

    #endif

    image->width    = 1;
    image->height   = 1;
    image->format   = 3;
    image->colors   = R8_CALLOC(R8ubyte, 3);
    image->defFree  = R8_TRUE;

    image->colors[0] = 0;
    image->colors[1] = 0;
    image->colors[2] = 0;

    #ifdef R8_INCLUDE_PLUGINS

    }

    #endif

    return image;
}

R8Image* r8_image_create(R8int width, R8int height, R8int format)
{
    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create image
    R8Image* image = R8_MALLOC(R8Image);

    image->width    = width;
    image->height   = height;
    image->format   = format;
    image->defFree  = R8_TRUE;
    image->colors   = R8_CALLOC(R8ubyte, width*height*format);

    return image;
}

void r8_image_delete(R8Image* image)
{
    if (image != NULL)
    {
        if (image->colors != NULL)
        {
            #ifdef R8_INCLUDE_PLUGINS
            if (!image->defFree)
            {
                stbi_image_free(image->colors);
            }
            else
            #endif
            {
                //error???
                //R8_FREE(image->colors);
            }
        }
        R8_FREE(image);
    }
}


#define PIXEL(x, y) imageBuffer[(y)*imageWidth+(x)]
#define DITHER(c, s)

/*
This function implements the "Floyd-Steinberg" dithering algorithm.
The distribution pattern around the pixel 'px' is:
       [ px ] [7/16]
[3/16] [5/16] [1/16]
*/
static void _dither_color(R8int* buffer, R8int x, R8int y, R8int comp, R8int width, R8int height, R8int scale)
{
    #define COLOR(x, y) buffer[((y)*width + (x))*3 + comp]

    // Get old and new color
    int oldPixel = COLOR(x, y);
    int newPixel = (oldPixel/scale)*scale;

    // Write final color
    COLOR(x, y) = newPixel;

    // Get quantification error
    int quantErr = oldPixel - newPixel;

    // Apply dithering distribution
    if (x + 1 < width)
        COLOR(x + 1, y    ) += quantErr*7/16;
    if (x > 0 && y + 1 < height)
        COLOR(x - 1, y + 1) += quantErr*3/16;
    if (y + 1 < height)
        COLOR(x    , y + 1) += quantErr*5/16;
    if (x + 1 < width && y + 1 < height)
        COLOR(x + 1, y + 1) += quantErr*1/16;

    #undef COLOR
}

void r8_image_color_to_colorindex(R8ColorBuffer* dstColors, const R8Image* srcImage, R8boolean dither)
{
    // Validate and map input parameters
    if (dstColors == NULL || srcImage == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }

    const R8int width = srcImage->width;
    const R8int height = srcImage->height;
    const R8int format = srcImage->format;

    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    const R8ubyte* src = srcImage->colors;

    const R8uint numPixels = width*height;

    if (dither != R8_FALSE)
    {
        // Fill temporary integer buffer
        const R8uint numColors = width*height*3;
        R8int* buffer = R8_CALLOC(R8int, numColors);

        if (format < 3)
        {
            // Copy gray scale image into buffer
            for (R8uint i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i    ] = src[j];
                buffer[i + 1] = src[j];
                buffer[i + 2] = src[j];
            }
        }
        else
        {
            // Copy RGB image into buffer
            for (R8uint i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i    ] = (R8int)src[j    ];
                buffer[i + 1] = (R8int)src[j + 1];
                buffer[i + 2] = (R8int)src[j + 2];
            }
        }

        // Apply dithering to buffer
        for (R8int y = 0; y < height; ++y)
        {
            for (R8int x = 0; x < width; ++x)
            {
                _dither_color(buffer, x, y, 0, width, height, R8_COLORINDEX_SCALE_RED  );
                _dither_color(buffer, x, y, 1, width, height, R8_COLORINDEX_SCALE_GREEN);
                _dither_color(buffer, x, y, 2, width, height, R8_COLORINDEX_SCALE_BLUE );
            }
        }

        // Finally convert buffer to color index
        for (R8uint i = 0, j = 0; i < numPixels; ++i, j += 3)
            dstColors[i] = r8_color_to_colorindex(buffer[j], buffer[j + 1], buffer[j + 2]);

        // Delete temporary buffer
        R8_FREE(buffer);
    }
    else
    {
        // Copy image data
        if (format < 3)
        {
            // Copy gray scale image into color index
            R8ubyte gray;

            for (R8uint i = 0, j = 0; i < numPixels; ++i, j += format)
            {
                gray = src[j];
                dstColors[i] = r8_color_to_colorindex(gray, gray, gray);
            }
        }
        else
        {
            // Copy RGB image into color index
            for (R8uint i = 0, j = 0; i < numPixels; ++i, j += format)
                dstColors[i] = r8_color_to_colorindex(src[j], src[j + 1], src[j + 2]);
        }
    }
}

