/* r8_global_state.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_global_state.h"
#include "r8_config.h"
#include "r8_error.h"
#include "r8_renderer.h"

R8GlobalState globalState_;

#define R8_IMM_VERTICES   globalState_.immModeVertexBuffer.vertices
#define R8_IMM_CUR_VERTEX R8_IMM_VERTICES[globalState_.immModeVertCounter]

R8void r8GlobalStateInit()
{
    r8TextureInit(&(globalState_.singularTexture));

    // Initialize immediate mode
    r8VertexBufferInit(&(globalState_.immModeVertexBuffer), R8_NUM_IMMEDIATE_VERTICES);
    globalState_.immModeActive = R8_FALSE;
    globalState_.immModeVertCounter = 0;
    globalState_.immModePrimitives = R8_POINTS;
}

R8void r8GlobalStateRelease()
{
    r8TextureClear(&(globalState_.singularTexture));
    r8VertexBufferClear(&(globalState_.immModeVertexBuffer));
}

static R8void immediate_mode_flush()
{
    if (globalState_.immModeVertCounter == 0)
        return;

    // Draw current vertex buffer
    switch (globalState_.immModePrimitives)
    {
    case R8_POINTS:
        r8RenderPoints(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
        break;

    case R8_LINES:
        r8RenderLines(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
        break;

    case R8_TRIANGLES:
        r8RenderTriangles(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
        break;
    case R8_TRIANGLES_STRIP:
        r8RenderTrianglesStrip(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
        break;

    default:
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        break;
    }

    // Reset vertex counter
    globalState_.immModeVertCounter = 0;
}

R8void r8ImmediateModeBegin(R8enum primitives)
{
    if (globalState_.immModeActive)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    // Store primitive type and reset vertex counter
    globalState_.immModeActive = R8_TRUE;
    globalState_.immModePrimitives = primitives;
    globalState_.immModeVertCounter = 0;
}

R8void r8ImmediateModeEnd()
{
    if (!globalState_.immModeActive)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    // Draw vertex buffer with current previously selected primitive
    immediate_mode_flush();

    globalState_.immModeActive = R8_FALSE;
}

R8void r8ImmediateModeTexCoord(R8float u, R8float v)
{
    // Store texture coordinate for current vertex
    R8_IMM_CUR_VERTEX.uv.x = u;
    R8_IMM_CUR_VERTEX.uv.y = v;
}

R8void r8ImmediateModeVertex(R8float x, R8float y, R8float z, R8float w)
{

    // Store vertex coordinate for current vertex
    R8_IMM_CUR_VERTEX.position.x = x;
    R8_IMM_CUR_VERTEX.position.y = y;
    R8_IMM_CUR_VERTEX.position.z = z;
    R8_IMM_CUR_VERTEX.position.w = w;

    // Count to next vertex
    ++globalState_.immModeVertCounter;

    // Check if limit is exceeded
    if (globalState_.immModeVertCounter >= R8_NUM_IMMEDIATE_VERTICES)
        immediate_mode_flush();

    switch (globalState_.immModePrimitives)
    {
    case R8_TRIANGLES:
        if (globalState_.immModeVertCounter + 3 >= R8_NUM_IMMEDIATE_VERTICES &&
            globalState_.immModeVertCounter % 3 == 0)
        {
            immediate_mode_flush();
        }
        break;
    }
}