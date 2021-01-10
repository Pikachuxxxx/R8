/* r8_renderer.c
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

/********** Internal **********/

#define R8_MAX_NUM_POLY_VERTS   32

static R8ClipVertex clipVertices_[R8_MAX_NUM_POLY_VERTS], clipVerticesTemp_[R8_MAX_NUM_POLY_VERTS];
static R8RasterVertex rasterVertices_[R8_MAX_NUM_POLY_VERTS], rasterVerticesTemp_[R8_MAX_NUM_POLY_VERTS];
static R8int numPolyVertices_ = 0;

#define R8_CLIP_VERT_VEC2(v)    (*(R8Vec2*)(&(clipVertices_[v].x)))

static R8void transform_vertexbuffer(R8sizei numVertices, R8sizei firstVertex, R8VertexBuffer* vertexbuffer)
{
    r8VertexBufferTransformVertices(numVertices, firstVertex, vertexbuffer, &(R8_STATE_MACHINE.MVPMatrix), &(R8_STATE_MACHINE.viewport));
}

static R8void transform_all_vertexbuffer(R8VertexBuffer* vertexbuffer)
{
    r8VertexBufferTransformAllVertices(vertexbuffer, &(R8_STATE_MACHINE.MVPMatrix), &(R8_STATE_MACHINE.viewport));
}

static R8void transform_vertex(R8ClipVertex* clipVert, const R8Vertex* vert)
{
    r8Mat4MultiplyFloat4(&(clipVert->x), &(R8_STATE_MACHINE.MVPMatrix), &(vert->position.x));
    clipVert->u = vert->uv.x;
    clipVert->v = vert->uv.y;
}

static R8void project_vertex(R8ClipVertex* clipVert, const R8Viewport* viewport)
{
    // Transform the coords into NDC
    R8float ndc_w = 1.0f / clipVert->w;

    clipVert->x *= ndc_w;
    clipVert->y *= ndc_w;
    clipVert->z  = ndc_w;

    // Transform vertex to screen coordinate (+0.5 is for rounding adjustment)
    clipVert->x = viewport->x + (clipVert->x + 1.0f) * viewport->halfWidth + 0.5f;
    clipVert->y = viewport->y + (clipVert->y + 1.0f) * viewport->halfHeight + 0.5f;
    //vertex->z = viewport->minDepth + vertex->z * viewport->depthSize;

    #ifdef R8_PERSPECTIVE_DEPTH_CORRECTED
        // Setup inverse texture coordinates
        clipVert->u *= ndc_w;
        clipVert->v *= ndc_w;
    #endif
}

static R8void setup_raster_vertex(R8RasterVertex* rasterVert, const R8ClipVertex* clipVert)
{
    rasterVert->x = (R8int)(clipVert->x);
    rasterVert->y = (R8int)(clipVert->y);
    rasterVert->z = clipVert->z;
    rasterVert->u = clipVert->u;
    rasterVert->v = clipVert->v;
}

