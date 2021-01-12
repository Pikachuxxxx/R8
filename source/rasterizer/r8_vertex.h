/*
 * r8_vertex.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_VERTEX_H
#define R8_VERTEX_H


#include "r8_vector2.h"
#include "r8_vector3.h"
#include "r8_vector4.h"
#include "r8_matrix4.h"
#include "r8_config.h"


// Simple vertex with 3D coordinate and 2D texture-coordinate.
typedef struct R8Vertex
{
    // Before vertex processing
    R8Vector4 coord;       // Original coordinate.
    R8Vector2 texCoord;    // Texture-coordinate.

    #if 1// remove!
    R8Vector4 ndc;         // Normalized device coordinate.
    #ifdef R8_PERSPECTIVE_CORRECTED
    R8Vector2 invTexCoord; // Inverse texture-coordinates.
    #endif
    #endif
}
R8Vertex;


void r8_vertex_init(R8Vertex* vertex);


#endif
