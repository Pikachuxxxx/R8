/* context.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "context.h"
#include "r8_error.h"
#include "r8_memory.h"

R8Context* currentContext_ = NULL;

R8Context* r8ContextCreate(const R8ContextDesc* desc, R8uint width, R8uint height)
{
    if (desc == NULL || desc->window == NULL || width <= 0 || height <= 0)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    // Create render context
    R8Context* context = R8_MALLOC(R8Context);

    // Setup bitmap info structure
    BITMAPINFO* bmi = (&context->bmpInfo);

    memset(bmi, 0, sizeof(BITMAPINFO));

    bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth = (LONG)width;
    bmi->bmiHeader.biHeight = (LONG)height;
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = 24;
    bmi->bmiHeader.biCompression = BI_RGB;

    // Setup context
    context->window = *((HWND*)desc->window);
    context->dc = GetDC(context->window);
    context->dcBmp = CreateCompatibleDC(context->dc);
    context->bmp = CreateCompatibleBitmap(context->dc, width, height);
    context->colors = R8_CALLOC(R8ColorPalette, width * height);
    context->width = width;
    context->height = height;

    SelectObject(context->dcBmp, context->bmp);

    // Create color palette
    context->colorPalette = R8_MALLOC(R8ColorPalette);
    r8FillColorPalette(context->colorPalette);

    // Initialize state machine
    r8InitStateMachine(&(context->stateMachine));
    r8ContextMakeCurrent(context);

    return context;
}

R8void r8ContextDelete(R8Context* context)
{
    if (context != NULL)
    {
        r8AssertSMRef(&(context->stateMachine));

        if (context->bmp != NULL)
            DeleteObject(context->bmp);
        if (context->dcBmp != NULL)
            DeleteDC(context->dcBmp);

        free(context->colorPalette);
        free(context->colors);
        free(context);
    }
}

R8void r8ContextMakeCurrent(R8Context* context)
{
    currentContext_ = context;
    if (context != NULL)
        r8StateMachineMakeCurrent(&(context->stateMachine));
    else
        r8StateMachineMakeCurrent(NULL);
}

R8void r8ContextPresent(R8Context* context, const R8FrameBuffer* framebuffer)
{
    if (context == NULL || framebuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }
    if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        R8_ERROR(R8_ERROR_ARGUMENT_MISMATCH);
        return;
    }

    // Get iterators
    const R8uint num = context->width * context->height;

    R8Color* dst = context->colors;
    R8Color* dstEnd = dst + num;

    const R8Pixel* pixels = framebuffer->pixels;
    const R8Color* palette = context->colorPalette->colors;

    const R8Color* paletteColor;

    // Iterate over all pixels
    while (dst != dstEnd)
    {
        paletteColor = (palette + pixels->colorBuffer);

        dst->r = paletteColor->r;
        dst->g = paletteColor->g;
        dst->b = paletteColor->b;

        ++dst;
        ++pixels;
    }

    // Show framebuffer on device context ('SetDIBits' only needs a device context when 'DIB_PAL_COLORS' is used)
    SetDIBits(NULL, context->bmp, 0, context->height, context->colors, &(context->bmpInfo), DIB_RGB_COLORS);
    BitBlt(context->dc, 0, 0, context->width, context->height, context->dcBmp, 0, 0, SRCCOPY);
}

