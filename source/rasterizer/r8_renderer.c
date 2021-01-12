/*
 * r8_renderer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */


#include "r8_renderer.h"
#include "r8_state_machine.h"
#include "r8_global_state.h"
#include "r8_raster_triangle.h"
#include "r8_external_math.h"
#include "r8_matrix4.h"
#include "r8_error.h"
#include "r8_config.h"

#include <stdio.h>
#include <math.h>


// --- internals ---

#define MAX_NUM_POLYGON_VERTS 32

static R8ClipVertex _clipVertices[MAX_NUM_POLYGON_VERTS], _clipVerticesTmp[MAX_NUM_POLYGON_VERTS];
static R8RasterVertex _rasterVertices[MAX_NUM_POLYGON_VERTS], _rasterVerticesTmp[MAX_NUM_POLYGON_VERTS];
static R8int _numPolyVerts = 0;

#define _CVERT_VEC2(v) (*(R8Vector2*)(&((_clipVertices[v]).x)))

static void _vertexbuffer_transform(R8sizei numVertices, R8sizei firstVertex, R8VertexBuffer* vertexBuffer)
{
    r8_vertexbuffer_transform(
        numVertices,
        firstVertex,
        vertexBuffer,
        &(R8_STATE_MACHINE.worldViewProjectionMatrix),
        &(R8_STATE_MACHINE.viewport)
    );
}

static void _vertexbuffer_transform_all(R8VertexBuffer* vertexBuffer)
{
    r8_vertexbuffer_transform_all(
        vertexBuffer,
        &(R8_STATE_MACHINE.worldViewProjectionMatrix),
        &(R8_STATE_MACHINE.viewport)
    );
}

static void _transform_vertex(R8ClipVertex* clipVert, const R8Vertex* vert)
{
    r8_matrix_mul_float4(&(clipVert->x), &(R8_STATE_MACHINE.worldViewProjectionMatrix), &(vert->coord.x));
    clipVert->u = vert->texCoord.x;
    clipVert->v = vert->texCoord.y;
}

static void _r8oject_vertex(R8ClipVertex* vertex, const R8Viewport* viewport)
{
    // Transform coordinate into normalized device coordinates
    //vertex->z = 1.0f / vertex->w;
    R8float rhw = 1.0f / vertex->w;

    vertex->x *= rhw;
    vertex->y *= rhw;
    //vertex->z *= rhw;
    vertex->z = rhw;

    // Transform vertex to screen coordiante (+0.5 is for rounding adjustment)
    vertex->x = viewport->x + (vertex->x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->y = viewport->y + (vertex->y + 1.0f) * viewport->halfHeight + 0.5f;
    //vertex->z = viewport->minDepth + vertex->z * viewport->depthSize;

    #ifdef R8_PERSPECTIVE_CORRECTED
    // Setup inverse texture coordinates
    vertex->u *= rhw;
    vertex->v *= rhw;
    #endif
}

static void _setup_raster_vertex(R8RasterVertex* rasterVert, const R8ClipVertex* clipVert)
{
    rasterVert->x = (R8int)(clipVert->x);
    rasterVert->y = (R8int)(clipVert->y);
    rasterVert->z = clipVert->z;
    rasterVert->u = clipVert->u;
    rasterVert->v = clipVert->v;
}

// Returns R8_TRUE if the specified triangle vertices are culled.
static R8boolean _is_triangle_culled(const R8Vector2 a, const R8Vector2 b, const R8Vector2 c)
{
    if (R8_STATE_MACHINE.cullMode != R8_CULL_NONE)
    {
        const R8float vis = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);

        if (R8_STATE_MACHINE.cullMode == R8_CULL_FRONT)
        {
            if (vis > 0.0f)
                return R8_TRUE;
        }
        else
        {
            if (vis < 0.0f)
                return R8_TRUE;
        }
    }
    return R8_FALSE;
}

// --- points --- //

void r8_render_screenspace_point(R8int x, R8int y)
{
    // Validate bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    if ( x < 0 || x >= (R8int)frameBuffer->width ||
         y < 0 || y >= (R8int)frameBuffer->height )
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    #ifdef R8_ORIGIN_LEFT_TOP
    y = frameBuffer->height - y - 1;
    #endif

    // Plot screen space point
    r8_framebuffer_plot(frameBuffer, x, y, R8_STATE_MACHINE.color0);
}

