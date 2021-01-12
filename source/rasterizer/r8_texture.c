/*
 * r8_texture.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_texture.h"
#include "r8_external_math.h"
#include "r8_error.h"
#include "r8_memory.h"
#include "r8_image.h"
#include "r8_state_machine.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>


// --- internals --- //

static void _texture_subimage2d(
    R8ColorBuffer* texels, R8ubyte mip, R8texsize width, R8texsize height, R8enum format, const R8void* data, R8boolean dither)
{
    if (format != R8_UBYTE_RGB)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    // Setup structure for sub-image
    R8Image subimage;
    subimage.width      = width;
    subimage.height     = height;
    subimage.format     = 3;
    subimage.defFree    = R8_TRUE;
    subimage.colors     = (R8ubyte*)data;

    r8_image_color_to_colorindex(texels, &subimage, dither);
}

static void _texture_subimage2d_rect(
    R8Texture* texture, R8ubyte mip, R8texsize x, R8texsize y, R8texsize width, R8texsize height, R8enum format, const R8void* data)
{
    //...
}

static R8ubyte _color_box4_blur(R8ubyte a, R8ubyte b, R8ubyte c, R8ubyte d)
{
    R8int x;
    x  = a;
    x += b;
    x += c;
    x += d;
    return (R8ubyte)(x / 4);
}

static R8ubyte _color_box2_blur(R8ubyte a, R8ubyte b)
{
    R8int x;
    x  = a;
    x += b;
    return (R8ubyte)(x / 2);
}

static R8ubyte* _image_scale_down_ubyte_rgb(R8texsize width, R8texsize height, const R8ubyte* data)
{
    #define COLOR(x, y, i) data[((y)*width + (x))*3 + (i)]

    const R8texsize scaledWidth = (width > 1 ? width/2 : 1);
    const R8texsize scaledHeight = (height > 1 ? height/2 : 1);

    R8ubyte* scaled = R8_CALLOC(R8ubyte, scaledWidth*scaledHeight*3);

    if (width > 1 && height > 1)
    {
        for (R8texsize y = 0; y < scaledHeight; ++y)
        {
            for (R8texsize x = 0; x < scaledWidth; ++x)
            {
                for (R8int i = 0; i < 3; ++i)
                {
                    scaled[(y*scaledWidth + x)*3 + i] = _color_box4_blur(
                        COLOR(x*2    , y*2    , i),
                        COLOR(x*2 + 1, y*2    , i),
                        COLOR(x*2 + 1, y*2 + 1, i),
                        COLOR(x*2    , y*2 + 1, i)
                    );
                }
            }
        }
    }
    else if (width > 1)
    {
        for (R8texsize x = 0; x < scaledWidth; ++x)
        {
            for (R8int i = 0; i < 3; ++i)
            {
                scaled[x*3 + i] = _color_box2_blur(
                    COLOR(x*2    , 0, i),
                    COLOR(x*2 + 1, 0, i)
                );
            }
        }
    }
    else if (height > 1)
    {
        for (R8texsize y = 0; y < scaledHeight; ++y)
        {
            for (R8int i = 0; i < 3; ++i)
            {
                scaled[y*scaledWidth*3 + i] = _color_box2_blur(
                    COLOR(0, y*2    , i),
                    COLOR(0, y*2 + 1, i)
                );
            }
        }
    }

    return scaled;

    #undef COLOR
}

static R8ubyte* _image_scale_down(R8texsize width, R8texsize height, R8enum format, const R8void* data)
{
    switch (format)
    {
        case R8_UBYTE_RGB:
            return _image_scale_down_ubyte_rgb(width, height, (const R8ubyte*)data);
        default:
            break;
    }
    return NULL;
}

// --- interface --- //

R8Texture* r8_texture_create()
{
    // Create texture
    R8Texture* texture = R8_MALLOC(R8Texture);

    texture->width  = 0;
    texture->height = 0;
    texture->mips   = 0;
    texture->texels = NULL;

    for (size_t i = 0; i < R8_MAX_NUM_MIPS; ++i)
        texture->mipTexels[i] = NULL;

    r8_ref_add(texture);

    return texture;
}

void r8_texture_delete(R8Texture* texture)
{
    if (texture != NULL)
    {
        r8_ref_release(texture);

        R8_FREE(texture->texels);
        R8_FREE(texture);
    }
}

void r8_texture_singular_init(R8Texture* texture)
{
    if (texture != NULL)
    {
        texture->width  = 1;
        texture->height = 1;
        texture->mips   = 0;
        texture->texels = R8_CALLOC(R8ColorBuffer, 1);
    }
}

void r8_texture_singular_clear(R8Texture* texture)
{
    if (texture != NULL)
        R8_FREE(texture->texels);
}

R8boolean r8_texture_image2d(
    R8Texture* texture, R8texsize width, R8texsize height, R8enum format, const R8void* data, R8boolean dither, R8boolean generateMips)
{
    // Validate parameters
    if (texture == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return R8_FALSE;
    }
    if (width == 0 || height == 0)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, "textures must not have a size equal to zero");
        return R8_FALSE;
    }
    if (width > R8_MAX_TEX_SIZE || height > R8_MAX_TEX_SIZE)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, "maximum texture size exceeded");
        return R8_FALSE;
    }

    // Determine number of texels
    R8ubyte mips = 0;
    size_t numTexels = 0;

    if (generateMips != R8_FALSE)
    {
        R8texsize w = width;
        R8texsize h = height;

        while (1)
        {
            // Count number of texels
            numTexels += w*h;
            ++mips;

            if (w == 1 && h == 1)
                break;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }
    else
    {
        mips = 1;
        numTexels = width*height;
    }

    // Check if texels must be reallocated
    if (texture->width != width || texture->height != height || texture->mips != mips)
    {
        // Setup new texture dimension
        texture->width  = width;
        texture->height = height;
        texture->mips   = mips;

        // Free r8evious texels
        R8_FREE(texture->texels);

        // Create texels
        texture->texels = R8_CALLOC(R8ColorBuffer, numTexels);

        // Setup MIP texel offsets
        const R8ColorBuffer* texels = texture->texels;
        R8texsize w = width, h = height;

        for (R8ubyte mip = 0; mip < texture->mips; ++mip)
        {
            // Store current texel offset
            texture->mipTexels[mip] = texels;

            // Goto next texel MIP level
            texels += w*h;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }

    // Fill image data of first MIP level
    R8ColorBuffer* texels = texture->texels;

    _texture_subimage2d(texels, 0, width, height, format, data, dither);

    if (generateMips != R8_FALSE)
    {
        R8void* r8evData = (R8void*)data;

        // Fill image data
        for (R8ubyte mip = 1; mip < texture->mips; ++mip)
        {
            // Goto next texel MIP level
            texels += width*height;

            // Scale down image data
            data = _image_scale_down(width, height, format, r8evData);

            if (mip > 1)
                R8_FREE(r8evData);
            r8evData = (R8void*)data;

            if (data == NULL)
            {
                r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
                return R8_FALSE;
            }

            // Halve MIP size
            if (width > 1)
                width /= 2;
            if (height > 1)
                height /= 2;

            // Fill image data for current MIP level
            _texture_subimage2d(texels, mip, width, height, format, data, dither);
        }

        R8_FREE(r8evData);
    }

    return R8_TRUE;
}

R8boolean r8_texture_subimage2d(
    R8Texture* texture, R8ubyte mip, R8texsize x, R8texsize y, R8texsize width, R8texsize height, R8enum format, const R8void* data, R8boolean dither)
{
    // Validate parameters
    if (texture == NULL)
    {
        r8_error_set(R8_ERROR_NULL_POINTER, __FUNCTION__);
        return R8_FALSE;
    }
    if (texture->texels == NULL || x < 0 || y < 0 || x + width >= texture->width || y + height >= texture->height || width <= 0 || height <= 0 || mip >= texture->mips)
    {
        r8_error_set(R8_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return R8_FALSE;
    }

    // Fill image data for specified MIP level
    _texture_subimage2d_rect(texture, mip, x, y, width, height, format, data);

    return R8_TRUE;
}

R8ubyte r8_texture_num_mips(R8ubyte maxSize)
{
    return maxSize > 0 ? (R8ubyte)(floorf(log2f(maxSize))) + 1 : 0;
}

const R8ColorBuffer* r8_texture_select_miplevel(const R8Texture* texture, R8ubyte mip, R8texsize* width, R8texsize* height)
{
    // Return texel buffer (MIP-map 0) if there are no MIP-maps
    if (texture->mips == 0)
        return texture->texels;

    // Add MIP level offset
    mip = R8_CLAMP((R8ubyte)(((R8int)mip) + stateMachine_->textureLodBias), 0, texture->mips - 1);

    // Store mip size in output parameters
    *width = R8_MIP_SIZE(texture->width, mip);
    *height = R8_MIP_SIZE(texture->height, mip);

    // Return MIP-map texel offset
    return texture->mipTexels[mip];
}

/*R8ubyte _r8_texture_compute_miplevel(const r8_texture* texture, R8float r1x, R8float r1y, R8float r2x, R8float r2y)
{
    // Compute derivation of u and v vectors
    r1x = fabsf(r1x);// * texture->width;
    r1y = fabsf(r1y);// * texture->height;

    r2x = fabsf(r2x);// * texture->width;
    r2y = fabsf(r2y);// * texture->height;

    // Select LOD by maximal derivation vector length (using dot r8oduct)
    R8float r1_len = sqrtf(r1x*r1x + r1y*r1y);
    R8float r2_len = sqrtf(r2x*r2x + r2y*r2y);
    R8float d = R8_MAX(r1_len, r2_len)*10.0f;

    // Clamp LOD to [0, texture->texture->mips)
    R8int lod = _int_log2(d);
    //R8int lod = (R8int)log2f(d);
    return (R8ubyte)R8_CLAMP(lod, 0, texture->mips - 1);
}*/

