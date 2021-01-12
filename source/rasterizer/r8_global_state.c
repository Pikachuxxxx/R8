/*
 * r8_global_state.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_global_state.h"
#include "r8_config.h"
#include "r8_error.h"
#include "r8_renderer.h"


r8_global_state globalState_;

#define _IMM_VERTICES   globalState_.immModeVertexBuffer.vertices
#define _IMM_CUR_VERTEX _IMM_VERTICES[globalState_.immModeVertCounter]


void r8_global_state_init()
{
    r8_texture_singular_init(&(globalState_.singularTexture));

    // Initialize immediate mode
    r8_vertexbuffer_singular_init(&(globalState_.immModeVertexBuffer), R8_NUM_IMMEDIATE_VERTICES);
    globalState_.immModeActive      = R8_FALSE;
    globalState_.immModeVertCounter = 0;
    globalState_.immModePrimitives  = R8_POINTS;
}

void r8_global_state_release()
{
    r8_texture_singular_clear(&(globalState_.singularTexture));
    r8_vertexbuffer_singular_clear(&(globalState_.immModeVertexBuffer));
}

static void _immediate_mode_flush()
{
    if (globalState_.immModeVertCounter == 0)
        return;

    // Draw current vertex buffer
    switch (globalState_.immModePrimitives)
    {
        case R8_POINTS:
            r8_render_points(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;

        case R8_LINES:
            r8_render_lines(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;
        case R8_LINE_STRIP:
            r8_render_line_strip(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;
        case R8_LINE_LOOP:
            r8_render_line_loop(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;

        case R8_TRIANGLES:
            r8_render_triangles(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;
        case R8_TRIANGLE_STRIP:
            r8_render_triangle_strip(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;
        case R8_TRIANGLE_FAN:
            r8_render_triangle_fan(globalState_.immModeVertCounter, 0, &(globalState_.immModeVertexBuffer));
            break;

        default:
            R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
            break;
    }

    // Reset vertex counter
    globalState_.immModeVertCounter = 0;
}

void r8_immediate_mode_begin(R8enum priitives)
{
    if (globalState_.immModeActive)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }
    if (priitives < R8_POINTS || priitives > R8_TRIANGLE_FAN)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Store priitive type and reset vertex counter
    globalState_.immModeActive      = R8_TRUE;
    globalState_.immModePrimitives  = priitives;
    globalState_.immModeVertCounter = 0;
}

void r8_immediate_mode_end()
{
    if (!globalState_.immModeActive)
    {
        R8_ERROR(R8_ERROR_INVALID_STATE);
        return;
    }

    // Draw vertex buffer with current r8eviously selected priitive
    _immediate_mode_flush();

    globalState_.immModeActive = R8_FALSE;
}

void r8_immediate_mode_texcoord(R8float u, R8float v)
{
    // Store texture coordinate for current vertex
    _IMM_CUR_VERTEX.texCoord.x = u;
    _IMM_CUR_VERTEX.texCoord.y = v;
}

void r8_immediate_mode_vertex(R8float x, R8float y, R8float z, R8float w)
{
    // Store vertex coordinate for current vertex
    _IMM_CUR_VERTEX.coord.x = x;
    _IMM_CUR_VERTEX.coord.y = y;
    _IMM_CUR_VERTEX.coord.z = z;
    _IMM_CUR_VERTEX.coord.w = w;

    // Count to next vertex
    ++globalState_.immModeVertCounter;

    // Check if limit is exceeded
    if (globalState_.immModeVertCounter >= R8_NUM_IMMEDIATE_VERTICES)
        _immediate_mode_flush();

    switch (globalState_.immModePrimitives)
    {
        case R8_TRIANGLES:
            if ( globalState_.immModeVertCounter + 3 >= R8_NUM_IMMEDIATE_VERTICES &&
                 globalState_.immModeVertCounter % 3 == 0 )
            {
                _immediate_mode_flush();
            }
            break;
    }
}