void r8_render_points(R8sizei numVertices, R8sizei firstVertex, /*const */R8VertexBuffer* vertexBuffer)
{
    // Validate bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    // Validate vertex buffer
    if (vertexBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    if (firstVertex + numVertices >= vertexBuffer->numVertices)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Transform vertices
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);

    // Render points
    R8Vertex* vert;

    R8uint x, y;
    R8uint width = frameBuffer->width, height = frameBuffer->height;

    for (; firstVertex < numVertices; ++firstVertex)
    {
        vert = (vertexBuffer->vertices + firstVertex);

        x = (R8uint)(vert->ndc.x);
        #ifdef R8_ORIGIN_LEFT_TOP
        y = frameBuffer->height - (R8uint)(vert->ndc.y) - 1;
        #else
        y = (R8uint)(vert->ndc.y);
        #endif

        if (x < width && y < height)
            r8_framebuffer_plot(frameBuffer, x, y, R8_STATE_MACHINE.color0);
    }
}

void r8_render_indexed_points(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

// --- lines --- //

// This function implements the line "bresenham" algorithm.
static void _render_screenspace_line_colored(R8int x1, R8int y1, R8int x2, R8int y2)
{
    // Get bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    if ( x1 < 0 || x1 >= (R8int)frameBuffer->width ||
         x2 < 0 || x2 >= (R8int)frameBuffer->width ||
         y1 < 0 || y1 >= (R8int)frameBuffer->height ||
         y2 < 0 || y2 >= (R8int)frameBuffer->height )
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Pre-compuations
    int dx = x2 - x1;
    int dy = y2 - y1;

    int incx = R8_SIGN(dx);
    int incy = R8_SIGN(dy);

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    int pdx, pdy, ddx, ddy, es, el;

    if (dx > dy)
    {
        pdx = incx;
        pdy = 0;
        ddx = incx;
        ddy = incy;
        es  = dy;
        el  = dx;
    }
    else
    {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        es  = dx;
        el  = dy;
    }

    if (el == 0)
        return;

    int x   = x1;
    int y   = y1;
    int err = el/2;

    // Render each pixel of the line
    for (int t = 0; t < el; ++t)
    {
        // Render pixel
        r8_framebuffer_plot(R8_STATE_MACHINE.boundFrameBuffer, (R8uint)x, (R8uint)y, R8_STATE_MACHINE.color0);

        // Move to next pixel
        err -= es;
        if (err < 0)
        {
            err += el;
            x += ddx;
            y += ddy;
        }
        else
        {
            x += pdx;
            y += pdy;
        }
    }
}

// Rasterizes a textured line using the "Bresenham" algorithm
static void _rasterize_line(R8FrameBuffer* frameBuffer, const R8Texture* texture, R8ubyte mipLevel, R8uint indexA, R8uint indexB)
{
    const R8RasterVertex* vertexA = &(_rasterVertices[indexA]);
    const R8RasterVertex* vertexB = &(_rasterVertices[indexB]);

    // Select MIP level
    R8texsize mipWidth = 0, mipHeight = 0;
    const R8ColorBuffer* texels = r8_texture_select_miplevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Pre-computations
    int dx = vertexB->x - vertexA->x;
    int dy = vertexB->y - vertexA->y;

    int incx = R8_SIGN(dx);
    int incy = R8_SIGN(dy);

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    int pdx, pdy, ddx, ddy, es, el;

    if (dx > dy)
    {
        pdx = incx;
        pdy = 0;
        ddx = incx;
        ddy = incy;
        es  = dy;
        el  = dx;
    }
    else
    {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        es  = dx;
        el  = dy;
    }

    if (el == 0)
        return;

    int x = vertexA->x;
    int y = vertexA->y;
    R8interp u = vertexA->u;
    R8interp v = vertexA->v;

    R8interp uStep = R8_FLOAT(0.0), vStep = R8_FLOAT(0.0);

    if (el > 1)
    {
        uStep = (vertexB->u - vertexA->u) / (el - 1);
        vStep = (vertexB->v - vertexA->v) / (el - 1);
    }

    int err = el/2;

    R8ColorBuffer colorIndex;

    // Render each pixel of the line
    for (R8int t = 0; t < el; ++t)
    {
        // Render pixel
        colorIndex = r8_texture_sample_nearest_from_mipmap(texels, mipWidth, mipHeight, (R8float)u, (R8float)v);

        r8_framebuffer_plot(frameBuffer, (R8uint)x, (R8uint)y, colorIndex);

        // Increase tex-coords
        u += uStep;
        v += vStep;

        // Move to next pixel
        err -= es;
        if (err < 0)
        {
            err += el;
            x += ddx;
            y += ddy;
        }
        else
        {
            x += pdx;
            y += pdy;
        }
    }
}

