/* r8_raster_triangle.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_RASTER_TRIANGLE_H
#define R_8_RASTER_TRIANGLE_H

#include "r8_raster_vertex.h"

typedef struct R8RasterTriangle
{
    R8RasterVertex vertexA;
    R8RasterVertex vertexB;
    R8RasterVertex vertexC;
}R8RasterTriangle;

#endif
