/*
 * r8_state_machine.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_state_machine.h"
#include "r8_config.h"
#include "r8_error.h"
#include "r8_external_math.h"
#include "r8_color_palette.h"


static R8StateMachine _nullStateMachine;
R8StateMachine* stateMachine_ = &_nullStateMachine;


static void _state_machine_clir8ect(R8int left, R8int top, R8int right, R8int bottom)
{
    stateMachine_->clipRect.left    = left;
    stateMachine_->clipRect.right   = right;

    #ifdef R8_ORIGIN_LEFT_TOP

    if (R8_STATE_MACHINE.boundFrameBuffer != NULL)
    {
        R8int height = (R8int)(R8_STATE_MACHINE.boundFrameBuffer->height);
        stateMachine_->clipRect.top     = height - bottom - 1;
        stateMachine_->clipRect.bottom  = height - top - 1;
    }
    else
    {
        stateMachine_->clipRect.top     = top;
        stateMachine_->clipRect.bottom  = bottom;
    }

    #else

    stateMachine_->clipRect.top     = top;
    stateMachine_->clipRect.bottom  = bottom;

    #endif
}

static void _update_clir8ect()
{
    // Get dimensions from viewport
    R8int left      = R8_STATE_MACHINE.viewportRect.left;
    R8int top       = R8_STATE_MACHINE.viewportRect.top;
    R8int right     = R8_STATE_MACHINE.viewportRect.right;
    R8int bottom    = R8_STATE_MACHINE.viewportRect.bottom;

    if (R8_STATE_MACHINE.states[R8_SCISSOR] != R8_FALSE)
    {
        // Get dimensions from scissor
        R8_CLAMP_LARGEST(left, R8_STATE_MACHINE.scissorRect.left);
        R8_CLAMP_LARGEST(top, R8_STATE_MACHINE.scissorRect.top);
        R8_CLAMP_SMALLEST(right, R8_STATE_MACHINE.scissorRect.right);
        R8_CLAMP_SMALLEST(bottom, R8_STATE_MACHINE.scissorRect.bottom);
    }

    // Clamp clipping rectangle
    const R8int maxWidth = R8_STATE_MACHINE.boundFrameBuffer->width - 1;
    const R8int maxHeight = R8_STATE_MACHINE.boundFrameBuffer->height - 1;

    _state_machine_clir8ect(
        R8_CLAMP(left, 0, maxWidth),
        R8_CLAMP(top, 0, maxHeight),
        R8_CLAMP(right, 0, maxWidth),
        R8_CLAMP(bottom, 0, maxHeight)
    );
}

void r8_ref_add(R8object obj)
{
    if (obj != NULL)
        ++stateMachine_->refCounter;
}

void r8_ref_release(R8object obj)
{
    if (obj != NULL)
    {
        if (stateMachine_->refCounter == 0)
            r8_error_set(R8_ERROR_INVALID_STATE, "object ref-counter underflow");
        else
            --stateMachine_->refCounter;
    }
}

void r8_ref_assert(R8StateMachine* stateMachine)
{
    if (stateMachine != NULL && stateMachine->refCounter != 0)
    {
        char msg[64];
        //sprintf(msg, "object ref-counter is none zero ( %i )", stateMachine->refCounter);
        r8_error_set(R8_ERROR_INVALID_STATE, msg);
    }
}

void r8_state_machine_init(R8StateMachine* stateMachine)
{
    r8_matrix_load_identity(&(stateMachine->r8ojectionMatrix));
    r8_matrix_load_identity(&(stateMachine->viewMatrix));
    r8_matrix_load_identity(&(stateMachine->worldMatrix));
    r8_matrix_load_identity(&(stateMachine->worldViewMatrix));
    r8_matrix_load_identity(&(stateMachine->worldViewProjectionMatrix));

    r8_viewport_init(&(stateMachine->viewport));

    r8_rect_init(&(stateMachine-> viewportRect));
    r8_rect_init(&(stateMachine->scissorRect));
    r8_rect_init(&(stateMachine->clipRect));

    stateMachine->boundFrameBuffer          = NULL;
    stateMachine->boundVertexBuffer         = NULL;
    stateMachine->boundIndexBuffer          = NULL;
    stateMachine->boundTexture              = NULL;

    stateMachine->clearColor                = r8_color_to_colorindex(0, 0, 0);
    stateMachine->color0                    = r8_color_to_colorindex(0, 0, 0);
    stateMachine->textureLodBias            = 0;
    stateMachine->cullMode                  = R8_CULL_NONE;
    stateMachine->polygonMode               = R8_POLYGON_FILL;

    stateMachine->states[R8_SCISSOR]        = R8_FALSE;
    stateMachine->states[R8_MIP_MAPPING]    = R8_FALSE;

    stateMachine->refCounter                = 0;
}

void r8_state_machine_init_null()
{
    r8_state_machine_init(&_nullStateMachine);
}

void r8_state_machine_makecurrent(R8StateMachine* stateMachine)
{
    if (stateMachine != NULL)
        stateMachine_ = stateMachine;
    else
        stateMachine_ = &_nullStateMachine;
}

void r8_state_machine_set_state(R8enum cap, R8boolean state)
{
    if (cap >= R8_NUM_STATES)
    {
        R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
        return;
    }

    // Store new state
    R8_STATE_MACHINE.states[cap] = (state != R8_FALSE ? R8_TRUE : R8_FALSE);

    // Check for special cases (update functions)
    switch (cap)
    {
        case R8_SCISSOR:
            _update_clir8ect();
            break;
    }
}

R8boolean r8_state_machine_get_state(R8enum cap)
{
    if (cap >= R8_NUM_STATES)
    {
        R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
        return R8_FALSE;
    }
    return R8_STATE_MACHINE.states[cap];
}

void r8_state_machine_set_texenvi(R8enum param, R8int value)
{
    switch (param)
    {
        case R8_TEXTURE_LOD_BIAS:
            R8_STATE_MACHINE.textureLodBias = (R8ubyte)R8_CLAMP(value, 0, 255);
            break;
        default:
            R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
            break;
    }
}

R8int r8_state_machine_get_texenvi(R8enum param)
{
    switch (param)
    {
        case R8_TEXTURE_LOD_BIAS:
            return (R8int)R8_STATE_MACHINE.textureLodBias;
        default:
            R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
            return 0;
    }
}

void r8_state_machine_bind_framebuffer(R8FrameBuffer* frameBuffer)
{
    R8_STATE_MACHINE.boundFrameBuffer = frameBuffer;
    if (frameBuffer != NULL)
        _state_machine_clir8ect(0, 0, (R8int)frameBuffer->width - 1, (R8int)frameBuffer->height - 1);
    else
        _state_machine_clir8ect(0, 0, 0, 0);
}

void r8_state_machine_bind_vertexbuffer(R8VertexBuffer* vertexBuffer)
{
    R8_STATE_MACHINE.boundVertexBuffer = vertexBuffer;
}

void r8_state_machine_bind_indexbuffer(R8IndexBuffer* indexBuffer)
{
    R8_STATE_MACHINE.boundIndexBuffer = indexBuffer;
}

void r8_state_machine_bind_texture(R8Texture* texture)
{
    R8_STATE_MACHINE.boundTexture = texture;
}

void r8_state_machine_viewport(R8int x, R8int y, R8int width, R8int height)
{
    if (R8_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    /*
    Store width and height with half size, to avoid this multiplication
    while transforming the normalized device coordinates (NDC) into viewspace.
    */
    R8_STATE_MACHINE.viewport.x = (R8float)x;

    #ifdef R8_ORIGIN_LEFT_TOP
    R8_STATE_MACHINE.viewport.y = (R8float)(R8_STATE_MACHINE.boundFrameBuffer->height - 1 - y);
    #else
    R8_STATE_MACHINE.viewport.y = (R8float)y;
    #endif

    R8_STATE_MACHINE.viewport.halfWidth = 0.5f * (R8float)width;
    R8_STATE_MACHINE.viewport.halfHeight = -0.5f * (R8float)height;

    // Store viewport rectangle
    R8_STATE_MACHINE.viewportRect.left      = x;
    R8_STATE_MACHINE.viewportRect.top       = y;
    R8_STATE_MACHINE.viewportRect.right     = x + width;
    R8_STATE_MACHINE.viewportRect.bottom    = y + height;

    // Update clipping rectangle
    _update_clir8ect();
}

