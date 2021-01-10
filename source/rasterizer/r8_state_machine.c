/* r8_state_machine.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_state_machine.h"
#include "r8_config.h"
#include "r8_error.h"
#include "r8_external_math.h"
#include "r8_color_palette.h"

static R8StateMachine nullStateMachine_;

R8StateMachine* stateMachine_ = &(nullStateMachine_);

static R8void r8StateMachineClipRect(R8int left, R8int top, R8int right, R8int bottom)
{
    stateMachine_->clipQuad.left    = left;
    stateMachine_->clipQuad.right   = right;

    #ifdef R8_ORIGIN_TOP_LEFT

    if (R8_STATE_MACHINE.framebuffer != NULL)
    {
        R8int height = (R8int)(R8_STATE_MACHINE.framebuffer->height);
        stateMachine_->clipQuad.top     = height - bottom - 1;
        stateMachine_->clipQuad.bottom  = height - top - 1;
    }
    else
    {
        stateMachine_->clipQuad.top     = top;
        stateMachine_->clipQuad.bottom  = bottom;
    }

    #else

    stateMachine_->clipQuad.top     = top;
    stateMachine_->clipQuad.bottom  = bottom;

    #endif
}

static void r8UpdateClipRect()
{
    // Get dimensions from viewport
    R8int left      = R8_STATE_MACHINE.viewportQuad.left;
    R8int top       = R8_STATE_MACHINE.viewportQuad.top;
    R8int right     = R8_STATE_MACHINE.viewportQuad.right;
    R8int bottom    = R8_STATE_MACHINE.viewportQuad.bottom;

    if (R8_STATE_MACHINE.states[R8_SCISSOR_MODE] != R8_FALSE)
    {
        // Get dimensions from scissor
        R8_CLAMP_LARGEST(left,      R8_STATE_MACHINE.scissorQuad.left);
        R8_CLAMP_LARGEST(top,       R8_STATE_MACHINE.scissorQuad.top);
        R8_CLAMP_SMALLEST(right,    R8_STATE_MACHINE.scissorQuad.right);
        R8_CLAMP_SMALLEST(bottom,   R8_STATE_MACHINE.scissorQuad.bottom);
    }

    // Clamp clipping rectangle
    const R8int maxWidth = R8_STATE_MACHINE.framebuffer->width - 1;
    const R8int maxHeight = R8_STATE_MACHINE.framebuffer->height - 1;

    r8StateMachineClipRect(
        R8_CLAMP(left, 0, maxWidth),
        R8_CLAMP(top, 0, maxHeight),
        R8_CLAMP(right, 0, maxWidth),
        R8_CLAMP(bottom, 0, maxHeight)
    );
}

R8void r8AddSMRef(R8object object)
{
    if (object != NULL)
        ++stateMachine_->refCounter;
}

R8void r8ReleaseSMRef(R8object object)
{
    if(object != NULL)
    {
        if(stateMachine_->refCounter == NULL)
            R8_ERROR(R8_ERROR_NULL_POINTER);
        else
            --stateMachine_->refCounter;
    }
}

R8void r8AssertSMRef(R8StateMachine* stateMachine)
{
    if(stateMachine != NULL && stateMachine->refCounter != 0)
    {
        printf("object ref-counter is none zero ( %i )", stateMachine->refCounter);
        R8_ERROR(R8_ERROR_INVALID_STATE);
    }
}

R8void r8InitStateMachine(R8StateMachine* stateMachine)
{
    r8Mat4LoadIdentity(&(stateMachine->projectionMatrix));
    r8Mat4LoadIdentity(&(stateMachine->viewMatrix));
    r8Mat4LoadIdentity(&(stateMachine->modelMatrix));
    r8Mat4LoadIdentity(&(stateMachine->modelViewMatrix));
    r8Mat4LoadIdentity(&(stateMachine->MVPMatrix));

    r8InitViewport(&(stateMachine->viewport));

    InitQuad(&(stateMachine->viewportQuad));
    InitQuad(&(stateMachine->scissorQuad));
    InitQuad(&(stateMachine->clipQuad));

    stateMachine->framebuffer          = NULL;
    stateMachine->vertexbuffer         = NULL;
    stateMachine->indexbuffer          = NULL;
    stateMachine->texture              = NULL;

    // TODO: convert the arguments to single color argument
    stateMachine->clearColor                = r8RGBToColorBuffer(0, 0, 0);
    stateMachine->color0                    = r8RGBToColorBuffer(0, 0, 0);
    stateMachine->textureLODBias = 0;
    stateMachine->cullMode                  = R8_CULL_NONE;
    stateMachine->polygonMode               = R8_POLYGON_FILL;

    stateMachine->states[R8_SCISSOR_MODE]    = R8_FALSE;
    stateMachine->states[R8_MIPMAP_MODE]    = R8_FALSE;

    stateMachine->refCounter                = 0;
}

R8void r8InitNullStateMachine()
{
    r8InitStateMachine(&nullStateMachine_);
}

R8void r8StateMachineMakeCurrent(R8StateMachine* stateMachine)
{
    if(stateMachine != NULL)
        stateMachine_ = stateMachine;
    else
        stateMachine_ = &nullStateMachine_;
}

R8void r8StateMachineSetState(R8enum cap, R8bool state)
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
        case R8_SCISSOR_MODE:
            r8UpdateClipRect();
            break;
    }
}

R8bool r8StateMachineGetState(R8enum cap)
{
    if (cap >= R8_NUM_STATES)
    {
        R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
        return R8_FALSE;
    }
    return R8_STATE_MACHINE.states[cap];
}

R8void r8StateMachineSetTexenvi(R8enum param, R8int value)
{
    switch (param)
    {
        case R8_TEXTURE_LOD_BIAS:
            R8_STATE_MACHINE.textureLODBias = (R8ubyte)R8_CLAMP(value, 0, 255);
            break;
        default:
            R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
            break;
    }
}

R8int r8StateMachineGetTexenvi(R8enum param)
{
    switch (param)
    {
        case R8_TEXTURE_LOD_BIAS:
            return (R8int)R8_STATE_MACHINE.textureLODBias;
        default:
            R8_ERROR(R8_ERROR_INDEX_OUT_OF_BOUNDS);
            return 0;
    }
}

R8void r8StateMachineBindFrameBuffer(R8FrameBuffer* frameBuffer)
{
    R8_STATE_MACHINE.framebuffer = frameBuffer;
    if (frameBuffer != NULL)
        r8StateMachineClipRect(0, 0, (R8int)frameBuffer->width - 1, (R8int)frameBuffer->height - 1);
    else
        r8StateMachineClipRect(0, 0, 0, 0);
}
R8void r8StateMachineBindVertexBuffer(R8VertexBuffer* vertexBuffer)
{
    R8_STATE_MACHINE.vertexbuffer = vertexBuffer;

}

R8void r8StateMachineBindIndexBuffer(R8IndexBuffer* indexBuffer)
{
    R8_STATE_MACHINE.indexbuffer = indexBuffer;
}

R8void r8StateMachineBindexture(R8Texture* texture)
{
    R8_STATE_MACHINE.texture = texture;
}

R8void r8StateMachineViewport(R8int x, R8int y, R8int width, R8int height)
{
    if (R8_STATE_MACHINE.framebuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    /*
    Store width and height with half size, to avoid this multiplication
    while transforming the normalized device coordinates (NDC) into view space.
    */
    R8_STATE_MACHINE.viewport.x = (R8float)x;

    #ifdef R8_ORIGIN_TOP_LEFT
    R8_STATE_MACHINE.viewport.y = (R8float)(R8_STATE_MACHINE.framebuffer->height - 1 - y);
    #else
    R8_STATE_MACHINE.viewport.y = (R8float)y;
    #endif

    R8_STATE_MACHINE.viewport.halfWidth = 0.5f * (R8float)width;
    R8_STATE_MACHINE.viewport.halfHeight = -0.5f * (R8float)height;

    // Store viewport rectangle
    R8_STATE_MACHINE.viewportQuad.left      = x;
    R8_STATE_MACHINE.viewportQuad.top       = y;
    R8_STATE_MACHINE.viewportQuad.right     = x + width;
    R8_STATE_MACHINE.viewportQuad.bottom    = y + height;

    // Update clipping rectangle
    r8UpdateClipRect();
}

