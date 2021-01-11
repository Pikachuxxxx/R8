/* r8.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8.h"
#include "r8_framebuffer.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"
#include "r8_texture.h"
#include "r8_image.h"
#include "r8_state_machine.h"
#include "r8_global_state.h"
#include "r8_renderer.h"
#include "r8_memory.h"
#include "context.h"

#include <string.h> // memset


/****************************************************
 *                                                  *
 *                 Initialization                   *
 *                                                  *
 ****************************************************/

R8bool r8Ignite()
{
    r8InitNullStateMachine();
    r8GlobalStateInit();
    return R8_TRUE;
}

R8bool r8Release()
{
    r8GlobalStateRelease();
    return R8_TRUE;
}

R8enum r8GetError()
{
    return r8GetError();
}

const char* r8GetString()
{
    // TODO: To be implemented yet
}

R8void r8ErrorHandle(R8_ERROR_HANDLER_PROC errorHandler)
{
    r8SetErrorHandler(errorHandler);
}

/****************************************************
 *                                                  *
 *                    Context                       *
 *                                                  *
 ****************************************************/

R8object r8CreateContext(const R8ContextDesc* contextDesc, R8int width, R8int height)
{
    return (R8object)r8ContextCreate(contextDesc, width, height);
}

R8void r8DeleteContext(R8object contextObject)
{
    r8ContextDelete((R8Context*)contextObject);
}

R8void r8MakeContextCurrent(R8object context)
{
    r8ContextMakeCurrent((R8Context*)context);
}

R8void r8RenderPass(R8object context)
{
    r8ContextPresent((R8Context*)context, R8_STATE_MACHINE.framebuffer);
}

/****************************************************
 *                                                  *
 *                  Frame Buffer                    *
 *                                                  *
 ****************************************************/

R8object r8CreateFrameBuffer(R8int width, R8int height)
{
    return (R8object)r8FrameBufferGenerate(width, height);
}

R8void r8DeleteFrameBuffer(R8object fbo)
{
    r8FrameBufferDelete((R8FrameBuffer*)fbo);
}

R8void r8BindFrameBuffer(R8object fbo)
{
    r8StateMachineBindFrameBuffer((R8FrameBuffer*)fbo);
}

R8void r8ClearFrameBuffer(R8object fbo, R8int depthMaskValue, R8bit clearFlags)
{
    r8FrameBufferClear((R8FrameBuffer*)fbo, depthMaskValue, clearFlags);
}

/****************************************************
 *                                                  *
 *                  Vertex Buffer                   *
 *                                                  *
 ****************************************************/

R8object r8CreateVertexBuffer()
{
    return (R8object)r8VertexBufferGenerate();
}

R8void r8DeleteVertxBuffer(R8object vbo)
{
    r8VertexBufferDelete((R8VertexBuffer*)vbo);
}

R8void r8VertexBufferData(R8object vbo, const R8void* vertices, const R8void* texCoords, const R8void* color, R8sizei stride, R8sizei numVerts)
{
     r8VertexBufferAddData(vbo, numVerts, vertices, texCoords, color, stride);
}

R8void r8BindVertexBuffer(R8object vbo)
{
    r8StateMachineBindVertexBuffer(vbo);
}

/****************************************************
 *                                                  *
 *                  Index Buffer                    *
 *                                                  *
 ****************************************************/

R8object r8CreateIndexBuffer()
{
    return (R8object)r8IndexBufferCreate();
}

R8void r8DeleteIndexBuffer(R8object ibo)
{
    r8IndexBufferDelete((R8IndexBuffer*)ibo);
}

R8void r8IndexBufferData(R8object ibo, const R8void* indices, R8sizei indicesCount)
{
    r8IndexBufferAddData((R8IndexBuffer*)ibo, indices, indicesCount);
}

R8void r8BindIndexBuffer(R8object ibo)
{
    r8StateMachineBindIndexBuffer((R8IndexBuffer*)ibo);
}

