/* r8_raster_vertex.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_RASTER_VERTEX_H
#define R_8_RASTER_VERTEX_H

#include "r8_types.h"

/// Rasterizable vertex structure before projection for clip space
typedef struct R8ClipVertex
{
    R8float x; // X coordinate (in view space)
    R8float y; // Y coordinate (in view space)
    R8float z; // Z coordinate (in view space)
    R8float w; // W coordinate (in view space)
    R8float u; // Texture coordinate U
    R8float v; // Texture coordinate V
}R8ClipVertex;

/// Rasterizable vertex structure after projection in ndc device coordinates
typedef struct R8RasterVertex
{
    R8int       x; // Screen coordinate X
    R8int       y; // Screen coordinate Y
    R8interp    z; // Normalized device coordinate Z
    R8interp    u; // Inverse texture coordinate U
    R8interp    v; // Inverse texture coordinate V
}R8RasterVertex;
#endif