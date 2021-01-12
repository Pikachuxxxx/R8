/*
 * context.h (Windows)
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_CONTEXT_H
#define R8_CONTEXT_H


#include "r8_types.h"
#include "r8_framebuffer.h"
#include "r8_color_palette.h"
#include "r8_color.h"
#include "r8_platform.h"
#include "r8_state_machine.h"

#include <Windows.h>


/// Render context structure.
typedef struct R8Context
{
    // Win32 objects
    HWND                wnd;
    HDC                 dc;
    HDC                 dcBmp;
    BITMAPINFO          bmpInfo;
    HBITMAP             bmp;

    // Renderer objects
    R8Color*           colors;
    R8uint              width;
    R8uint              height;
    R8ColorPalette*   colorPalette;

    // State objects
    R8StateMachine    stateMachine;
}
R8Context;


extern R8Context* _currentContext;

/// Creates a new render context for the specified device context.
R8Context* r8_context_create(const R8contextdesc* desc, R8uint width, R8uint height);
/// Deletes the specified render context.
void r8_context_delete(R8Context* context);

/// Makes the specified context to the current one.
void r8_context_makecurrent(R8Context* context);

/**
Presents the specified framebuffer onto the render context.
Errors:
- R8_ERROR_NULL_POINTER : If 'context', 'framebuffer' or 'colorPalette' is null.
- R8_ERROR_ARGUMENT_MISMATCH : If 'context' has another dimension than 'framebuffer'.
*/
void r8_context_present(R8Context* context, const R8FrameBuffer* framebuffer);


#endif
