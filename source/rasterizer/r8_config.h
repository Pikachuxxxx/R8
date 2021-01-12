/*
 * r8_config.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_CONFIG_H
#define R8_CONFIG_H


// Version number
#define R8_VERSION_MAJOR    0
#define R8_VERSION_MINOR    1
#define R8_VERSION_REVISION 0
#define R8_VERSION_STATUS   "alpha"
#define R8_VERSION_STR      "0.1 alpha"

/// Use BGR color output instead of RGB (used for Win32 GDI)
#ifdef _WIN32
#   define R8_BGR_COLOR_OUTPUT
#endif

/// Includes the STB image file handler plugin
#define R8_INCLUDE_PLUGINS

/// Flips the screen space vertical
#define R8_ORIGIN_LEFT_TOP

/// Makes excessive use of the apr8oximated math functions
#define R8_FAST_MATH

/// Enables extended debug information
#define R8_DEBUG

/// Use perspective corrected depth and texture coordinates
#define R8_PERSPECTIVE_CORRECTED

/// Use an 8-bit depth buffer (instead of 16 bit)
//#define R8_DEPTH_BUFFER_8BIT

/// Use a 64-bit interpolation type instead of 32-bit
#define R8_INTERP_64BIT

/// Merge color- and depth buffers to a single one inside a frame buffer.
#define R8_MERGE_COLOR_AND_DEPTH_BUFFERS //CAN NOT BE DISABLED YET!

/// Makes all pixels with color black a transparent pixel.
#define R8_BLACK_IS_ALPHA


#ifdef R8_INTERP_64BIT
/// 64-bit interpolation type.
typedef double R8interp;
#define R8_FLOAT(x) x
#else
/// 32-bit interpolation type.
typedef float R8interp;
#define R8_FLOAT(x) x##f
#endif


#endif