R8ColorBuffer r8_texture_sample_nearest_from_mipmap(const R8ColorBuffer* mipTexels, R8texsize mipWidth, R8texsize mipHeight, R8float u, R8float v)
{
    // Clamp texture coordinates
    R8int x = (R8int)((u - (R8int)u)*mipWidth);
    R8int y = (R8int)((v - (R8int)v)*mipHeight);

    if (x < 0)
        x += mipWidth;
    if (y < 0)
        y += mipHeight;

    // Sample from texels
    return mipTexels[y*mipWidth + x];
}

R8ColorBuffer r8_texture_sample_nearest(const R8Texture* texture, R8float u, R8float v, R8float ddx, R8float ddy)
{
    // Select MIP-level texels by tex-coord derivation
    const R8float dx = ddx * texture->width;
    const R8float dy = ddy * texture->height;

    const R8float deltaMaxSq = R8_MAX(dx*dx, dy*dy);

    const R8int lod = r8_int_log2(deltaMaxSq);// / 2;
    const R8ubyte mip = (R8ubyte)R8_CLAMP(lod, 0, texture->mips - 1);

    // Get texels from MIP-level
    R8texsize w, h;
    const R8ColorBuffer* texels = r8_texture_select_miplevel(texture, mip, &w, &h);

    // Sample nearest texel
    return r8_texture_sample_nearest_from_mipmap(texels, w, h, u, v);
    //return _r8_color_to_colorindex_r3g3b2(mip*20, mip*20, mip*20);
}

R8int r8_texture_get_mip_parameter(const R8Texture* texture, R8ubyte mip, R8enum param)
{
    if (texture == NULL || mip >= texture->mips || param < R8_TEXTURE_WIDTH || param > R8_TEXTURE_HEIGHT)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return 0;
    }

    switch (param)
    {
        case R8_TEXTURE_WIDTH:
            return (texture->width << mip);
        case R8_TEXTURE_HEIGHT:
            return (texture->height << mip);
    }

    return 0;
}

