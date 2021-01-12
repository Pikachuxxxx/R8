/*
 * r8_state_machine.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_STATE_MACHINE_H
#define R8_STATE_MACHINE_H


#include "r8_matrix4.h"
#include "r8_viewport.h"
#include "r8_rect.h"
#include "r8_framebuffer.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"
#include "r8_texture.h"
#include "r8_macros.h"


#define R8_STATE_MACHINE    (*stateMachine_)
#define R8_NUM_STATES       2


typedef struct R8StateMachine
{
    R8Matrix4          r8ojectionMatrix;
    R8Matrix4          viewMatrix;
    R8Matrix4          worldMatrix;
    R8Matrix4          viewProjectionMatrix;
    R8Matrix4          worldViewMatrix;
    R8Matrix4          worldViewProjectionMatrix;

    R8Viewport         viewport;

    R8Rect             viewportRect;
    R8Rect             scissorRect;
    R8Rect             clipRect;

    R8FrameBuffer*     boundFrameBuffer;
    R8VertexBuffer*    boundVertexBuffer;
    R8IndexBuffer*     boundIndexBuffer;
    R8Texture*         boundTexture;

    R8ColorBuffer        clearColor;
    R8ColorBuffer        color0;                 // Active color index
    R8ubyte             textureLodBias;

    R8enum              cullMode;
    R8enum              polygonMode;

    R8boolean           states[R8_NUM_STATES];

    R8sizei             refCounter;                 // Object reference counter
}
R8StateMachine;


/// Reference to the state machine of the current context.
extern R8StateMachine* stateMachine_;


void r8_ref_add(R8object obj);
void r8_ref_release(R8object obj);
void r8_ref_assert(R8StateMachine* stateMachine);

void r8_state_machine_init(R8StateMachine* stateMachine);
void r8_state_machine_init_null();

void r8_state_machine_makecurrent(R8StateMachine* stateMachine);

void r8_state_machine_set_state(R8enum cap, R8boolean state);
R8boolean r8_state_machine_get_state(R8enum cap);

void r8_state_machine_set_texenvi(R8enum param, R8int value);
R8int r8_state_machine_get_texenvi(R8enum param);

void r8_state_machine_bind_framebuffer(R8FrameBuffer* frameBuffer);
void r8_state_machine_bind_vertexbuffer(R8VertexBuffer* vertexBuffer);
void r8_state_machine_bind_indexbuffer(R8IndexBuffer* indexBuffer);
void r8_state_machine_bind_texture(R8Texture* texture);

void r8_state_machine_viewport(R8int x, R8int y, R8int width, R8int height);
void r8_state_machine_depth_range(R8float minDepth, R8float maxDepth);
void r8_state_machine_scissor(R8int x, R8int y, R8int width, R8int height);
void r8_state_machine_cull_mode(R8enum mode);
void r8_state_machine_polygon_mode(R8enum mode);

void r8_state_machine_r8ojection_matrix(const R8Matrix4* matrix);
void r8_state_machine_view_matrix(const R8Matrix4* matrix);
void r8_state_machine_world_matrix(const R8Matrix4* matrix);


#endif
