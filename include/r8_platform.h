/* r8_platform.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_PLATFORM_H
#define R_8_PLATFORM_H

typedef struct R8ContextDesc
{
    /*
     * For Win32, window type : 'const HWND*'
     * For MacOS, window type : 'const NSWindow*'
     * For Linux, window type : 'const Window*'
     */
    const void* window;
}R8ContextDesc;

#endif