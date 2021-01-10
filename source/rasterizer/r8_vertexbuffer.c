/* r8_vertexbuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_vertexbuffer.h"
#include "r8_state_machine.h"
#include "r8_error.h"
#include "r8_config.h"
#include "r8_memory.h"

R8VertexBuffer* r8VertexBufferGenerate()
{
    R8VertexBuffer* buffer = R8_MALLOC(R8VertexBuffer);
    buffer->numVerts = 0;
    buffer->vertices = NULL;

    r8AddSMRef(buffer);

    return buffer;
}

R8void r8VertexBufferDelete(R8VertexBuffer* buffer)
{
    if (buffer != NULL)
    {
        r8ReleaseSMRef(buffer);

        R8_FREE(buffer->vertices);
        R8_FREE(buffer);
    }
}

R8void r8VertexBufferInit(R8VertexBuffer* buffer, R8sizei numVerts)
{
    if (buffer != NULL)
    {
        buffer->numVerts = numVerts;
        buffer->vertices = R8_CALLOC(R8Vertex, numVerts);
    }
}

R8void r8VertexBufferClear(R8VertexBuffer* buffer)
{
    if (buffer != NULL)
        R8_FREE(buffer->vertices);
}

static R8void r8TransformVertex(R8Vertex* vertex, const R8Mat4* MVPMatrix, const R8Viewport* viewport)
{
    // Transform view-space coordinate into projection space
    r8Mat4MultiplyVector4(&(vertex->ndc.x), MVPMatrix, &(vertex->position.x));

    // Transform coordinates into NDC
    vertex->ndc.z = 1.0f / vertex->ndc.x;
    vertex->ndc.x *= vertex->ndc.z;
    vertex->ndc.y *= vertex->ndc.z;

    // Transform vertex to screen coordinate (+0.5 is for rounding adjustment)
    vertex->ndc.x = viewport->x + (vertex->ndc.x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->ndc.y = viewport->y + (vertex->ndc.y + 1.0f) * viewport->halfHeight + 0.5f;

    #ifdef R8_PERSPECTIVE_DEPTH_CORRECTED
        // Setup inverse-texture coordinates
        vertex->invTexCoord.x = vertex->uv.x * vertex->ndc.z;
        vertex->invTexCoord.y = vertex->uv.y * vertex->ndc.z;
    #endif
}

R8void r8VertexBufferTransformVertices(R8sizei numVerts, R8sizei firstVertex, R8VertexBuffer* vertexbuffer, const R8Mat4* MVPMatrix, const R8Viewport* viewport)
{
    const R8sizei lastVertex = numVerts + firstVertex;

    if (lastVertex >= vertexbuffer->numVerts)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    for (R8sizei i = firstVertex; i < lastVertex; ++i)
    {
        r8TransformVertex((vertexbuffer->vertices + i), MVPMatrix, viewport);
    }
}

R8void r8VertexBufferTransformAllVertices(R8VertexBuffer* vertexbuffer, const R8Mat4* MVPMatrix, const R8Viewport* viewport)
{
    for (R8sizei i = 0; i < vertexbuffer->numVerts; ++i)
    {
        r8TransformVertex((vertexbuffer->vertices + i), MVPMatrix, viewport);
    }
}

static R8void r8VertexBufferResize(R8VertexBuffer* vertexbuffer, R8sizei numVerts)
{
    // Check if the vertex buffer must be re-allocated
    if (vertexbuffer->vertices == NULL || vertexbuffer->numVerts != numVerts)
    {

        R8_FREE(vertexbuffer->vertices);

        vertexbuffer->vertices = R8_CALLOC(R8Vertex, numVerts);
        vertexbuffer->numVerts = numVerts;
    }
}

R8void r8VertexBufferAddData(R8VertexBuffer* vertexbuffer, R8sizei numVerts, const R8void* pos, const R8void* uv, const R8void* color, R8sizei stride)
{
    if (vertexbuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    r8VertexBufferResize(vertexbuffer, numVerts);

    // offset pointers
    const R8byte* posByteAlign = (const R8byte*)pos;
    const R8byte* uvByteAlign = (const R8byte*)uv;
    const R8byte* colorByteAlign = (const R8byte*)color;

    // Fill vertex buffer
    R8Vertex* vert = vertexbuffer->vertices;

    while (numVerts-- > 0)
    {
        if (posByteAlign != NULL)
        {
            const R8float* coord = (const R8float*)posByteAlign;
            vert->position.x = coord[0];
            vert->position.y = coord[1];
            vert->position.z = coord[2];
            vert->position.w = 1.0f;
        }
        else
        {
            vert->position.x = 0.0f;
            vert->position.y = 0.0f;
            vert->position.z = 0.0f;
            vert->position.w = 1.0f;
        }

        if (uvByteAlign != NULL)
        {
            const R8float* texCoord = (const R8float*)uvByteAlign;
            vert->uv.x = texCoord[0];
            vert->uv.y = texCoord[1];

            uvByteAlign += stride;
        }
        else
        {
            vert->uv.x = 0.0f;
            vert->uv.y = 0.0f;
        }

        if (colorByteAlign != NULL)
        {
            const R8float* vertCol = (const R8float*)colorByteAlign;
            vert->color.x = vertCol[0];
            vert->color.y = vertCol[1];
            vert->color.z = vertCol[2];
            vert->color.w = vertCol[3];
        }
        else
        {
            vert->color.x = 1.0f;
            vert->color.y = 0.0f;
            vert->color.z = 1.0f;
            vert->color.w = 1.0f;
        }

        ++vert;
    }
}