static void _render_indexed_lines_textured(
    const R8Texture* texture, R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

static void _render_indexed_lines_colored(
    R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //r8_framebuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (R8sizei i = firstVertex, n = numVertices + firstVertex; i + 1 < n; i += 2)
    {
        // Fetch indices
        R8ushort indexA = indexBuffer->indices[i];
        R8ushort indexB = indexBuffer->indices[i + 1];

        #ifdef R8_DEBUG
        if (indexA >= vertexBuffer->numVertices || indexB >= vertexBuffer->numVertices)
        {
            R8_SET_ERROR_FATAL("element in index buffer out of bounds");
            return;
        }
        #endif

        // Fetch vertices
        const R8Vertex* vertexA = (vertexBuffer->vertices + indexA);
        const R8Vertex* vertexB = (vertexBuffer->vertices + indexB);

        // Raster line
        R8int x1 = (R8int)vertexA->ndc.x;
        R8int y1 = (R8int)vertexA->ndc.y;

        R8int x2 = (R8int)vertexB->ndc.x;
        R8int y2 = (R8int)vertexB->ndc.y;

        _render_screenspace_line_colored(x1, y1, x2, y2);
    }
}

void r8_render_screenspace_line(R8int x1, R8int y1, R8int x2, R8int y2)
{
    // Get bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    #ifdef R8_ORIGIN_LEFT_TOP
    y1 = frameBuffer->height - y1 - 1;
    y2 = frameBuffer->height - y2 - 1;
    #endif

    _render_screenspace_line_colored(x1, y1, x2, y2);
}

void r8_render_lines(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    //...
}

void r8_render_line_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    //...
}

void r8_render_line_loop(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    //...
}

void r8_render_indexed_lines(R8sizei numVertices, R8sizei firstVertex, /*const */R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    if (R8_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL || indexBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > indexBuffer->numIndices)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    _vertexbuffer_transform_all(vertexBuffer);

    if (R8_STATE_MACHINE.boundTexture != NULL)
        _render_indexed_lines_textured(R8_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer, indexBuffer);
    else
        _render_indexed_lines_colored(numVertices, firstVertex, vertexBuffer, indexBuffer);
}

void r8_render_indexed_line_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

