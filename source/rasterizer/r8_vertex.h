/* r8_vertex.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_VERTEX_H
#define R_8_VERTEX_H

#include "r8_types.h"
#include "r8_structs.h"
#include "r8_config.h"

typedef struct R8Vertex
{
    R8Vec4 position;
    R8Vec2 uv;
    R8Vec4 color;

    R8Vec4 ndc; // Normalized device coordinates [-1, 1]
    #ifdef R8_PERSPECTIVE_DEPTH_CORRECTED
        R8Vec2 invTexCoord;
    #endif
}R8Vertex;

R8void r8InitVertex(R8Vertex vertex);

#endif
