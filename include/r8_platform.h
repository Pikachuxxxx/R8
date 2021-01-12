/*
 * r8_platform.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_PLATFORM_H
#define R8_PLATFORM_H


/// Render context description structure.
typedef struct R8contextdesc
{
    /**
     * Reference to the OS dependent window.
    - For Win32, this must be from type 'const HWND*'
    - For MacOS, this must be from type 'const NSWindow*'
    - For Linux, this must be from type 'const Window*'
    */
    const void* window;
}
R8contextdesc;


#endif