void r8_render_indexed_line_loop(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

// --- images --- //

static void _render_screenspace_image_textured(const R8Texture* texture, R8int left, R8int top, R8int right, R8int bottom)
{
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    // Clamp rectangle
    left    = R8_CLAMP(left, 0, (R8int)frameBuffer->width - 1);
    top     = R8_CLAMP(top, 0, (R8int)frameBuffer->height - 1);
    right   = R8_CLAMP(right, 0, (R8int)frameBuffer->width - 1);
    bottom  = R8_CLAMP(bottom, 0, (R8int)frameBuffer->height - 1);

    // Flip vertical
    #ifdef R8_ORIGIN_LEFT_TOP
    top = frameBuffer->height - top - 1;
    bottom = frameBuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        R8_SWAP(R8int, top, bottom);
    if (left > right)
        R8_SWAP(R8int, left, right);

    // Select MIP level
    R8texsize width = 0, height = 0;
    R8ubyte mipLevel = 0;//_r8_texture_compute_miplevel(texture, 1.0f / (R8float)(right - left), 0.0f, 0.0f, 1.0f / (R8float)(bottom - top));
    const R8ColorBuffer* texels = r8_texture_select_miplevel(texture, mipLevel, &width, &height);

    // Rasterize rectangle
    R8Pixel* pixels = frameBuffer->pixels;
    const R8uint pitch = frameBuffer->width;
    R8Pixel* scanline;

    R8float u = 0.0f;
    #ifdef R8_ORIGIN_LEFT_TOP
    R8float v = 1.0f;
    #else
    R8float v = 0.0f;
    #endif

    const R8float uStep = 1.0f / ((R8float)(right - left));
    const R8float vStep = 1.0f / ((R8float)(bottom - top));

    for (R8int y = top; y <= bottom; ++y)
    {
        scanline = pixels + (y * pitch + left);

        u = 0.0f;

        for (R8int x = left; x <= right; ++x)
        {
            R8ColorBuffer color = r8_texture_sample_nearest_from_mipmap(texels, width, height, u, v);

            #ifdef R8_BLACK_IS_ALPHA
            #   ifdef R8_COLOR_BUFFER_24BIT
            if (color.r != 0 || color.g != 0 || color.b != 0)
            {
            #   else
            if (color != 0)
            {
            #   endif
            #endif

            scanline->colorIndex = color;

            #ifdef R8_BLACK_IS_ALPHA
            }
            #endif

            ++scanline;
            u += uStep;
        }

        #ifdef R8_ORIGIN_LEFT_TOP
        v -= vStep;
        #else
        v += vStep;
        #endif
    }
}

static void _render_screenspace_image_colored(R8ColorBuffer colorIndex, R8int left, R8int top, R8int right, R8int bottom)
{
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    // Clamp rectangle
    left    = R8_CLAMP(left, 0, (R8int)frameBuffer->width - 1);
    top     = R8_CLAMP(top, 0, (R8int)frameBuffer->height - 1);
    right   = R8_CLAMP(right, 0, (R8int)frameBuffer->width - 1);
    bottom  = R8_CLAMP(bottom, 0, (R8int)frameBuffer->height - 1);

    // Flip vertical
    #ifdef R8_ORIGIN_LEFT_TOP
    top = frameBuffer->height - top - 1;
    bottom = frameBuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        R8_SWAP(R8int, top, bottom);
    if (left > right)
        R8_SWAP(R8int, left, right);

    // Rasterize rectangle
    R8Pixel* pixels = frameBuffer->pixels;
    const R8uint pitch = frameBuffer->width;
    R8Pixel* scanline;

    for (R8int y = top; y <= bottom; ++y)
    {
        scanline = pixels + (y * pitch + left);
        for (R8int x = left; x <= right; ++x)
        {
            scanline->colorIndex = colorIndex;
            ++scanline;
        }
    }
}

void r8_render_screenspace_image(R8int left, R8int top, R8int right, R8int bottom)
{
    if (R8_STATE_MACHINE.boundFrameBuffer != NULL)
    {
        if (R8_STATE_MACHINE.boundTexture != NULL)
            _render_screenspace_image_textured(R8_STATE_MACHINE.boundTexture, left, top, right, bottom);
        else
            _render_screenspace_image_colored(R8_STATE_MACHINE.color0, left, top, right, bottom);
    }
    else
        R8_ERROR(R8_ERROR_INVALID_STATE);
}

// --- triangles --- //

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'z'
static R8ClipVertex _get_zplane_vertex(R8ClipVertex a, R8ClipVertex b, R8float z)
{
    R8interp m = ((R8interp)(z - b.z)) / (a.z - b.z);
    R8ClipVertex c;

    c.x = (R8float)(m * (a.x - b.x) + b.x);
    c.y = (R8float)(m * (a.y - b.y) + b.y);
    c.z = z;
    c.w = (R8float)(m * (a.w - b.w) + b.w);

    c.u = (R8float)(m * (a.u - b.u) + b.u);
    c.v = (R8float)(m * (a.v - b.v) + b.v);

    return c;
}

// Clips the polygon at the z planes
static void _polygon_z_clipping(R8float zMin, R8float zMax)
{
    R8int x, y;

    // Clip at near clipping plane (zMin)
    R8int localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_clipVertices[x].z >= zMin && _clipVertices[y].z >= zMin)
            _clipVerticesTmp[localNumVerts++] = _clipVertices[y];

        // Leaving
        if (_clipVertices[x].z >= zMin && _clipVertices[y].z < zMin)
            _clipVerticesTmp[localNumVerts++] = _get_zplane_vertex(_clipVertices[x], _clipVertices[y], zMin);

        // Entering
        if (_clipVertices[x].z < zMin && _clipVertices[y].z >= zMin)
        {
            _clipVerticesTmp[localNumVerts++] = _get_zplane_vertex(_clipVertices[x], _clipVertices[y], zMin);
            _clipVerticesTmp[localNumVerts++] = _clipVertices[y];
        }
    }

    // Clip at far clipping plane (zMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_clipVerticesTmp[x].z <= zMax && _clipVerticesTmp[y].z <= zMax)
            _clipVertices[_numPolyVerts++] = _clipVerticesTmp[y];

        // Leaving
        if (_clipVerticesTmp[x].z <= zMax && _clipVerticesTmp[y].z > zMax)
            _clipVertices[_numPolyVerts++] = _get_zplane_vertex(_clipVerticesTmp[x], _clipVerticesTmp[y], zMax);

        // Entering
        if (_clipVerticesTmp[x].z > zMax && _clipVerticesTmp[y].z <= zMax)
        {
            _clipVertices[_numPolyVerts++] = _get_zplane_vertex(_clipVerticesTmp[x], _clipVerticesTmp[y], zMax);
            _clipVertices[_numPolyVerts++] = _clipVerticesTmp[y];
        }
    }
}

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'x'
static R8RasterVertex _get_xplane_vertex(R8RasterVertex a, R8RasterVertex b, R8int x)
{
    R8interp m = ((R8interp)(x - b.x)) / (a.x - b.x);
    R8RasterVertex c;

    c.x = x;
    c.y = (R8int)(m * (a.y - b.y) + b.y);
    c.z = m * (a.z - b.z) + b.z;

    c.u = m * (a.u - b.u) + b.u;
    c.v = m * (a.v - b.v) + b.v;

    return c;
}

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'y'
static R8RasterVertex _get_yplane_vertex(R8RasterVertex a, R8RasterVertex b, R8int y)
{
    R8interp m = ((R8interp)(y - b.y)) / (a.y - b.y);
    R8RasterVertex c;

    c.x = (R8int)(m * (a.x - b.x) + b.x);
    c.y = y;
    c.z = m * (a.z - b.z) + b.z;

    c.u = m * (a.u - b.u) + b.u;
    c.v = m * (a.v - b.v) + b.v;

    return c;
}

