/*
 * r8.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8.h"
#include "r8_error.h"
#include "context.h"
#include "r8_framebuffer.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"
#include "r8_texture.h"
#include "r8_image.h"
#include "r8_state_machine.h"
#include "r8_global_state.h"
#include "r8_renderer.h"
#include "r8_memory.h"

#include <string.h>


// --- common --- //

R8boolean r8Init()
{
    r8_state_machine_init_null();
    r8_global_state_init();
    return R8_TRUE;
}

R8boolean r8Release()
{
    r8_global_state_release();
    return R8_TRUE;
}

R8enum r8GetError()
{
    return r8_error_get();
}

void r8ErrorHandler(R8_ERROR_HANDLER_PROC errorHandler)
{
    r8_error_set_handler(errorHandler);
}

const char* r8GetString(R8enum str)
{
    switch (str)
    {
        case R8_STRING_VERSION:
            return R8_VERSION_STR;
        case R8_STRING_RENDERER:
            return "R8";
        case R8_STRING_PLUGINS:
            #ifdef R8_INCLUDE_PLUGINS
            return "stb_image;";
            #else
            return "";
            #endif
    }
    return NULL;
}

R8int r8GetIntegerv(R8enum param)
{
    switch (param)
    {
        case R8_MAX_TEXTURE_SIZE:
            return R8_MAX_TEX_SIZE;
    }
    return 0;
}

// --- context --- //

R8object r8CreateContext(const R8contextdesc* desc, R8uint width, R8uint height)
{
    return (R8object)r8_context_create(desc, width, height);
}

void r8DeleteContext(R8object context)
{
    r8_context_delete((R8Context*)context);
}

void r8MakeCurrent(R8object context)
{
    r8_context_makecurrent((R8Context*)context);
}

void r8Present(R8object context)
{
    r8_context_present((R8Context*)context, R8_STATE_MACHINE.boundFrameBuffer);
}

// --- framebuffer --- //

R8object r8CreateFrameBuffer(R8uint width, R8uint height)
{
    return (R8object)r8_framebuffer_create(width, height);
}

void r8DeleteFrameBuffer(R8object frameBuffer)
{
    r8_framebuffer_delete((R8FrameBuffer*)frameBuffer);
}

void r8BindFrameBuffer(R8object frameBuffer)
{
    r8_state_machine_bind_framebuffer((R8FrameBuffer*)frameBuffer);
}

void r8ClearFrameBuffer(R8object frameBuffer, R8float clearDepth, R8bitfield clearFlags)
{
    r8_framebuffer_clear((R8FrameBuffer*)frameBuffer, clearDepth, clearFlags);
}

// --- texture --- //

R8object r8CreateTexture()
{
    return (R8object)r8_texture_create();
}

void r8DeleteTexture(R8object texture)
{
    r8_texture_delete((R8Texture*)texture);
}

void r8BindTexture(R8object texture)
{
    r8_state_machine_bind_texture((R8Texture*)texture);
}

void r8TexImage2D(
    R8object texture, R8texsize width, R8texsize height, R8enum format,
    const R8void* data, R8boolean dither, R8boolean generateMips)
{
    r8_texture_image2d((R8Texture*)texture, width, height, format, data, dither, generateMips);
}

void r8TexImage2DFromFile(
    R8object texture, const char* filename, R8boolean dither, R8boolean generateMips)
{
    R8Image* image = r8_image_load_from_file(filename);

    r8_texture_image2d(
        (R8Texture*)texture,
        (R8texsize)(image->width),
        (R8texsize)(image->height),
        R8_UBYTE_RGB,
        image->colors,
        dither,
        generateMips
    );

    r8_image_delete(image);
}

void r8TexEnvi(R8enum param, R8int value)
{
    r8_state_machine_set_texenvi(param, value);
}

R8int r8GetTexLevelParameteri(R8object texture, R8ubyte mipLevel, R8enum param)
{
    return r8_texture_get_mip_parameter((const R8Texture*)texture, mipLevel, param);
}

// --- vertexbuffer --- //

R8object r8CreateVertexBuffer()
{
    return (R8object)r8_vertexbuffer_create();
}

void r8DeleteVertexBuffer(R8object vertexBuffer)
{
    r8_vertexbuffer_delete((R8VertexBuffer*)vertexBuffer);
}

void r8VertexBufferData(R8object vertexBuffer, R8sizei numVertices, const R8void* coords, const R8void* texCoords, R8sizei vertexStride)
{
    r8_vertexbuffer_data((R8VertexBuffer*)vertexBuffer, numVertices, coords, texCoords, vertexStride);
}

void r8VertexBufferDataFromFile(R8object vertexBuffer, R8sizei* numVertices, FILE* file)
{
    r8_vertexbuffer_data_from_file((R8VertexBuffer*)vertexBuffer, numVertices, file);
}

void r8BindVertexBuffer(R8object vertexBuffer)
{
    r8_state_machine_bind_vertexbuffer((R8VertexBuffer*)vertexBuffer);
}

// --- indexbuffer --- //

R8object r8CreateIndexBuffer()
{
    return (R8object)r8_indexbuffer_create();
}

void r8DeleteIndexBuffer(R8object indexBuffer)
{
    r8_indexbuffer_delete((R8IndexBuffer*)indexBuffer);
}

void r8IndexBufferData(R8object indexBuffer, const R8ushort* indices, R8sizei numIndices)
{
    r8_indexbuffer_data((R8IndexBuffer*)indexBuffer, indices, numIndices);
}

void r8IndexBufferDataFromFile(R8object indexBuffer, R8sizei* numIndices, FILE* file)
{
    r8_indexbuffer_data_from_file((R8IndexBuffer*)indexBuffer, numIndices, file);
}

void r8BindIndexBuffer(R8object indexBuffer)
{
    r8_state_machine_bind_indexbuffer((R8IndexBuffer*)indexBuffer);
}

// --- matrices --- //

void r8ProjectionMatrix(const R8float* matrix4x4)
{
    r8_state_machine_r8ojection_matrix((R8Matrix4*)matrix4x4);
}

void r8ViewMatrix(const R8float* matrix4x4)
{
    r8_state_machine_view_matrix((R8Matrix4*)matrix4x4);
}

void r8WorldMatrix(const R8float* matrix4x4)
{
    r8_state_machine_world_matrix((R8Matrix4*)matrix4x4);
}

void r8BuildPerspectiveProjection(
    R8float* matrix4x4, R8float aspectRatio, R8float nearPlane, R8float farPlane, R8float fov)
{
    r8_matrix_build_perspective((R8Matrix4*)matrix4x4, aspectRatio, nearPlane, farPlane, fov);
}

void r8BuildOrthogonalProjection(
    R8float* matrix4x4, R8float width, R8float height, R8float nearPlane, R8float farPlane)
{
    r8_matrix_build_orthogonal((R8Matrix4*)matrix4x4, width, height, nearPlane, farPlane);
}

void r8Translate(R8float* matrix4x4, R8float x, R8float y, R8float z)
{
    r8_matrix_translate((R8Matrix4*)matrix4x4, x, y, z);
}

void r8Rotate(R8float* matrix4x4, R8float x, R8float y, R8float z, R8float angle)
{
    r8_matrix_rotate((R8Matrix4*)matrix4x4, x, y, z, angle);
}

void r8Scale(R8float* matrix4x4, R8float x, R8float y, R8float z)
{
    r8_matrix_scale((R8Matrix4*)matrix4x4, x, y, z);
}

void r8LoadIdentity(R8float* matrix4x4)
{
    r8_matrix_load_identity((R8Matrix4*)matrix4x4);
}

// --- states --- //

void r8SetState(R8enum cap, R8boolean state)
{
    r8_state_machine_set_state(cap, state);
}

R8boolean r8GetState(R8enum cap)
{
    return r8_state_machine_get_state(cap);
}

void r8Enable(R8enum cap)
{
    r8_state_machine_set_state(cap, R8_TRUE);
}

void r8Disable(R8enum cap)
{
    r8_state_machine_set_state(cap, R8_FALSE);
}

void r8Viewport(R8int x, R8int y, R8int width, R8int height)
{
    r8_state_machine_viewport(x, y, width, height);
}

void r8Scissor(R8int x, R8int y, R8int width, R8int height)
{
    r8_state_machine_scissor(x, y, width, height);
}

void r8DepthRange(R8float minDepth, R8float maxDepth)
{
    r8_state_machine_depth_range(minDepth, maxDepth);
}

void r8CullMode(R8enum mode)
{
    r8_state_machine_cull_mode(mode);
}

void r8PolygonMode(R8enum mode)
{
    r8_state_machine_polygon_mode(mode);
}

// --- drawing --- //

void r8ClearColor(R8ubyte r, R8ubyte g, R8ubyte b)
{
    R8_STATE_MACHINE.clearColor = r8_color_to_colorindex(r, g, b);
}

void r8Color(R8ubyte r, R8ubyte g, R8ubyte b)
{
    R8_STATE_MACHINE.color0 = r8_color_to_colorindex(r, g, b);
}

void r8DrawScreenPoint(R8int x, R8int y)
{
    r8_render_screenspace_point(x, y);
}

void r8DrawScreenLine(R8int x1, R8int y1, R8int x2, R8int y2)
{
    r8_render_screenspace_line(x1, y1, x2, y2);
}

void r8DrawScreenImage(R8int left, R8int top, R8int right, R8int bottom)
{
    r8_render_screenspace_image(left, top, right, bottom);
}

void r8Draw(R8enum priitives, R8ushort numVertices, R8ushort firstVertex)
{
    switch (priitives)
    {
        case R8_POINTS:
            r8_render_points(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;

        case R8_LINES:
            r8_render_lines(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;
        case R8_LINE_STRIP:
            r8_render_line_strip(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;
        case R8_LINE_LOOP:
            r8_render_line_loop(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;

        case R8_TRIANGLES:
            r8_render_triangles(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;
        case R8_TRIANGLE_STRIP:
            r8_render_triangle_strip(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;
        case R8_TRIANGLE_FAN:
            r8_render_triangle_fan(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer);
            break;

        default:
            R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
            break;
    }
}

void r8DrawIndexed(R8enum priitives, R8ushort numVertices, R8ushort firstVertex)
{
    switch (priitives)
    {
        case R8_POINTS:
            r8_render_indexed_points(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;

        case R8_LINES:
            r8_render_indexed_lines(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;
        case R8_LINE_STRIP:
            r8_render_indexed_line_strip(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;
        case R8_LINE_LOOP:
            r8_render_indexed_line_loop(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;

        case R8_TRIANGLES:
            r8_render_indexed_triangles(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;
        case R8_TRIANGLE_STRIP:
            r8_render_indexed_triangle_strip(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;
        case R8_TRIANGLE_FAN:
            r8_render_indexed_triangle_fan(numVertices, firstVertex, R8_STATE_MACHINE.boundVertexBuffer, R8_STATE_MACHINE.boundIndexBuffer);
            break;

        default:
            R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
            break;
    }
}

// --- immediate mode --- //

void r8Begin(R8enum priitives)
{
    r8_immediate_mode_begin(priitives);
}

void r8End()
{
    r8_immediate_mode_end();
}

void r8TexCoord2f(R8float u, R8float v)
{
    r8_immediate_mode_texcoord(u, v);
}

void r8TexCoord2i(R8int u, R8int v)
{
    r8_immediate_mode_texcoord((R8float)u, (R8float)v);
}

void r8Vertex4f(R8float x, R8float y, R8float z, R8float w)
{
    r8_immediate_mode_vertex(x, y, z, w);
}

void r8Vertex4i(R8int x, R8int y, R8int z, R8int w)
{
    r8_immediate_mode_vertex((R8float)x, (R8float)y, (R8float)z, (R8float)w);
}

void r8Vertex3f(R8float x, R8float y, R8float z)
{
    r8_immediate_mode_vertex(x, y, z, 1.0f);
}

void r8Vertex3i(R8int x, R8int y, R8int z)
{
    r8_immediate_mode_vertex((R8float)x, (R8float)y, (R8float)z, 1.0f);
}

void r8Vertex2f(R8float x, R8float y)
{
    r8_immediate_mode_vertex(x, y, 0.0f, 1.0f);
}

void r8Vertex2i(R8int x, R8int y)
{
    r8_immediate_mode_vertex((R8float)x, (R8float)y, 0.0f, 1.0f);
}

