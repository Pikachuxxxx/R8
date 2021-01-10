/* r8_renderer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_RENDERER_H
#define R_8_RENDERER_H

#include "r8_types.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"

/********** Points **********/

R8void r8RenderScreenSpacePoint(R8int x, R8int y);

R8void r8RenderPoints(R8sizei numVertices, R8sizei firstVertex, R8VertexBuffer* vertexbuffer);

R8void r8RenderIndexedPoints(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer);

/********** Lines **********/

R8void r8RenderScreenSpaceLine(R8int x1, R8int y1, R8int x2, R8int y2);

R8void r8RenderLines(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer);

R8void r8RenderLinesIndexed(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer);

/********** Images **********/

R8void r8RenderScreenSpaceImage(R8int left, R8int top, R8int right, R8int bottom);

/********** Triangles **********/

R8void r8RenderTriangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer);

R8void r8RenderTrianglesStrip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer);

R8void r8RenderIndexedTriangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer);

R8void r8RenderIndexedTrianglesStrip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexbuffer, const R8IndexBuffer* indexbuffer);

#endif