// Clips the polygon at the x and y planes
static void _polygon_xy_clipping(R8int xMin, R8int xMax, R8int yMin, R8int yMax)
{
    R8int x, y;

    // Clip at left clipping plane (xMin)
    R8int localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVertices[x].x >= xMin && _rasterVertices[y].x >= xMin)
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];

        // Leaving
        if (_rasterVertices[x].x >= xMin && _rasterVertices[y].x < xMin)
            _rasterVerticesTmp[localNumVerts++] = _get_xplane_vertex(_rasterVertices[x], _rasterVertices[y], xMin);

        // Entering
        if (_rasterVertices[x].x < xMin && _rasterVertices[y].x >= xMin)
        {
            _rasterVerticesTmp[localNumVerts++] = _get_xplane_vertex(_rasterVertices[x], _rasterVertices[y], xMin);
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];
        }
    }

    // Clip at right clipping plane (xMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVerticesTmp[x].x <= xMax && _rasterVerticesTmp[y].x <= xMax)
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];

        // Leaving
        if (_rasterVerticesTmp[x].x <= xMax && _rasterVerticesTmp[y].x > xMax)
            _rasterVertices[_numPolyVerts++] = _get_xplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], xMax);

        // Entering
        if (_rasterVerticesTmp[x].x > xMax && _rasterVerticesTmp[y].x <= xMax)
        {
            _rasterVertices[_numPolyVerts++] = _get_xplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], xMax);
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];
        }
    }

    // Clip at top clipping plane (yMin)
    localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVertices[x].y >= yMin && _rasterVertices[y].y >= yMin)
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];

        // Leaving
        if (_rasterVertices[x].y >= yMin && _rasterVertices[y].y < yMin)
            _rasterVerticesTmp[localNumVerts++] = _get_yplane_vertex(_rasterVertices[x], _rasterVertices[y], yMin);

        // Entering
        if (_rasterVertices[x].y < yMin && _rasterVertices[y].y >= yMin)
        {
            _rasterVerticesTmp[localNumVerts++] = _get_yplane_vertex(_rasterVertices[x], _rasterVertices[y], yMin);
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];
        }
    }

    // Clip at bottom clipping plane (yMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVerticesTmp[x].y <= yMax && _rasterVerticesTmp[y].y <= yMax)
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];

        // Leaving
        if (_rasterVerticesTmp[x].y <= yMax && _rasterVerticesTmp[y].y > yMax)
            _rasterVertices[_numPolyVerts++] = _get_yplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], yMax);

        // Entering
        if (_rasterVerticesTmp[x].y > yMax && _rasterVerticesTmp[y].y <= yMax)
        {
            _rasterVertices[_numPolyVerts++] = _get_yplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], yMax);
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];
        }
    }
}

