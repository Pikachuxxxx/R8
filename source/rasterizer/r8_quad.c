/* r8_quad.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_quad.h"

R8void InitQuad(R8Quad* quad)
{
    if (quad != NULL)
    {
        quad->left      = 0;
        quad->top       = 0;
        quad->right     = 0;
        quad->bottom    = 0;
    }
}