R8void r8StateMachineDepthRange(R8float minDepth, R8float maxDepth)
{
    R8_STATE_MACHINE.viewport.minDepth = minDepth;
    R8_STATE_MACHINE.viewport.maxDepth = maxDepth;
    R8_STATE_MACHINE.viewport.depthSize = maxDepth - minDepth;
}

R8void r8StateMachineScissor(R8int x, R8int y, R8int width, R8int height)
{
    // Store scissor rectangle
    R8_STATE_MACHINE.scissorQuad.left = x;
    R8_STATE_MACHINE.scissorQuad.top = y;
    R8_STATE_MACHINE.scissorQuad.right = x + width;
    R8_STATE_MACHINE.scissorQuad.bottom = y + height;

    if (R8_STATE_MACHINE.states[R8_SCISSOR_MODE] != R8_FALSE)
    {
        // Update clipping rectangle
        r8UpdateClipRect();
    }
}

R8void r8StateMachineCullMode(R8enum mode)
{
    if (mode < R8_CULL_NONE || mode > R8_CULL_BACK)
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
    else
        R8_STATE_MACHINE.cullMode = mode;
}

R8void r8StateMachinePolygonMode(R8enum mode)
{
    if (mode < R8_POLYGON_FILL || mode > R8_POLYGON_POINT)
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
    else
        R8_STATE_MACHINE.polygonMode = mode;
}

static void _update_viewprojection_matrix()
{
    r8Mat4MultiplyMatrix(
        &(R8_STATE_MACHINE.viewProjectionMatrix),
        &(R8_STATE_MACHINE.projectionMatrix),
        &(R8_STATE_MACHINE.viewMatrix)
    );
}

static void _update_worldview_matrix()
{
    r8Mat4MultiplyMatrix(
        &(R8_STATE_MACHINE.modelViewMatrix),
        &(R8_STATE_MACHINE.viewMatrix),
        &(R8_STATE_MACHINE.modelMatrix)
    );
}

static void _update_worldviewprojection_matrix()
{
    r8Mat4MultiplyMatrix(
        &(R8_STATE_MACHINE.MVPMatrix),
        &(R8_STATE_MACHINE.viewProjectionMatrix),
        &(R8_STATE_MACHINE.modelMatrix)
    );
}

R8void r8StateMachineProjectionMatrix(const R8Mat4* matrix)
{
    r8Mat4Copy(&(R8_STATE_MACHINE.projectionMatrix), matrix);

    _update_viewprojection_matrix();
    _update_worldviewprojection_matrix();
}
R8void r8StateMachineViewMatrix(const R8Mat4* matrix)
{
    r8Mat4Copy(&(R8_STATE_MACHINE.viewMatrix), matrix);

    _update_viewprojection_matrix();
    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}

R8void r8StateMachineModelMatrix(const R8Mat4* matrix)
{
    r8Mat4Copy(&(R8_STATE_MACHINE.modelMatrix), matrix);

    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}
