/*
 * context.m (OSX)
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#import <Cocoa/Cocoa.h>

#include "context.h"
#include "error.h"
#include "helper.h"


// Custom view to display context bitmap in NSWindow

@interface R8NSView : NSView
{
    r8_context* context;
}

@end

@implementation R8NSView

- (id)initWithFrame:(NSRect)frame ctx:(r8_context*)pContext;
{
    self = [super initWithFrame:frame];
    context = pContext;
    return self;
}

- (void)drawRect:(NSRect)pRect
{
    NSBitmapImageRep* bmp = (NSBitmapImageRep*)context->bmp;
    [bmp draw];
}

@end


// MacOS specific graphics context

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

    // Store OSX objects
    context->wnd = (void*)desc->window;
    context->bmp = (void*)[[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:   nil
        pixelsWide:                 width
        pixelsHigh:                 height
        bitsPerSample:              8
        samplesPerPixel:            3
        hasAlpha:                   NO
        isPlanar:                   NO
        colorSpaceName:             NSDeviceRGBColorSpace//NSCalibratedRGBColorSpace
        bytesPerRow:                (3 * width)
        bitsPerPixel:               24
    ];

    // Create graphics context
    NSWindow* wnd = (NSWindow*)desc->window;

    // Create pixel buffer
    context->width  = width;
    context->height = height;
    
    // Create color palette
    context->colorPalette = R8_MALLOC(r8_color_palette);
    _r8_color_palette_fill_r3g3b2(context->colorPalette);

    // Initialize state machine
    _r8_state_machine_init(&(context->stateMachine));
    _r8_context_makecurrent(context);

    // Use custom view to display context bitmap
    wnd.contentView = [[R8NSView alloc]
        initWithFrame:  [[wnd contentView] bounds]
        ctx:            context
    ];

    return context;
}

void _r8_context_delete(r8_context* context)
{
    if (context != NULL)
    {
        _r8_ref_assert(&(context->stateMachine));
        
        // Delete OSX objects
        [((NSBitmapImageRep*)context->bmp) release];

        free(context->colorPalette);
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
    const R8uint num = context->width * context->height;

    NSBitmapImageRep* bmp = (NSBitmapImageRep*)context->bmp;

    r8_color* dst = (r8_color*)[bmp bitmapData];
    r8_color* dstEnd = dst + num;

    const r8_pixel* pixels = framebuffer->pixels;

    const r8_color* palette = context->colorPalette->colors;
    const r8_color* paletteColor;

    // Iterate over all pixels
    while (dst != dstEnd)
    {
        paletteColor = (palette + pixels->colorIndex);

        dst->r = paletteColor->r;
        dst->g = paletteColor->g;
        dst->b = paletteColor->b;

        ++dst;
        ++pixels;
    }

    // Trigger content view to be redrawn
    NSWindow* wnd = (NSWindow*)context->wnd;
    [[wnd contentView] setNeedsDisplay:YES];
}

