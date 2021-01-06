/* r8_macros.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_MACROS_H
#define R_8_MACROS_H

// Color Formats
// TODO: Add color components with alpha component
//#define R8_RGBA  0x00000000
#define R8_RGB                  0x00000001
#define R8_BGR                  0x00000002
#define R8_RRR                  0x00000003
#define R8_GGG                  0x00000004
#define R8_BBB                  0x00000005

// GetString arguments
#define R8_VERSION              0x00000010

// GetInteger arguments
#define R8_MAX_TEXTURE_SIZE     0x00000020

// Geometry primitives
#define R8_POINTS               0x00000030
#define R8_LINES                0x00000031
#define R8_TRIANGLES            0x00000032
#define R8_TRIANGLES_STRIP      0x00000033
                                 
// Culling modes                 
#define R8_CULL_NONE            0x00000040
#define R8_CULL_FRONT           0x00000041
#define R8_CULL_BACK            0x00000042
                                 
// Polygone drawing mode         
#define R8_FILL                 0x00000050
#define R8_LINE                 0x00000051

// Texture Parameters
#define R8_MAX_TEXTURE_WIDTH    0x00000060
#define R8_MAX_TEXTURE_HEIGHT   0x00000061
#define R8_TEXTURE_WIDTH        0x00000062
#define R8_TEXTURE_HEIGHT       0x00000063
#define R8_MAG_FILTER           0x00000064
#define R8_MIN_FILTER           0x00000065
#define R8_TEXTURE_S            0x00000066
#define R8_TEXTURE_T            0x00000066
#define R8_LINEAR               0x00000067
#define R8_CLAMP                0x00000068
#define R8_REPEAT               0x0000006A

// rendering states
#define R8_WIREFRAME_MODE       0x00000070
#define R8_SCISSOR_MODE         0x00000071
#define R8_MIPMAP_MODE          0x00000072

// Frame buffer flags
#define R8_COLOR_BUFFER_BIT     0x00000080
#define R8_DEPTH_BUFFER_BIT     0x00000081

#endif