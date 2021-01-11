/* context.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_CONTEXR_H
#define R_8_CONTEXR_H

#include "r8_types.h"
#include "r8_framebuffer.h"
#include "r8_color_palette.h"
#include "r8_platform.h"
#include "r8_state_machine.h"

#include <windows.h>

typedef struct R8Context
{
    // Win32 Objects
    HWND window;
    HDC dc;
    HDC dcBmp;
    BITMAPINFO bmpInfo;
    HBITMAP bmp;

    // Renderer Objects
    R8Color* colors;
    R8uint width;
    R8uint height;
    R8ColorPalette* colorPalette;
    R8StateMachine stateMachine;
}R8Context;

extern R8Context* currentContext_;

R8Context* r8ContextCreate(const R8ContextDesc* desc, R8uint width, R8uint height);
R8void r8ContextDelete(R8Context* context);
R8void r8ContextMakeCurrent(R8Context* context);
R8void r8ContextPresent(R8Context* context, const R8FrameBuffer* framebuffer);

#endif