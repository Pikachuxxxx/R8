/* r8_indexbuffer.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_indexbuffer.h"
#include "r8_memory.h"
#include "r8_error.h"
#include "r8_state_machine.h"

R8IndexBuffer* r8IndexBufferCreate()
{
    R8IndexBuffer* indexbuffer = R8_MALLOC(R8IndexBuffer);

    indexbuffer->numIndices = 0;
    indexbuffer->indices = NULL;

    // Add to state machine
    r8AddSMRef(indexbuffer);

    return indexbuffer;
}

R8void r8IndexBufferDelete(R8IndexBuffer* indexbuffer)
{
    if (indexbuffer != NULL)
    {
        // Release from state machine 
        r8ReleaseSMRef(indexbuffer);

        R8_FREE(indexbuffer->indices);
        R8_FREE(indexbuffer);
    }
}

static R8void r8IndexBufferResize(R8IndexBuffer* indexbuffer, R8ushort numIndices)
{
    // Check if the vertex buffer must be re-allocated
    if (indexbuffer->indices == NULL || indexbuffer->numIndices != numIndices)
    {

        R8_FREE(indexbuffer->indices);

        indexbuffer->indices = R8_CALLOC(R8ushort, numIndices);
        indexbuffer->numIndices = numIndices;
    }
}

R8void r8IndexBufferAddData(R8IndexBuffer* indexbuffer, const R8ushort* indices, R8ushort numIndices)
{
    if (indexbuffer == NULL || indices == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return;
    }

    r8IndexBufferResize(indexbuffer, numIndices);

    // Fill index buffer
    while (numIndices-- > 0)
        indexbuffer->indices[numIndices] = indices[numIndices];
}
