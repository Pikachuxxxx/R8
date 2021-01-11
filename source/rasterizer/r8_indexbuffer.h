/* r8_indexbuffer.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_INDEX_BUFFER_H
#define R_8_INDEX_BUFFER_H

#include "r8_types.h"

#include <stdio.h>

typedef struct R8IndexBuffer
{
    R8ushort    numIndices;
    R8ushort*   indices;
}R8IndexBuffer;


R8IndexBuffer* r8IndexBufferCreate();
R8void r8IndexBufferDelete(R8IndexBuffer* indexbuffer);

R8void r8IndexBufferAddData(R8IndexBuffer* indexbuffer, const R8ushort* indices, R8ushort numIndices);

#endif


