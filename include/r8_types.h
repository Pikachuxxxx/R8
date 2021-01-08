/* r8_types.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_TYPES_H
#define R_8_TYPES_H

#include <stddef.h>

#define R8_TRUE     1
#define R8_FALSE    0

typedef void            R8void;
typedef void*           R8object;
typedef char            R8bool;
typedef char            R8byte;
typedef unsigned char   R8ubyte;
typedef short           R8short;
typedef unsigned short  R8ushort;
typedef int             R8int;
typedef unsigned int    R8uint;
typedef long            R8long;
typedef unsigned long   R8ulong;
typedef float           R8float;
typedef double          R8double;
typedef long double     R8ldouble;
typedef R8uint          R8enum;
typedef R8uint          R8bit;
typedef R8int           R8sizei;
typedef R8short         R8texturesize;

#endif
