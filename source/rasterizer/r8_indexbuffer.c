/*
 * r8_indexbuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_indexbuffer.h"
#include "r8_memory.h"
#include "r8_error.h"
#include "r8_state_machine.h"

#include <stdlib.h>


R8IndexBuffer* r8_indexbuffer_create()
{
    R8IndexBuffer* indexBuffer = R8_MALLOC(R8IndexBuffer);

    indexBuffer->numIndices = 0;
    indexBuffer->indices    = NULL;

    r8_ref_add(indexBuffer);

    return indexBuffer;
}

void r8_indexbuffer_delete(R8IndexBuffer* indexBuffer)
{
    if (indexBuffer != NULL)
    {
        r8_ref_release(indexBuffer);

        R8_FREE(indexBuffer->indices);
        R8_FREE(indexBuffer);
    }
}

static void _indexbuffer_resize(R8IndexBuffer* indexBuffer, R8ushort numIndices)
{
    // Check if index buffer must be reallocated
    if (indexBuffer->indices == NULL || indexBuffer->numIndices != numIndices)
    {
        // Create new index buffer data
        R8_FREE(indexBuffer->indices);

        indexBuffer->numIndices = numIndices;
        indexBuffer->indices    = R8_CALLOC(R8ushort, numIndices);
    }
}

void r8_indexbuffer_data(R8IndexBuffer* indexBuffer, const R8ushort* indices, R8ushort numIndices)
{
    if (indexBuffer == NULL || indices == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    _indexbuffer_resize(indexBuffer, numIndices);

    // Fill index buffer
    while (numIndices-- > 0)
        indexBuffer->indices[numIndices] = indices[numIndices];
}

void r8_indexbuffer_data_from_file(R8IndexBuffer* indexBuffer, R8sizei* numIndices, FILE* file)
{
    if (indexBuffer == NULL || numIndices == NULL || file == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    // Read number of indices
    R8ushort numInd = 0;
    fread(&numInd, sizeof(R8ushort), 1, file);
    *numIndices = (R8sizei)numInd;

    _indexbuffer_resize(indexBuffer, *numIndices);

    // Read all indices
    fread(indexBuffer->indices, sizeof(R8ushort), *numIndices, file);
}

