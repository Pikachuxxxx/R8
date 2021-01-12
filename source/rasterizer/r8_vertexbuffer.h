/*
 * r8_vertexbuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_VERTEXBUFFER_H
#define R8_VERTEXBUFFER_H


#include "r8_vertex.h"
#include "r8_viewport.h"
#include "r8_structs.h"

#include <stdio.h>


typedef struct R8VertexBuffer
{
    R8sizei     numVertices;
    R8Vertex*  vertices;
}
R8VertexBuffer;


R8VertexBuffer* r8_vertexbuffer_create();
void r8_vertexbuffer_delete(R8VertexBuffer* vertexBuffer);

void r8_vertexbuffer_singular_init(R8VertexBuffer* vertexBuffer, R8sizei numVertices);
void r8_vertexbuffer_singular_clear(R8VertexBuffer* vertexBuffer);

void r8_vertexbuffer_transform(
    R8sizei numVertices,
    R8sizei firstVertex,
    R8VertexBuffer* vertexBuffer,
    const R8Matrix4* worldViewProjectionMatrix,
    const R8Viewport* viewport
);

void r8_vertexbuffer_transform_all(
    R8VertexBuffer* vertexBuffer,
    const R8Matrix4* worldViewProjectionMatrix,
    const R8Viewport* viewport
);

void r8_vertexbuffer_data(R8VertexBuffer* vertexBuffer, R8sizei numVertices, const R8void* coords, const R8void* texCoords, R8sizei vertexStride);
void r8_vertexbuffer_data_from_file(R8VertexBuffer* vertexBuffer, R8sizei* numVertices, FILE* file);


#endif