/// Returns true if the specified triangle vertices are culled
static R8bool is_triangle_culled(const R8Vec2 a, const R8Vec2 b, const R8Vec2 c)
{
    if (R8_STATE_MACHINE.cullMode != R8_CULL_NONE)
    {
        const R8float vis = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

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

/********** Points **********/

R8void r8RenderScreenSpacePoint(R8int x, R8int y)
{
    // Validate bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.framebuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    if (x < 0 || x >= (R8int)frameBuffer->width ||
        y < 0 || y >= (R8int)frameBuffer->height)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    #ifdef R8_ORIGIN_TOP_LEFT
    y = frameBuffer->height - y - 1;
    #endif

    // Plot screen space point
    r8FrameBufferPlot(frameBuffer, x, y, R8_STATE_MACHINE.color0);
}

R8void r8RenderPoints(R8sizei numVertices, R8sizei firstVertex, R8VertexBuffer* vertexbuffer)
{
    // Validate bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.framebuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    // Validate vertex buffer
    if (vertexbuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    if (firstVertex + numVertices >= vertexbuffer->numVerts)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Transform vertices
    transform_vertexbuffer(numVertices, firstVertex, vertexbuffer);

    // Render points
    R8Vertex* vert;

    R8uint x, y;
    R8uint width = frameBuffer->width, height = frameBuffer->height;

    for (; firstVertex < numVertices; ++firstVertex)
    {
        vert = (vertexbuffer->vertices + firstVertex);

        x = (R8uint)(vert->ndc.x);
        #ifdef R8_ORIGIN_TOP_LEFT
            y = frameBuffer->height - (R8uint)(vert->ndc.y) - 1;
        #else
            y = (R8uint)(vert->ndc.y);
        #endif

        if (x < width && y < height)
            r8FrameBufferPlot(frameBuffer, x, y, R8_STATE_MACHINE.color0);
    }
}

R8void r8RenderIndexedPoints(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer)
{
    // TODO: To be implemented
}

/********** Lines **********/

// This function implements the line "Bresenham" algorithm
static R8void render_screenspace_line_colored(R8int x1, R8int y1, R8int x2, R8int y2)
{
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.framebuffer;

    if (x1 < 0 || x1 >= (R8int)frameBuffer->width   ||
        x2 < 0 || x2 >= (R8int)frameBuffer->width   ||
        y1 < 0 || y1 >= (R8int)frameBuffer->height  ||
        y2 < 0 || y2 >= (R8int)frameBuffer->height)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Pre-computations
    int dx = x2 - x1;
    int dy = y2 - y1;

    int incx = R8_SIGNUM(dx);
    int incy = R8_SIGNUM(dy);

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
        es = dy;
        el = dx;
    }
    else
    {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        es = dx;
        el = dy;
    }

    if (el == 0)
        return;

    int x = x1;
    int y = y1;
    int err = el / 2;

    // Render each pixel of the line
    for (int t = 0; t < el; ++t)
    {
        // Render pixel
        r8FrameBufferPlot(R8_STATE_MACHINE.framebuffer, (R8uint)x, (R8uint)y, R8_STATE_MACHINE.color0);

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
static R8void rasterize_line(R8FrameBuffer* framebuffer, const R8Texture* texture, R8ubyte mipLevel, R8int indexA, R8int indexB)
{
    const R8RasterVertex* vertexA = &(rasterVertices_[indexA]);
    const R8RasterVertex* vertexB = &(rasterVertices_[indexB]);

    // Select MIP level
    R8texturesize mipWidth = 0, mipHeight = 0;
    const R8ColorBuffer* texels = r8TextureSelectMipmapLevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Pre-computations
    int dx = vertexB->x - vertexA->x;
    int dy = vertexB->y - vertexA->y;

    int incx = R8_SIGNUM(dx);
    int incy = R8_SIGNUM(dy);

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
        es = dy;
        el = dx;
    }
    else
    {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        es = dx;
        el = dy;
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

    int err = el / 2;

    R8ColorBuffer colorIndex;

    // Render each pixel of the line
    for (R8int t = 0; t < el; ++t)
    {
        // Render pixel
        colorIndex = r8TextureSampleFromNearestMipmap(texels, mipWidth, mipHeight, (R8float)u, (R8float)v);

        r8FrameBufferPlot(framebuffer, (R8uint)x, (R8uint)y, colorIndex);

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

static R8void render_indexed_linex_textured(const R8Texture* texture, R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer)
{
    // TODO: To be implemented yet
}

static R8void render_indexed_lines_colored(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer)
{
    // Iterate over the index buffer
    for (R8sizei i = firstVertex, n = numVertices + firstVertex; i + 1 < n; i += 2)
    {
        // Fetch indices
        R8ushort indexA = indexbuffer->indices[i];
        R8ushort indexB = indexbuffer->indices[i + 1];

        #ifdef R8_DEBUG
            if (indexA >= vertexbuffer->numVerts || indexB >= vertexbuffer->numVerts)
            {
                R8_SET_FATAL_ERROR("element in index buffer out of bounds");
                return;
            }
        #endif

        // Fetch vertices
        const R8Vertex* vertexA = (vertexbuffer->vertices + indexA);
        const R8Vertex* vertexB = (vertexbuffer->vertices + indexB);

        // Raster line
        R8int x1 = (R8int)vertexA->ndc.x;
        R8int y1 = (R8int)vertexA->ndc.y;

        R8int x2 = (R8int)vertexB->ndc.x;
        R8int y2 = (R8int)vertexB->ndc.y;

        render_screenspace_line_colored(x1, y1, x2, y2);
    }
}


R8void r8RenderScreenSpaceLine(R8int x1, R8int y1, R8int x2, R8int y2)
{
    // Get bound frame buffer
    R8FrameBuffer* frameBuffer = R8_STATE_MACHINE.framebuffer;

    if (frameBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    #ifdef R8_ORIGIN_TOP_LEFT
        y1 = frameBuffer->height - y1 - 1;
        y2 = frameBuffer->height - y2 - 1;
    #endif

    render_screenspace_line_colored(x1, y1, x2, y2);
}

R8void r8RenderLines(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer)
{
    // TODO: To be implemented yet
}

R8void r8RenderLinesIndexed(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer)
{
    if (R8_STATE_MACHINE.framebuffer == NULL)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }
    if (vertexbuffer == NULL || indexbuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > indexbuffer->numIndices)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    transform_all_vertexbuffer(vertexbuffer);

    if (R8_STATE_MACHINE.texture != NULL)
        render_indexed_linex_textured(R8_STATE_MACHINE.texture, numVertices, firstVertex, vertexbuffer, indexbuffer);
    else
        render_indexed_lines_colored(numVertices, firstVertex, vertexbuffer, indexbuffer);
}

/********** Images **********/

static R8void render_screenspace_image_textured(const R8Texture* texture, R8int left, R8int top, R8int right, R8int bottom)
{
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Clamp rectangle
    left = PR_CLAMP(left, 0, (PRint)frameBuffer->width - 1);
    top = PR_CLAMP(top, 0, (PRint)frameBuffer->height - 1);
    right = PR_CLAMP(right, 0, (PRint)frameBuffer->width - 1);
    bottom = PR_CLAMP(bottom, 0, (PRint)frameBuffer->height - 1);

    // Flip vertical
    #ifdef PR_ORIGIN_LEFT_TOP
    top = frameBuffer->height - top - 1;
    bottom = frameBuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        PR_SWAP(PRint, top, bottom);
    if (left > right)
        PR_SWAP(PRint, left, right);

    // Select MIP level
    PRtexsize width = 0, height = 0;
    PRubyte mipLevel = 0;//_pr_texture_compute_miplevel(texture, 1.0f / (PRfloat)(right - left), 0.0f, 0.0f, 1.0f / (PRfloat)(bottom - top));
    const PRcolorindex* texels = _pr_texture_select_miplevel(texture, mipLevel, &width, &height);

    // Rasterize rectangle
    pr_pixel* pixels = frameBuffer->pixels;
    const PRuint pitch = frameBuffer->width;
    pr_pixel* scanline;

    PRfloat u = 0.0f;
    #ifdef PR_ORIGIN_LEFT_TOP
    PRfloat v = 1.0f;
    #else
    PRfloat v = 0.0f;
    #endif

    const PRfloat uStep = 1.0f / ((PRfloat)(right - left));
    const PRfloat vStep = 1.0f / ((PRfloat)(bottom - top));

    for (PRint y = top; y <= bottom; ++y)
    {
        scanline = pixels + (y * pitch + left);

        u = 0.0f;

        for (PRint x = left; x <= right; ++x)
        {
            PRcolorindex color = _pr_texture_sample_nearest_from_mipmap(texels, width, height, u, v);

            #ifdef PR_BLACK_IS_ALPHA
            #   ifdef PR_COLOR_BUFFER_24BIT
            if (color.r != 0 || color.g != 0 || color.b != 0)
            {
                #   else
            if (color != 0)
            {
                #   endif
                #endif

                scanline->colorIndex = color;

                #ifdef PR_BLACK_IS_ALPHA
            }
            #endif

            ++scanline;
            u += uStep;
            }

        #ifdef PR_ORIGIN_LEFT_TOP
        v -= vStep;
        #else
        v += vStep;
        #endif
        }
}

R8void r8RenderScreenSpaceImage(R8int left, R8int top, R8int right, R8int bottom)
{

}
