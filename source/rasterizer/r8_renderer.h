/*
 * r8_renderer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_RENDERER_H
#define R8_RENDERER_H


#include "r8_types.h"
#include "r8_vertexbuffer.h"
#include "r8_indexbuffer.h"


// --- points --- //

void r8_render_screenspace_point(R8int x, R8int y);

void r8_render_points(R8sizei numVertices, R8sizei firstVertex, /*const */R8VertexBuffer* vertexBuffer);

void r8_render_indexed_points(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);

// --- lines --- //

void r8_render_screenspace_line(R8int x1, R8int y1, R8int x2, R8int y2);

void r8_render_lines(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);
void r8_render_line_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);
void r8_render_line_loop(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);

void r8_render_indexed_lines(R8sizei numVertices, R8sizei firstVertex, /*const */R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);
void r8_render_indexed_line_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);
void r8_render_indexed_line_loop(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);

// --- images --- //

void r8_render_screenspace_image(R8int left, R8int top, R8int right, R8int bottom);

// --- triangles --- //

void r8_render_triangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);
void r8_render_triangle_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);
void r8_render_triangle_fan(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer);

void r8_render_indexed_triangles(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);
void r8_render_indexed_triangle_strip(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);
void r8_render_indexed_triangle_fan(R8sizei numVertices, R8sizei firstVertex, const R8VertexBuffer* vertexBuffer, const R8IndexBuffer* indexBuffer);


#endif
