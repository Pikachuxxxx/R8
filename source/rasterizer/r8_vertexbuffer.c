/*
 * r8_vertexbuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_vertexbuffer.h"
#include "r8_state_machine.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_config.h"

#include <stdlib.h>


R8VertexBuffer* r8_vertexbuffer_create()
{
    R8VertexBuffer* vertexBuffer = R8_MALLOC(R8VertexBuffer);

    vertexBuffer->numVertices   = 0;
    vertexBuffer->vertices      = NULL;

    r8_ref_add(vertexBuffer);

    return vertexBuffer;
}

void r8_vertexbuffer_delete(R8VertexBuffer* vertexBuffer)
{
    if (vertexBuffer != NULL)
    {
        r8_ref_release(vertexBuffer);

        R8_FREE(vertexBuffer->vertices);
        R8_FREE(vertexBuffer);
    }
}

void r8_vertexbuffer_singular_init(R8VertexBuffer* vertexBuffer, R8sizei numVertices)
{
    if (vertexBuffer != NULL)
    {
        vertexBuffer->numVertices   = numVertices;
        vertexBuffer->vertices      = R8_CALLOC(R8Vertex, numVertices);
    }
}

void r8_vertexbuffer_singular_clear(R8VertexBuffer* vertexBuffer)
{
    if (vertexBuffer != NULL)
        R8_FREE(vertexBuffer->vertices);
}

//!REMOVE THIS!
static void _vertex_transform(
    R8Vertex* vertex, const R8Matrix4* worldViewProjectionMatrix, const R8Viewport* viewport)
{
    // Transform view-space coordinate into r8ojection space
    r8_matrix_mul_float4(&(vertex->ndc.x), worldViewProjectionMatrix, &(vertex->coord.x));

    // Transform coordinate into normalized device coordinates
    vertex->ndc.z = 1.0f / vertex->ndc.w;
    vertex->ndc.x *= vertex->ndc.z;
    vertex->ndc.y *= vertex->ndc.z;

    // Transform vertex to screen coordinate (+0.5 is for rounding adjustment)
    vertex->ndc.x = viewport->x + (vertex->ndc.x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->ndc.y = viewport->y + (vertex->ndc.y + 1.0f) * viewport->halfHeight + 0.5f;
    //vertex->ndc.z = viewport->minDepth + vertex->ndc.z * viewport->depthSize;

    #ifdef R8_PERSPECTIVE_CORRECTED
    // Setup inverse-texture coordinates
    vertex->invTexCoord.x = vertex->texCoord.x * vertex->ndc.z;
    vertex->invTexCoord.y = vertex->texCoord.y * vertex->ndc.z;
    #endif
}

void r8_vertexbuffer_transform(
    R8sizei numVertices, R8sizei firstVertex, R8VertexBuffer* vertexBuffer,
    const R8Matrix4* worldViewProjectionMatrix, const R8Viewport* viewport)
{
    const R8sizei lastVertex = numVertices + firstVertex;

    if (lastVertex >= vertexBuffer->numVertices)
    {
        r8_error_set(R8_ERROR_INDEX_OUT_OF_BOUNDS, __FUNCTION__);
        return;
    }

    for (R8sizei i = firstVertex; i < lastVertex; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

void r8_vertexbuffer_transform_all(
    R8VertexBuffer* vertexBuffer, const R8Matrix4* worldViewProjectionMatrix, const R8Viewport* viewport)
{
    for (R8sizei i = 0; i < vertexBuffer->numVertices; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

static void _vertexbuffer_resize(R8VertexBuffer* vertexBuffer, R8sizei numVertices)
{
    // Check if vertex buffer must be reallocated
    if (vertexBuffer->vertices == NULL || vertexBuffer->numVertices != numVertices)
    {
        // Create new vertex buffer data
        R8_FREE(vertexBuffer->vertices);

        vertexBuffer->numVertices   = numVertices;
        vertexBuffer->vertices      = R8_CALLOC(R8Vertex, numVertices);
    }
}

void r8_vertexbuffer_data(R8VertexBuffer* vertexBuffer, R8sizei numVertices, const R8void* coords, const R8void* texCoords, R8sizei vertexStride)
{
    if (vertexBuffer == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    _vertexbuffer_resize(vertexBuffer, numVertices);

    // Get offset pointers
    const R8byte* coordsByteAlign = (const R8byte*)coords;
    const R8byte* texCoordsByteAlign = (const R8byte*)texCoords;

    // Fill vertex buffer
    R8Vertex* vert = vertexBuffer->vertices;

    while (numVertices-- > 0)
    {
        // Copy coordinates
        if (coordsByteAlign != NULL)
        {
            const R8float* coord = (const R8float*)coordsByteAlign;

            vert->coord.x = coord[0];
            vert->coord.y = coord[1];
            vert->coord.z = coord[2];
            vert->coord.w = 1.0f;//coord[3];

            coordsByteAlign += vertexStride;
        }
        else
        {
            vert->coord.x = 0.0f;
            vert->coord.y = 0.0f;
            vert->coord.z = 0.0f;
            vert->coord.w = 1.0f;
        }

        // Copy texture coordinates
        if (texCoordsByteAlign != NULL)
        {
            const R8float* texCoord = (const R8float*)texCoordsByteAlign;

            vert->texCoord.x = texCoord[0];
            vert->texCoord.y = texCoord[1];

            texCoordsByteAlign += vertexStride;
        }
        else
        {
            vert->texCoord.x = 0.0f;
            vert->texCoord.y = 0.0f;
        }

        // Next vertex
        ++vert;
    }
}

void r8_vertexbuffer_data_from_file(R8VertexBuffer* vertexBuffer, R8sizei* numVertices, FILE* file)
{
    if (vertexBuffer == NULL || numVertices == NULL || file == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    // Read number of vertices
    R8ushort vertCount = 0;
    fread(&vertCount, sizeof(R8ushort), 1, file);
    *numVertices = (R8sizei)vertCount;

    _vertexbuffer_resize(vertexBuffer, *numVertices);

    // Read all vertices
    sR8_vertex data;
    R8Vertex* vert = vertexBuffer->vertices;

    for (R8ushort i = 0; i < *numVertices; ++i)
    {
        if (feof(file))
        {
            R8_ERROR(R8_ERROR_UNEXPECTED_EOF);
            return;
        }

        // Read next vertex data
        fread(&data, sizeof(sR8_vertex), 1, file);

        vert->coord.x = data.x;
        vert->coord.y = data.y;
        vert->coord.z = data.z;
        vert->coord.w = 1.0f;

        vert->texCoord.x = data.u;
        vert->texCoord.y = data.v;

        ++vert;
    }
}
