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

R8void r8AddSMRef(R8object* ref);
R8void r8ReleaseSMRef(R8object* ref);
R8void r8AssertSMRef(R8StateMachine* stateMachine);

R8void r8InitStateMachine(R8StateMachine* stateMachine);
R8void r8InitNullStateMachine();

R8void r8StateMachineMakeCurrent(R8StateMachine* stateMachine);

R8void r8StateMachineSetState(R8enum cap, PRboolean state);
R8bool r8StateMachineGetState(R8enum cap);

R8void r8StateMachineSetTexenvi(R8enum param, R8int value);
R8int  r8StateMachineGetTexenvi(R8enum param);

R8void r8StateMachineBindFrameBuffer(R8FrameBuffer* frameBuffer);
R8void r8StateMachineBindVertexBuffer(R8VertexBuffer* vertexBuffer);
R8void r8StateMachineBindIndexBuffer(R8IndexBuffer* indexBuffer);
R8void r8StateMachineBindexture(R8Texture* texture);

R8void r8StateMachineViewport(R8int x, R8int y, R8int width, R8int height);
R8void r8StateMachineDepthRange(R8float minDepth, R8float maxDepth);
R8void r8StateMachineScissor(R8int x, R8int y, R8int width, R8int height);
R8void r8StateMachineCullMode(R8enum mode);
R8void r8StateMachinePolygonMode(R8enum mode);

R8void r8StateMachineProjectionMatrix(const R8Mat4* matrix);
R8void r8StateMachineViewMatrix(const R8Mat4* matrix);
R8void r8StateMachineModelMatrix(const R8Mat4* matrix);

#endif
