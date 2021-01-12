/*
 * r8_rect.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_RECT_H
#define R8_RECT_H


#include "r8_types.h"


typedef struct R8Rect
{
    R8int left;
    R8int top;
    R8int right;
    R8int bottom;
}
R8Rect;


void r8_rect_init(R8Rect* rect);


#endif
