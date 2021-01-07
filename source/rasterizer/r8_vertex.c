/* r8_pixel.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_vertex.h"


R8void r8InitVertex(R8Vertex vertex)
{
    if (vertex != NULL)
        memset(vertex, 0, sizeof(R8Vertex));
}
