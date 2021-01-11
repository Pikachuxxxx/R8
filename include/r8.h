/* r8.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_H
#define R_8_H

#ifdef _cplusplus
extern "C" {
#endif //  _cplusplus

#include "r8_constants.h"
#include "r8_macros.h"
#include "r8_platform.h"
#include "r8_structs.h"
#include "r8_types.h"
#include "r8_error.h"

#include <stdio.h>

/****************************************************
 *                                                  *
 *                 Initialization                   *
 *                                                  *
 ****************************************************/

/// Ignite the engine
R8bool r8Ignite();

/// Flush the Render Queue
R8bool r8Release();

/// Get the error code for the error
R8enum r8GetError();

/// Get the errors of render state or API calls
const char* r8GetString();

/// Sets the error handler callback
R8void r8ErrorHandle(R8_ERROR_HANDLER_PROC errorHandler);

/****************************************************
 *                                                  *
 *                    Context                       *
 *                                                  *
 ****************************************************/

/// Create the Context
R8object r8CreateContext(const R8ContextDesc* contextDesc, R8int width, R8int height);

/// Delete the context
R8void r8DeleteContext(R8object contextObject);

/// Make the context active
R8void r8MakeContextCurrent(R8object context);

/// Present the currently bound frame buffer in the current render context
R8void r8RenderPass(R8object context);

/****************************************************
 *                                                  *
 *                  Frame Buffer                    *
 *                                                  *
 ****************************************************/

/// Creates the Frame Buffer object 
R8object r8CreateFrameBuffer(R8int width, R8int height);

/// deletes the frame buffer object 
R8void r8DeleteFrameBuffer(R8object fbo);

/// Binds the specified frame buffer to the current render context 
R8void r8BindFrameBuffer(R8object fbo);

/// Clear the specified frame buffer and its internal flags
R8void r8ClearFrameBuffer(R8object fbo, R8int depthMaskValue, R8bit clearFlags);

/****************************************************
 *                                                  *
 *                  Vertex Buffer                   *
 *                                                  *
 ****************************************************/

/// Creates a vertex buffer to hold the vertex data
R8object r8CreateVertexBuffer();

/// Deletes the specified vertex buffer object
R8void r8DeleteVertxBuffer(R8object vbo);

/// Add data to the vertex buffer
// TODO: Use vector types instead of (void *)
R8void r8VertexBufferData(R8object vbo, const R8void* vertices, const R8void* texCoords, const R8void* color, R8sizei stride, R8sizei numVerts);

/// Binds the vertex buffer data to use the associated vertex data to render
R8void r8BindVertexBuffer(R8object vbo);

/****************************************************
 *                                                  *
 *                  Index Buffer                    *
 *                                                  *
 ****************************************************/

/// Creates a index buffer to hold the index data to draw the vertices in a indexed manner
R8object r8CreateIndexBuffer();

/// Delete the specified index buffer 
R8void r8DeleteIndexBuffer(R8object ibo);

/// Add data to the index buffer
// TODO: Use vector types instead of (void*)
R8void r8IndexBufferData(R8object ibo, const R8void* indices, R8sizei indicesCount);

/// Binds the index buffer object to use the associated indices to render the vertices
R8void r8BindIndexBuffer(R8object ibo);

/****************************************************
 *                                                  *
 *                      Texture                     *
 *                                                  *
 ****************************************************/

/// Create an object to hold the texture data
R8object r8GenerateTexture();

/// Delete the texture
R8void r8DeleteTexture(R8object texture);

/// Bind the texture to the buffer to render onto the screen
R8void r8BindTexture(R8object texture);

/// Add data to the texture 
R8void r8TexImage2D(R8object texture, R8int width, R8int height, R8enum format, const R8void* data, R8bool dither, R8bool generateMips);

/// Whether to or not generate mip maps for the given texture
R8void r8GenerateMipMaps(R8object texture);

/// Sets the texture environment parameters
R8void r8TexEnvi(R8enum param, R8int value);

/****************************************************
 *                                                  *
 *             MVP and Transformations              *
 *                                                  *
 ****************************************************/

