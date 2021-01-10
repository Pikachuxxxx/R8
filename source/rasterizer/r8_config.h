/* r8_config.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_CONFIG_H
#define R_8_CONFIG_H

// Version number
#define R8_VER_MAJOR 0
#define R8_VER_MINOR 1
#define R8_VER_PATCH 0
#define R8_VER_STRING "alpha 0.1.0"

/*
 * Use BGR color buffer format instead of RGB (used for Win32 GDI)
 */
#if (_WIN32)
    #define R8_BGR_COLOR_FORMAT
#endif

/// Merge color- and depth buffers to a single one inside a frame buffer.
#define R8_MERGE_COLOR_AND_DEPTH_BUFFERS // cannot be disabled yet!!!

/// Makes all pixels with color black a transparent pixel.
#define R8_BLACK_TO_TRANSPARENCY

/// Use perspective corrected depth and texture coordinates
#define R8_PERSPECTIVE_DEPTH_CORRECTED

/// Flips the screen space vertical
#define R8_ORIGIN_TOP_LEFT

/// Makes use of the approximated math functions
#define R8_FAST_MATH

/// Enables extended debug information
#define R8_DEBUG

/// 32-bit interpolation type.
typedef float R8interp;
#define R8_FLOAT(x) x##f // appending the f to the number

#endif 