static void _index_inc(R8int* x, R8int numVertices)
{
    ++(*x);
    if (*x >= numVertices)
        *x = 0;
}

static void _index_dec(R8int* x, R8int numVertices)
{
    --(*x);
    if (*x < 0)
        *x = numVertices - 1;
}

// Rasterizes convex polygon filled
static void _rasterize_polygon_fill(R8FrameBuffer* frameBuffer, const R8Texture* texture, R8ubyte mipLevel)
{
    // Select MIP level
    R8texsize mipWidth = 0, mipHeight = 0;
    const R8ColorBuffer* texels = r8_texture_select_miplevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Find left- and right sided polygon edges
    R8int x, y, top = 0, bottom = 0;

    for (x = 1; x < _numPolyVerts; ++x)
    {
        if (_rasterVertices[top].y > _rasterVertices[x].y)
            top = x;
        if (_rasterVertices[bottom].y < _rasterVertices[x].y)
            bottom = x;
    }

    // Setup raster scanline sides
    R8ScalineSide* leftSide = frameBuffer->scanlinesStart;
    R8ScalineSide* rightSide = frameBuffer->scanlinesEnd;

    x = y = top;
    for (_index_dec(&y, _numPolyVerts); x != bottom; x = y, _index_dec(&y, _numPolyVerts))
        r8_framebuffer_setup_scanlines(frameBuffer, leftSide, _rasterVertices[x], _rasterVertices[y]);

    x = y = top;
    for (_index_inc(&y, _numPolyVerts); x != bottom; x = y, _index_inc(&y, _numPolyVerts))
        r8_framebuffer_setup_scanlines(frameBuffer, rightSide, _rasterVertices[x], _rasterVertices[y]);

    // Check if sides must be swaped
    long midIndex = (_rasterVertices[bottom].y + _rasterVertices[top].y) / 2;
    if (frameBuffer->scanlinesStart[midIndex].offset > frameBuffer->scanlinesEnd[midIndex].offset)
        R8_SWAP(R8ScalineSide*, leftSide, rightSide);

    // Start rasterizing the polygon
    R8int len, offset;
    R8interp z, zAct, zStep;
    R8interp u, uAct, uStep;
    R8interp v, vAct, vStep;

    R8int yStart = _rasterVertices[top].y;
    R8int yEnd = _rasterVertices[bottom].y;

    R8Pixel* pixel;

    // Rasterize each scanline
    for (y = yStart; y <= yEnd; ++y)
    {
        len = rightSide[y].offset - leftSide[y].offset;
        if (len <= 0)
            continue;

        zStep = (rightSide[y].z - leftSide[y].z) / len;
        uStep = (rightSide[y].u - leftSide[y].u) / len;
        vStep = (rightSide[y].v - leftSide[y].v) / len;

        offset = leftSide[y].offset;
        zAct = leftSide[y].z;
        uAct = leftSide[y].u;
        vAct = leftSide[y].v;

        // Rasterize current scanline
        while (len-- >= 0)
        {
            // Fetch pixel from framebuffer
            pixel = &(frameBuffer->pixels[offset]);

            // Make depth test
            R8DepthBuffer depth = r8_pixel_write_depth(zAct);

            if (depth > pixel->depth)
            {
                pixel->depth = depth;

                #ifdef R8_PERSPECTIVE_CORRECTED
                // Compute perspective corrected texture coordinates
                z = R8_FLOAT(1.0) / zAct;
                u = uAct * z;
                v = vAct * z;
                #else
                z = zAct;
                u = uAct;
                v = vAct;
                #endif

                // Sample texture
                pixel->colorIndex = r8_texture_sample_nearest_from_mipmap(texels, mipWidth, mipHeight, (R8float)u, (R8float)v);
                //pixel->colorIndex = _r8_texture_sample_nearest(texture, u, v, uStep*z, vStep*z);
                //pixel->colorIndex = (R8ubyte)(zAct * (R8float)UCHAR_MAX);
            }

            // Next pixel
            ++offset;
            zAct += zStep;
            uAct += uStep;
            vAct += vStep;
        }
    }
}

