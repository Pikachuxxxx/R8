/* r8_vertexbuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_VERTEX_BUFFER_H
#define R_8_VERTEX_BUFFER_H

#include "r8_structs.h"
#include "r8_types.h"
#include "r8_vertex.h"
#include "r8_viewport.h"

#include <stdio.h>


typedef struct R8VertexBuffer
{
    R8sizei numVerts;
    R8Vertex* vertices;
}R8VertexBuffer;

R8VertexBuffer* r8VertexBufferGenerate();
R8void r8VertexBufferDelete(R8VertexBuffer* buffer);

R8void r8VertexBufferInit(R8VertexBuffer* buffer, R8sizei numVerts);
R8void r8VertexBufferClear(R8VertexBuffer* buffer);

R8void r8VertexBufferTransformVertices(R8sizei numVerts, R8sizei firstVertex, R8VertexBuffer* vertexbuffer, const R8Mat4* MVPMatrix, const R8Viewport* viewport);
R8void r8VertexBufferTransformAllVertices(R8VertexBuffer* vertexbuffer, const R8Mat4* MVPMatrix, const R8Viewport* viewport);

R8void r8VertexBufferAddData(R8VertexBuffer* vertexbuffer, R8sizei numVerts, const R8void* pos, const R8void* uy, const R8void* color, R8sizei stride);



#endif
