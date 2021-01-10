/* r8_global_state.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_GLOBAL_STATE_H
#define R_8_GLOBAL_STATE_H

#include "r8_texture.h"
#include "r8_vertexbuffer.h"

#define R8_SINGLULAR_TEXTURE        globalState.singularTexture
#define R8_SINGULAR_VERTEX_BUFFER   globalState.singularVertexBuffer

// Number of vertices for the vertex buffer of the immediate drawing mode
#define R8_NUM_IMMEDIATE_VERTICES   32

typedef struct R8GlobalState
{
    R8Texture singularTexture;
    R8VertexBuffer singularVertexBuffer;
    // Immediate mode
    R8VertexBuffer  immModeVertexBuffer;
    R8bool          immModeActive;
    R8sizei         immModeVertCounter;
    R8enum          immModePrimitives;
}R8GlobalState;

/// Global render engine state
extern R8GlobalState globalState;

R8void r8GlobalStateInit();
R8void r8GlobalStateRelease();

R8void r8ImmediateModeBegin(R8enum primitives);
R8void r8ImmediateModeRelease();

R8void r8ImmediateModeTexCoord(R8float u, R8float v);
R8void r8ImmediateModeVertex(R8float x, R8float y, R8float z, R8float w);

#endif