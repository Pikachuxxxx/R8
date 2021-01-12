/*
 * r8_types.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_TYPES_H
#define R8_TYPES_H


#include <stddef.h>


#define R8_TRUE         1
#define R8_FALSE        0

#ifdef _MSC_VER
#   define R8_INLINE    _inline
#else
#   define R8_INLINE    static inline
#endif


/// Boolean type.
typedef char R8boolean;

/// 8-bit signed integer.
typedef char R8byte;
/// 8-bit unsigned integer.
typedef unsigned char R8ubyte;

/// 16-bit signed integer.
typedef short R8short;
/// 16-bit unsigned integer.
typedef unsigned short R8ushort;

/// 32-bit signed integer.
typedef int R8int;
/// 32-bit unsigned integer.
typedef unsigned int R8uint;

/// 32-bit floating-point.
typedef float R8float;
/// 64-bit floating-point.
typedef double R8double;

/// Enumeration type.
typedef unsigned int R8enum;
/// Bit field type used for flags.
typedef unsigned int R8bitfield;
/// Size type.
typedef int R8sizei;

/// Texture size type: 8-bit unsigned byte.
typedef R8short R8texsize;

/// No type or used for void pointer.
typedef void R8void;

/// Object type.
typedef void* R8object;


#endif
