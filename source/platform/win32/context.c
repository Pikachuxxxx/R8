/*
 * context.c 
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "context.h"
#include "r8_error.h"
#include "r8_memory.h"


R8Context* _currentContext = NULL;

R8Context* r8_context_create(const R8contextdesc* desc, R8uint width, R8uint height)
{
    if (desc == NULL || desc->window == NULL || width <= 0 || height <= 0)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create render context
    R8Context* context = R8_MALLOC(R8Context);

    // Setup bitmap info structure
    BITMAPINFO* bmi = (&context->bmpInfo);
    memset(bmi, 0, sizeof(BITMAPINFO));

    bmi->bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth          = (LONG)width;
    bmi->bmiHeader.biHeight         = (LONG)height;
    bmi->bmiHeader.biPlanes         = 1;
    bmi->bmiHeader.biBitCount       = 24;
    bmi->bmiHeader.biCompression    = BI_RGB;

    // Setup context
    context->wnd        = *((HWND*)desc->window);
    context->dc         = GetDC(context->wnd);
    context->dcBmp      = CreateCompatibleDC(context->dc);
    context->bmp        = CreateCompatibleBitmap(context->dc, width, height);
    context->colors     = R8_CALLOC(R8Color, width*height);
    context->width      = width;
    context->height     = height;

    SelectObject(context->dcBmp, context->bmp);

    // Create color palette
    context->colorPalette = R8_MALLOC(R8ColorPalette);
    r8_color_palette_fill_r3g3b2(context->colorPalette);

    // Initialize state machine
    r8_state_machine_init(&(context->stateMachine));
    r8_context_makecurrent(context);

    return context;
}

void r8_context_delete(R8Context* context)
{
    if (context != NULL)
    {
        r8_ref_assert(&(context->stateMachine));

        if (context->bmp != NULL)
            DeleteObject(context->bmp);
        if (context->dcBmp != NULL)
            DeleteDC(context->dcBmp);

        free(context->colorPalette);
        free(context->colors);
        free(context);
    }
}

void r8_context_makecurrent(R8Context* context)
{
    _currentContext = context;
    if (context != NULL)
        r8_state_machine_makecurrent(&(context->stateMachine));
    else
        r8_state_machine_makecurrent(NULL);
}

void r8_context_present(R8Context* context, const R8FrameBuffer* framebuffer)
{
    if (context == NULL || framebuffer == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }
    if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        r8_error_set(R8_ERROR_ARGUMENT_MISMATCH, __FUNCTION__);
        return;
    }

    // Get iterators
    const R8uint num = context->width*context->height;

    R8Color* dst = context->colors;
    R8Color* dstEnd = dst + num;

    const R8Pixel* pixels = framebuffer->pixels;
    const R8Color* palette = context->colorPalette->colors;

    #ifndef R8_COLOR_BUFFER_24BIT
    const R8Color* paletteColor;
    #endif

    // Iterate over all pixels
    while (dst != dstEnd)
    {
        #ifdef R8_COLOR_BUFFER_24BIT
        *dst = pixels->colorIndex;
        #else
        paletteColor = (palette + pixels->colorIndex);

        dst->r = paletteColor->r;
        dst->g = paletteColor->g;
        dst->b = paletteColor->b;
        #endif

        ++dst;
        ++pixels;
    }

    // Show framebuffer on device context ('SetDIBits' only needs a device context when 'DIB_PAL_COLORS' is used)
    SetDIBits(NULL, context->bmp, 0, context->height, context->colors, &(context->bmpInfo), DIB_RGB_COLORS);
    BitBlt(context->dc, 0, 0, context->width, context->height, context->dcBmp, 0, 0, SRCCOPY);
}

