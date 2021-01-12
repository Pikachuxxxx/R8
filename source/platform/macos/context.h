/*
 * context.h (OSX)
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */
#ifndef R8_CONTEXT_H
#define R8_CONTEXT_H


#include "types.h"
#include "framebuffer.h"
#include "color_palette.h"
#include "color.h"
#include "platform.h"
#include "state_machine.h"


//! Render context structure.
typedef struct r8_context
{
    // OSX objects
    void*               wnd;
    void*               bmp;
    
    // Renderer objects
    R8uint              width;
    R8uint              height;
    r8_color_palette*   colorPalette;
    
    // State objects
    r8_state_machine    stateMachine;
}
r8_context;


extern r8_context* _currentContext;

//! Creates a new render context for the specified device context.
r8_context* _r8_context_create(const R8contextdesc* desc, R8uint width, R8uint height);
//! Deletes the specified render context.
void _r8_context_delete(r8_context* context);

//! Makes the specified context to the current one.
void _r8_context_makecurrent(r8_context* context);

/**
Presents the specified framebuffer onto the render context.
Errors:
- R8_ERROR_NULL_POINTER : If 'context', 'framebuffer' or 'colorPalette' is null.
- R8_ERROR_ARGUMENT_MISMATCH : If 'context' has another dimension than 'framebuffer'.
*/
void _r8_context_present(r8_context* context, const r8_framebuffer* framebuffer);


#endif
