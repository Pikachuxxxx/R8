/*
 * context.c (Linux)
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "context.h"
#include "error.h"
#include "helper.h"

#include <stdlib.h>


r8_context* _currentContext = NULL;

r8_context* _r8_context_create(const R8contextdesc* desc, R8uint width, R8uint height)
{
    if (desc == NULL || desc->window == NULL || width <= 0 || height <= 0)
    {
        _r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create render context
    r8_context* context = R8_MALLOC(r8_context);

    // Open X11 display
    Display* display = XOpenDisplay(NULL);

    if (display == NULL)
    {
        _r8_error_set(R8_ERROR_CONTEXT, __FUNCTION__);
        return NULL;
    }

    // Create X11 objects
    context->wnd = *((Window*)desc->window);
    context->pmp = XCreatePixmap(display, context->wnd, width, height, 8);
    //todo...

    if (context->pmp == 0)
    {
        _r8_error_set(R8_ERROR_CONTEXT, __FUNCTION__);
        return NULL;
    }

    // Create color palette
    context->colorPalette = R8_MALLOC(r8_color_palette);
    _r8_color_palette_fill_r3g3b2(context->colorPalette);

    // Initialize state machine
    _r8_state_machine_init(&(context->stateMachine));
    _r8_context_makecurrent(context);

    return context;
}

void _r8_context_delete(r8_context* context)
{
    if (context != NULL)
    {
        _r8_ref_assert(&(context->stateMachine));

        // Free X11 objects
        XFree(context->pmp);

        free(context->colorPalette);
        free(context->colors);
        free(context);
    }
}

void _r8_context_makecurrent(r8_context* context)
{
    _currentContext = context;
    if (context != NULL)
        _r8_state_machine_makecurrent(&(context->stateMachine));
    else
        _r8_state_machine_makecurrent(NULL);
}

void _r8_context_present(r8_context* context, const r8_framebuffer* framebuffer)
{
    if (context == NULL || framebuffer == NULL)
    {
        _r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }
    if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        _r8_error_set(R8_ERROR_ARGUMENT_MISMATCH, __FUNCTION__);
        return;
    }

    // Get iterators
    const R8uint num = context->width*context->height;

    r8_color* dst = context->colors;
    r8_color* dstEnd = dst + num;

    const r8_pixel* pixels = framebuffer->pixels;
    const r8_color* palette = context->colorPalette->colors;

    #ifndef R8_COLOR_BUFFER_24BIT
    const r8_color* paletteColor;
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

    //todo...
}