/// Sets the global model matrix
R8void r8ModelMatrix(const R8float* matrix4x4);

/// Sets the global view matrix
R8void r8ViewMatrix(const R8float* matrix4x4);

/// Sets the global projection matrix
R8void r8ProjectionMatrix(const R8float* matrix4x4);

/// Generate a 4x4 perspective matrix
R8void r8GeneratePerpspectiveMatrix(const R8float* matrix4x4,R8float fov, R8float aspectRatio, R8float nearPlane, R8float farPlane);

/// Generate a 4x4 orthographic matrix
R8void r8GenerateOrthographicMatrix(const R8float* matrix4x4,R8float left, R8float right, R8float top, R8float bottom, R8float nearPlane, R8float farPlane);

/// Generates a translation matrix
R8void r8Translate(const R8float* matrix4x4,R8float x, R8float y, R8float z);

/// Generates a rotation matrix 
R8void r8Rotate(const R8float* matrix4x4,R8float x, R8float y, R8float z, R8float angle);

/// Generates a scaling matrix
R8void r8Scale(const R8float* matrix4x4,R8float x, R8float y, R8float z);

/// Generates the identity of the specified 4x4 left-handed matrix
R8void r8Identity(R8float* matrix4x4);

/****************************************************
 *                                                  *
 *                 Render States                    *
 *                                                  *
 ****************************************************/

/// Sets the specified state for the functionality
R8void r8SetState(R8enum cap, R8bool state);

/// Enable the specified functionality
R8void r8Enable(R8enum functionality);

/// Disable the specified functionality
R8void r8Disable(R8enum functionality);

/// Sets the culling mode 
R8void r8CullMode(R8enum mode);

/// Sets the polygon drawing mode
R8void r8PolygonDrawMode(R8enum mode);

/// Sets the depth range of the depth buffer of the currently bound frame buffer
R8void r8SetDepthRange(R8float min, R8float max);

/// Set the viewport position and dimensions
R8void r8Viewport(R8int x, R8int y, R8int width, R8int height);

/// Sets the scissoring rectangle dimensions
R8void r8ScissorRect(R8int x, R8int y, R8int width, R8int height);

/****************************************************
 *                                                  *
 *                Drawing functions                 *
 *                                                  *
 ****************************************************/

/// Set the clear color of the window's background
R8void r8ClearColor(R8ubyte r, R8ubyte g, R8ubyte b);

/// Bind the color to render
R8void r8BindColor(R8ubyte r, R8ubyte g, R8ubyte b);

/// Draw the vertices using the VBO
R8void r8Draw(R8enum primitive, R8ushort verticesCount, R8ushort vertexOffset);

/// Draw vertices using indices bound to IBO
R8void r8DrawIndexed(R8enum primitive, R8ushort indicesCount, R8ushort indexOffset);

//----------------Drawing utility functions--------------

/// Draw a point
R8void r8DrawPoint(R8float x, R8float y);

/// Draw a line 
R8void r8DrawLine(R8float x1, R8float y1, R8float x2, R8float y2);

/// Draw a texture onto the screen
R8void r8DrawTexture(R8int left, R8int top, R8int right, R8int bottom);

/****************************************************
 *                                                  *
 *              Immediate mode Drawing              *
 *                                                  *
 ****************************************************/
/// Begin immediate drawing mode
R8void r8Begin(R8enum primitive);

/// End immediate drawing mode
R8void r8End();

/// Sets the 2d coordinates for the current vertex
R8void r8Vertex2f(R8float x, R8float y);

/// Sets the 3d coordinates for the current vertex
R8void r8Vertex3f(R8float x, R8float y, R8float z);

/// Sets the 4d coordinates for the current vertex
R8void r8Vertex4f(R8float x, R8float y, R8float z, R8float w);

/// Sets the texture coordinates of the current vertex
R8void r8TexCoord2f(R8float s, R8float t);

/// Sets the color of the current vertex
R8void r8VertexColor4f(R8float r, R8float g, R8float b, R8float a);

#ifdef _cplusplus
}
#endif // _cplusplus

#endif // Header gaurd