// Rasterizes convex polygon outlines
static void _rasterize_polygon_line(R8FrameBuffer* frameBuffer, const R8Texture* texture, R8ubyte mipLevel)
{
    for (R8int i = 0; i + 1 < _numPolyVerts; ++i)
        _rasterize_line(frameBuffer, texture, mipLevel, i, i + 1);
    _rasterize_line(frameBuffer, texture, mipLevel, _numPolyVerts - 1, 0);
}

// Rasterizes convex polygon points
static void _rasterize_polygon_point(
    R8FrameBuffer* frameBuffer, const R8Texture* texture, R8ubyte mipLevel)
{
    for (R8int i = 0; i < _numPolyVerts; ++i)
    {
        r8_framebuffer_plot(
            frameBuffer,
            _rasterVertices[i].x,
            _rasterVertices[i].y,
            R8_STATE_MACHINE.color0
        );
    }
}

static void _rasterize_polygon(R8FrameBuffer* frameBuffer, const R8Texture* texture, R8ubyte mipLevel)
{
    // Rasterize polygon with selected MIP level
    switch (R8_STATE_MACHINE.polygonMode)
    {
        case R8_POLYGON_FILL:
            _rasterize_polygon_fill(frameBuffer, texture, mipLevel);
            break;
        case R8_POLYGON_LINE:
            _rasterize_polygon_line(frameBuffer, texture, mipLevel);
            break;
        case R8_POLYGON_POINT:
            _rasterize_polygon_point(frameBuffer, texture, mipLevel);
            break;
    }
}

static R8boolean _clip_and_r8oject_polygon(R8int numVertices)
{
    // Get clipping rectangle
    const R8int xMin = R8_STATE_MACHINE.clipRect.left;
    const R8int xMax = R8_STATE_MACHINE.clipRect.right;
    const R8int yMin = R8_STATE_MACHINE.clipRect.top;
    const R8int yMax = R8_STATE_MACHINE.clipRect.bottom;

    // Z clipping
    _numPolyVerts = numVertices;
    _polygon_z_clipping(1.0f, 100.0f);//!!!
    //_polygon_z_clipping(0.01f, 100.0f);//!!!

    if (_numPolyVerts < 3)
        return R8_FALSE;

    // Projection
    for (R8int j = 0; j < _numPolyVerts; ++j)
        _r8oject_vertex(&(_clipVertices[j]), &(R8_STATE_MACHINE.viewport));

    // Make culling test
    if (_is_triangle_culled(_CVERT_VEC2(0), _CVERT_VEC2(1), _CVERT_VEC2(2)))
        return R8_FALSE;

    // Setup raster vertices
    for (R8int j = 0; j < _numPolyVerts; ++j)
        _setup_raster_vertex(&(_rasterVertices[j]), &(_clipVertices[j]));

    // Edge clipping
    _polygon_xy_clipping(xMin, xMax, yMin, yMax);

    if (_numPolyVerts < 3)
        return R8_FALSE;

    return R8_TRUE;
}

