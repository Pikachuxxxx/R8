/*
 * r8_global_state.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_GLOBAL_STATE_H
#define R8_GLOBAL_STATE_H


#include "r8_texture.h"
#include "r8_vertexbuffer.h"


#define R8_SINGULAR_TEXTURE         globalState_.singularTexture
#define R8_SINGULAR_VERTEXBUFFER    globalState_.singularVertexBuffer

// Number of vertices for the vertex buffer of the immediate draw mode (r8Begin/r8End)
#define R8_NUM_IMMEDIATE_VERTICES   32


typedef struct r8_global_state
{
    R8Texture      singularTexture;        // Texture with single color
    R8VertexBuffer singularVertexBuffer;

    // Immediate mode
    R8VertexBuffer immModeVertexBuffer;
    R8boolean       immModeActive;
    R8sizei         immModeVertCounter;
    R8enum          immModePrimitives;
}
r8_global_state;


/// Global render engine state.
extern r8_global_state globalState_;


void r8_global_state_init();
void r8_global_state_release();

void r8_immediate_mode_begin(R8enum priitives);
void r8_immediate_mode_end();

void r8_immediate_mode_texcoord(R8float u, R8float v);
void r8_immediate_mode_vertex(R8float x, R8float y, R8float z, R8float w);


#endif
