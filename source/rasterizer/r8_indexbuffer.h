/*
 * r8_indexbuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_INDEXBUFFER_H
#define R8_INDEXBUFFER_H


#include "r8_types.h"

#include <stdio.h>


typedef struct R8IndexBuffer
{
    R8ushort    numIndices;
    R8ushort*   indices;
}
R8IndexBuffer;


R8IndexBuffer* r8_indexbuffer_create();
void r8_indexbuffer_delete(R8IndexBuffer* indexBuffer);

void r8_indexbuffer_data(R8IndexBuffer* indexBuffer, const R8ushort* indices, R8ushort numIndices);
void r8_indexbuffer_data_from_file(R8IndexBuffer* indexBuffer, R8sizei* numIndices, FILE* file);


#endif
