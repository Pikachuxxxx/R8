/* r8_state_machine.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_STATE_MACHINE_H
#define R_8_STATE_MACHINE_H

#include "r8_types.h"
#include "r8_macros.h"
#include "r8_viewport.h"
#include "r8_framebuffer.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"
#include "r8_texture.h"
#include "r8_quad.h"
#include "r8_matrix4.h"

#define R8_STATE_MACHINE    (*stateMachine_)
#define R8_NUM_STATES       2


typedef struct R8StateMachine
{
    R8Mat4              projectionMatrix;
    R8Mat4              viewMatrix;
    R8Mat4              modelMatrix;
    R8Mat4              modelViewMatrix;
    R8Mat4              MVPMatrix;

    R8Viewport          viewport;

    R8Quad              viewportQuad;
    R8Quad              scissorQuad;
    R8Quad              clipQuad;

    R8FrameBuffer*      framebuffer;
    R8VertexBuffer*     vertexbuffer;
    R8IndexBuffer*      indexbuffer;
    R8Texture*          texture;

    R8ColorBuffer       clearColor;
    R8ColorBuffer       color0;
    R8ubyte             textureLODBias;

    R8enum              cullMode;
    R8enum              polygonMode;

    R8bool              states[R8_NUM_STATES];

    R8sizei             refCounter;
}R8StateMachine;


extern R8StateMachine* stateMachine_;




#endif