static R8ubyte _compute_polygon_miplevel(const R8Texture* texture)
{
    if (R8_STATE_MACHINE.states[R8_MIP_MAPPING] != R8_FALSE && texture->mips > 0)
    {
        // Find closest vertex
        R8interp zMin = _rasterVertices[0].z;
        for (R8int i = 1; i < _numPolyVerts; ++i)
        {
            if (zMin > _rasterVertices[i].z)
                zMin = _rasterVertices[i].z;
        }

        // Derive mip level from z value
        zMin = 0.25f / zMin;
        R8int zLog = r8_int_log2((R8float)zMin);

        return R8_CLAMP(zLog, 0, texture->mips - 1);
    }
    return 0;
}

static void _render_triangles(
    const R8Texture* texture, R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    // Get clipping dimensions
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (R8sizei i = firstVertex, n = numVertices + firstVertex; i + 2 < n; i += 3)
    {
        // Fetch vertices
        const R8Vertex* vertexA = (vertexBuffer->vertices + i);
        const R8Vertex* vertexB = (vertexBuffer->vertices + (i + 1));
        const R8Vertex* vertexC = (vertexBuffer->vertices + (i + 2));

        // Setup polygon
        _transform_vertex(&(_clipVertices[0]), vertexA);
        _transform_vertex(&(_clipVertices[1]), vertexB);
        _transform_vertex(&(_clipVertices[2]), vertexC);

        if (_clip_and_r8oject_polygon(3) != R8_FALSE)
        {
            // Rasterize active polygon
            _rasterize_polygon(frameBuffer, texture, _compute_polygon_miplevel(texture));
        }
    }
}

void r8_render_triangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    if (R8_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > vertexBuffer->numVertices)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    R8Texture* texture = R8_STATE_MACHINE.boundTexture;
    if (texture == NULL || texture->texels == NULL)
    {
        r8_texture_singular_color(&R8_SINGULAR_TEXTURE, R8_STATE_MACHINE.color0);
        _render_triangles(&R8_SINGULAR_TEXTURE, numVertices, firstVertex, vertexBuffer);
    }
    else
        _render_triangles(texture, numVertices, firstVertex, vertexBuffer);
}

void r8_render_triangle_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    //...
}

void r8_render_triangle_fan(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer)
{
    //...
}

static void _render_indexed_triangles(
    const R8Texture* texture, R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    // Get clipping dimensions
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (R8sizei i = firstVertex, n = numVertices + firstVertex; i + 2 < n; i += 3)
    {
        // Fetch indices
        R8ushort indexA = indexBuffer->indices[i];
        R8ushort indexB = indexBuffer->indices[i + 1];
        R8ushort indexC = indexBuffer->indices[i + 2];

        #ifdef R8_DEBUG
        if (indexA >= vertexBuffer->numVertices || indexB >= vertexBuffer->numVertices || indexC >= vertexBuffer->numVertices)
        {
            R8_SET_ERROR_FATAL("element in index buffer out of bounds");
            return;
        }
        #endif

        // Fetch vertices
        const R8Vertex* vertexA = (vertexBuffer->vertices + indexA);
        const R8Vertex* vertexB = (vertexBuffer->vertices + indexB);
        const R8Vertex* vertexC = (vertexBuffer->vertices + indexC);

        // Setup polygon
        _transform_vertex(&(_clipVertices[0]), vertexA);
        _transform_vertex(&(_clipVertices[1]), vertexB);
        _transform_vertex(&(_clipVertices[2]), vertexC);

        if (_clip_and_r8oject_polygon(3) != R8_FALSE)
        {
            // Rasterize active polygon
            _rasterize_polygon(frameBuffer, texture, _compute_polygon_miplevel(texture));
        }
    }
}

void r8_render_indexed_triangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    if (R8_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL || indexBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > indexBuffer->numIndices)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    if (R8_STATE_MACHINE.boundTexture == NULL)
    {
        r8_texture_singular_color(&R8_SINGULAR_TEXTURE, R8_STATE_MACHINE.color0);
        _render_indexed_triangles(&R8_SINGULAR_TEXTURE, numVertices, firstVertex, vertexBuffer, indexBuffer);
    }
    else
        _render_indexed_triangles(R8_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer, indexBuffer);
}

void r8_render_indexed_triangle_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

void r8_render_indexed_triangle_fan(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    //...
}

