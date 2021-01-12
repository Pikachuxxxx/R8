/*
 * r8_structs.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_STRUCTS_H
#define R8_STRUCTS_H


#include "r8_types.h"


/// Vertex structure. Coordinates: X, Y, Z; Texture-coordinates: U, V.
typedef struct sR8_vertex
{
    R8float x, y, z, u, v;
}
sR8_vertex;


#endif