/****************************************************
 *                                                  *
 *                      Texture                     *
 *                                                  *
 ****************************************************/

R8object r8GenerateTexture()
{
    return (R8Texture*)r8TextureGenerate();
}

R8void r8DeleteTexture(R8object texture)
{
    r8TextureDelete((R8Texture*)texture);
}

R8void r8BindTexture(R8object texture)
{
    r8StateMachineBindexture((R8Texture*)texture);
}

R8void r8TexImage2D(R8object texture, R8int width, R8int height, R8enum format, const R8void* data, R8bool dither, R8bool generateMips)
{
    r8TextureImage2d((R8Texture*)texture, width, height, format, data, dither, generateMips);
}

R8void r8GenerateMipMaps(R8object texture)
{
    // TODO: To be implemented yet
}

R8void r8TexEnvi(R8enum param, R8int value)
{
    r8StateMachineSetTexenvi(param, value);
}

/****************************************************
 *                                                  *
 *             MVP and Transformations              *
 *                                                  *
 ****************************************************/

R8void r8ModelMatrix(const R8float* matrix4x4)
{
    r8StateMachineModelMatrix((R8Mat4*)matrix4x4);
}

R8void r8ViewMatrix(const R8float* matrix4x4)
{
    r8StateMachineViewMatrix((R8Mat4*)matrix4x4);
}

R8void r8ProjectionMatrix(const R8float* matrix4x4)
{
    r8StateMachineProjectionMatrix((R8Mat4*)matrix4x4);
}

R8void r8GeneratePerpspectiveMatrix(const R8float* matrix4x4, R8float fov, R8float aspectRatio, R8float nearPlane, R8float farPlane)
{
    r8Mat4Perspective((R8Mat4*)matrix4x4, fov, aspectRatio, nearPlane, farPlane);

}

R8void r8GenerateOrthographicMatrix(const R8float* matrix4x4, R8float left, R8float right, R8float top, R8float bottom, R8float nearPlane, R8float farPlane)
{
    r8Mat4Orthographic((R8Mat4*)matrix4x4, left, right, top, bottom, nearPlane, farPlane);
}

R8void r8Translate(const R8float* matrix4x4, R8float x, R8float y, R8float z)
{
    r8Mat4Translation((R8Mat4*)matrix4x4, x, y, z);
}

R8void r8Rotate(const R8float* matrix4x4, R8float x, R8float y, R8float z, R8float angle)
{
    r8Mat4Rotate((R8Mat4*)matrix4x4, x, y, z, angle);

}

R8void r8Scale(const R8float* matrix4x4, R8float x, R8float y, R8float z)
{
    r8Mat4Scale((R8Mat4*)matrix4x4, x, y, z);
}

R8void r8Identity(R8float* matrix4x4)
{
    r8Mat4LoadIdentity((R8Mat4*)matrix4x4);
}

/****************************************************
 *                                                  *
 *                 Render States                    *
 *                                                  *
 ****************************************************/

R8void r8SetState(R8enum cap, R8bool state)
{
    r8StateMachineSetState(cap, state);
}

R8void r8Enable(R8enum functionality)
{
    r8StateMachineSetState(functionality, R8_TRUE);
}

R8void r8Disable(R8enum functionality)
{
    r8StateMachineSetState(functionality, R8_FALSE);
}

R8void r8CullMode(R8enum mode)
{
    r8StateMachineCullMode(mode);
}

R8void r8PolygonDrawMode(R8enum mode)
{
    r8StateMachinePolygonMode(mode);
}

R8void r8SetDepthRange(R8float min, R8float max)
{
    r8StateMachineDepthRange(min, max);
}

R8void r8Viewport(R8int x, R8int y, R8int width, R8int height)
{
    r8StateMachineViewport(x, y, width, height);
}

R8void r8ScissorRect(R8int x, R8int y, R8int width, R8int height)
{
    r8StateMachineScissor(x, y, width, height);
}