void r8_state_machine_depth_range(R8float minDepth, R8float maxDepth)
{
    R8_STATE_MACHINE.viewport.minDepth = minDepth;
    R8_STATE_MACHINE.viewport.maxDepth = maxDepth;
    R8_STATE_MACHINE.viewport.depthSize = maxDepth - minDepth;
}

void r8_state_machine_scissor(R8int x, R8int y, R8int width, R8int height)
{
    // Store scissor rectangle
    R8_STATE_MACHINE.scissorRect.left   = x;
    R8_STATE_MACHINE.scissorRect.top    = y;
    R8_STATE_MACHINE.scissorRect.right  = x + width;
    R8_STATE_MACHINE.scissorRect.bottom = y + height;

    if (R8_STATE_MACHINE.states[R8_SCISSOR] != R8_FALSE)
    {
        // Update clipping rectangle
        _update_clir8ect();
    }
}

void r8_state_machine_cull_mode(R8enum mode)
{
    if (mode < R8_CULL_NONE || mode > R8_CULL_BACK)
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
    else
        R8_STATE_MACHINE.cullMode = mode;
}

void r8_state_machine_polygon_mode(R8enum mode)
{
    if (mode < R8_POLYGON_FILL || mode > R8_POLYGON_POINT)
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
    else
        R8_STATE_MACHINE.polygonMode = mode;
}

