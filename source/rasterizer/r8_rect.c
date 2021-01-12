/*
 * r8_rect.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_rect.h"


void r8_rect_init(R8Rect* rect)
{
    if (rect != NULL)
    {
        rect->left      = 0;
        rect->top       = 0;
        rect->right     = 0;
        rect->bottom    = 0;
    }
}