/****************************************************
 *                                                  *
 *                Drawing functions                 *
 *                                                  *
 ****************************************************/

R8void r8ClearColor(R8ubyte r, R8ubyte g, R8ubyte b)
{
    R8_STATE_MACHINE.clearColor = r8RGBToColorBuffer(r, g, b);
}

R8void r8BindColor(R8ubyte r, R8ubyte g, R8ubyte b)
{
    R8_STATE_MACHINE.color0 = r8RGBToColorBuffer(r, g, b);
}

R8void r8Draw(R8enum primitive, R8ushort verticesCount, R8ushort vertexOffset)
{
    switch (primitive)
    {
    case R8_POINTS:
        r8RenderPoints(verticesCount, vertexOffset, R8_STATE_MACHINE.vertexbuffer);
        break;

    case R8_LINES:
        r8RenderLines(verticesCount, vertexOffset, R8_STATE_MACHINE.vertexbuffer);
        break;

    case R8_TRIANGLES:
        r8RenderTriangles(verticesCount, vertexOffset, R8_STATE_MACHINE.vertexbuffer);
        break;
    case R8_TRIANGLES_STRIP:
        r8RenderTrianglesStrip(verticesCount, vertexOffset, R8_STATE_MACHINE.vertexbuffer);
        break;

    default:
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        break;
    }
}

R8void r8DrawIndexed(R8enum primitive, R8ushort indicesCount, R8ushort indexOffset)
{
    switch (primitive)
    {
    case R8_POINTS:
        r8RenderIndexedPoints(indicesCount, indexOffset, R8_STATE_MACHINE.vertexbuffer, R8_STATE_MACHINE.indexbuffer);
        break;

    case R8_LINES:
        r8RenderLinesIndexed(indicesCount, indexOffset, R8_STATE_MACHINE.vertexbuffer, R8_STATE_MACHINE.indexbuffer);
        break;

    case R8_TRIANGLES:
        r8RenderIndexedTriangles(indicesCount, indexOffset, R8_STATE_MACHINE.vertexbuffer, R8_STATE_MACHINE.indexbuffer);
        break;
    case R8_TRIANGLES_STRIP:
        r8RenderIndexedTrianglesStrip(indicesCount, indexOffset, R8_STATE_MACHINE.vertexbuffer, R8_STATE_MACHINE.indexbuffer);
        break;

    default:
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        break;
    }
}

//----------------Drawing utility functions--------------

R8void r8DrawPoint(R8float x, R8float y)
{
    r8RenderScreenSpacePoint(x, y);
}

R8void r8DrawLine(R8float x1, R8float y1, R8float x2, R8float y2)
{
    r8RenderScreenSpaceLine(x1, y1, x2, y2);

}

R8void r8DrawTexture(R8int left, R8int top, R8int right, R8int bottom)
{
    r8RenderScreenSpaceImage(left, top, right, bottom);
}

/****************************************************
 *                                                  *
 *              Immediate mode Drawing              *
 *                                                  *
 ****************************************************/

R8void r8Begin(R8enum primitive)
{
    r8ImmediateModeBegin(primitive);
}

R8void r8End()
{
    r8ImmediateModeEnd();
}

R8void r8Vertex2f(R8float x, R8float y)
{
    r8ImmediateModeVertex(x, y, 0.0f, 1.0f);
}

R8void r8Vertex3f(R8float x, R8float y, R8float z)
{
    r8ImmediateModeVertex(x, y, z, 1.0f);
}

R8void r8Vertex4f(R8float x, R8float y, R8float z, R8float w)
{
    r8ImmediateModeVertex(x, y, z, w);
}

R8void r8TexCoord2f(R8float s, R8float t)
{
    r8ImmediateModeTexCoord(s, t);
}

R8void r8VertexColor4f(R8float r, R8float g, R8float b, R8float a)
{
    // TODO: To be implemented
}