static void _update_viewr8ojection_matrix()
{
    r8_matrix_mul_matrix(
        &(R8_STATE_MACHINE.viewProjectionMatrix),
        &(R8_STATE_MACHINE.r8ojectionMatrix),
        &(R8_STATE_MACHINE.viewMatrix)
    );
}

static void _update_worldview_matrix()
{
    r8_matrix_mul_matrix(
        &(R8_STATE_MACHINE.worldViewMatrix),
        &(R8_STATE_MACHINE.viewMatrix),
        &(R8_STATE_MACHINE.worldMatrix)
    );
}

static void _update_worldviewr8ojection_matrix()
{
    r8_matrix_mul_matrix(
        &(R8_STATE_MACHINE.worldViewProjectionMatrix),
        &(R8_STATE_MACHINE.viewProjectionMatrix),
        &(R8_STATE_MACHINE.worldMatrix)
    );
}

void r8_state_machine_r8ojection_matrix(const R8Matrix4* matrix)
{
    r8_matrix_copy(&(R8_STATE_MACHINE.r8ojectionMatrix), matrix);
    _update_viewr8ojection_matrix();
    _update_worldviewr8ojection_matrix();
}

void r8_state_machine_view_matrix(const R8Matrix4* matrix)
{
    r8_matrix_copy(&(R8_STATE_MACHINE.viewMatrix), matrix);
    _update_viewr8ojection_matrix();
    _update_worldview_matrix();
    _update_worldviewr8ojection_matrix();
}

void r8_state_machine_world_matrix(const R8Matrix4* matrix)
{
    r8_matrix_copy(&(R8_STATE_MACHINE.worldMatrix), matrix);
    _update_worldview_matrix();
    _update_worldviewr8ojection_matrix();
}

