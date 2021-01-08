/* r8_image.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_image.h"



R8Image* r8ImageLoadFromFile(const char* fileName)
{
    if (fileName == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return NULL;
    }
    if (*fileName == '\0')
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    // Create image and load data from file
    R8Image* image = R8_MALLOC(R8Image);

    image->width = 1;
    image->height = 1;
    image->format = 3;
    image->colors = R8_CALLOC(R8ubyte, 3);
    image->defFree = R8_TRUE;

    image->colors[0] = 0;
    image->colors[1] = 0;
    image->colors[2] = 0;

    return image;
}

R8Image* r8ImageGenerate(R8int width, R8int height)
{
    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    // Create image
    R8Image* image = R8_MALLOC(R8Image);

    image->width = width;
    image->height = height;
    image->format = format;
    image->defFree = R8_TRUE;
    image->colors = R8_CALLOC(R8ubyte, width * height * format);

    return image;
}

R8void r8ImageDelete(R8Image* image)
{
    if (image != NULL)
    {
        if (image->colors != NULL)
        {
            //error???
            //PR_FREE(image->colors);
        }
        R8_FREE(image);
    }
}

#define PIXEL(x, y) imageBuffer[(y)*imageWidth+(x)]
#define DITHER(c, s)


/*
 * This function implements the "Floyd-Steinberg" dithering algorithm.
 * The distribution pattern around the pixel 'px' is:
 *        [ px ] [7/16]
 * [3/16] [5/16] [1/16]
 */

static R8void r8DitherColor(R8int* buffer, R8int x, R8int y, R8int comp, R8int width, R8int height, R8int scale)
{
    #define COLOR(x, y) buffer[((y)*width + (x))*3 + comp]

    // Get old and new color
    int oldPixel = COLOR(x, y);
    int newPixel = (oldPixel / scale) * scale;

    // Write final color
    COLOR(x, y) = newPixel;

    // Get quantification error
    int quantErr = oldPixel - newPixel;

    // Apply dithering distribution
    if (x + 1 < width)
        COLOR(x + 1, y) += quantErr * 7 / 16;
    if (x > 0 && y + 1 < height)
        COLOR(x - 1, y + 1) += quantErr * 3 / 16;
    if (y + 1 < height)
        COLOR(x, y + 1) += quantErr * 5 / 16;
    if (x + 1 < width && y + 1 < height)
        COLOR(x + 1, y + 1) += quantErr * 1 / 16;

    #undef COLOR
}


R8void r8ImageColorToColorBuffer(R8ColorBuffer* dstColors, const R8Image* srcImage, R8bool dither)
{
    // Validate and map input parameters
    if (dstColors == NULL || srcImage == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    const R8int width = srcImage->width;
    const R8int height = srcImage->height;
    const R8int format = srcImage->format;

    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    const R8ubyte* src = srcImage->colors;

    const R8uint numPixels = width * height;

    if (dither != R8_FALSE)
    {
        // Fill temporary integer buffer
        const R8uint numColors = width * height * 3;
        R8int* buffer = R8_CALLOC(R8int, numColors);

        if (format < 3)
        {
            // Copy gray scale image into buffer
            for (R8int i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i] = src[j];
                buffer[i + 1] = src[j];
                buffer[i + 2] = src[j];
            }
        }
        else
        {
            // Copy RGB image into buffer
            for (R8int i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i] = (R8int)src[j];
                buffer[i + 1] = (R8int)src[j + 1];
                buffer[i + 2] = (R8int)src[j + 2];
            }
        }

        // Apply dithering to buffer
        for (R8int y = 0; y < height; ++y)
        {
            for (PRint x = 0; x < width; ++x)
            {
                r8DitherColor(buffer, x, y, 0, width, height, 36);
                r8DitherColor(buffer, x, y, 1, width, height, 36);
                r8DitherColor(buffer, x, y, 2, width, height, 85);
            }
        }

        // Finally convert buffer to color index
        for (R8uint i = 0, j = 0; i < numPixels; ++i, j += 3)
        {
            R8Color color;
            color.r = buffer[j];
            color.g = buffer[j + 1];
            color.b = buffer[j + 2];
            dstColors[i] = r8RGBToColorBuffer(color);
        }

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
                R8Color color;
                color.r = gray;
                color.g = gray;
                color.b = gray;
                dstColors[i] = r8RGBToColorBuffer(color);
            }
        }
        else
        {
            // Copy RGB image into color index
            for (R8uint i = 0, j = 0; i < numPixels; ++i, j += format)
            {
                R8Color color;
                color.r = src[j];
                color.g = src[j + 1];
                color.b = src[j + 2];
                dstColors[i] = r8RGBToColorBuffer(color);
            }
        }
    }
}
