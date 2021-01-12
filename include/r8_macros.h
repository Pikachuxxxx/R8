/*
 * r8_macros.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_MACROS_H
#define R8_MACROS_H


// Color formats
#define R8_UBYTE_RGB        0x00000001
//#define R8_UBYTE_RGBA       0x00000002

// r8GetString arguments
#define R8_STRING_VERSION   0x00000011
#define R8_STRING_RENDERER  0x00000012
#define R8_STRING_PLUGINS   0x00000013

// r8GetIntegerv arguments
#define R8_MAX_TEXTURE_SIZE 0x00000021

// Geometry primitives
#define R8_POINTS           0x00000031
#define R8_LINES            0x00000032
#define R8_LINE_STRIP       0x00000033
#define R8_LINE_LOOP        0x00000034
#define R8_TRIANGLES        0x00000035
#define R8_TRIANGLE_STRIP   0x00000036
#define R8_TRIANGLE_FAN     0x00000037

// Cull modes
#define R8_CULL_NONE        0x00000040
#define R8_CULL_FRONT       0x00000041
#define R8_CULL_BACK        0x00000042

// Polygon modes
#define R8_POLYGON_FILL     0x00000053
#define R8_POLYGON_LINE     0x00000054
#define R8_POLYGON_POINT    0x00000055

// r8GetTexLevelParameteri arguments
#define R8_TEXTURE_WIDTH    0x00000060
#define R8_TEXTURE_HEIGHT   0x00000061

// States
#define R8_SCISSOR          0
#define R8_MIP_MAPPING      1

// Texture environment parameters
#define R8_TEXTURE_LOD_BIAS 0

// Frame buffer clear flags
#define R8_COLOR_BUFFER_BIT 0x00000001
#define R8_DEPTH_BUFFER_BIT 0x00000002


#endif
