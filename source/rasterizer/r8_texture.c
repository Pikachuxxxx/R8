/* r8_texture.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_texture.h"

 // --- internals --- //

static R8void r8TextureSubImage2d(
    R8ColorBuffer* texels, R8ubyte mip, R8texturesize width, R8texturesize height, R8enum format, const R8void* data, R8bool dither)
{
    if (format != R8_RGB)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Setup structure for sub-image
    R8Image subimage;
    subimage.width = width;
    subimage.height = height;
    subimage.format = 3;
    subimage.defFree = R8_TRUE;
    subimage.colors = (R8ubyte*)data;

    r8ImageColorToColorBuffer(texels, &subimage, dither);
}

static void r8TextureSubImage2dRect(
    R8Texture* texture, R8ubyte mip, R8texturesize x, R8texturesize y, R8texturesize width, R8texturesize height, R8enum format, const R8void* data)
{
    //...
}

static R8ubyte _color_box4_blur(R8ubyte a, R8ubyte b, R8ubyte c, R8ubyte d)
{
    R8int x;
    x = a;
    x += b;
    x += c;
    x += d;
    return (R8ubyte)(x / 4);
}

static R8ubyte _color_box2_blur(R8ubyte a, R8ubyte b)
{
    R8int x;
    x = a;
    x += b;
    return (R8ubyte)(x / 2);
}

static R8ubyte* _image_scale_down_ubyte_rgb(R8texturesize width, R8texturesize height, const R8ubyte* data)
{
    #define COLOR(x, y, i) data[((y)*width + (x))*3 + (i)]

    const R8texturesize scaledWidth = (width > 1 ? width / 2 : 1);
    const R8texturesize scaledHeight = (height > 1 ? height / 2 : 1);

    R8ubyte* scaled = R8_CALLOC(R8ubyte, scaledWidth * scaledHeight * 3);

    if (width > 1 && height > 1)
    {
        for (R8texturesize y = 0; y < scaledHeight; ++y)
        {
            for (R8texturesize x = 0; x < scaledWidth; ++x)
            {
                for (R8int i = 0; i < 3; ++i)
                {
                    scaled[(y * scaledWidth + x) * 3 + i] = _color_box4_blur(
                        COLOR(x * 2, y * 2, i),
                        COLOR(x * 2 + 1, y * 2, i),
                        COLOR(x * 2 + 1, y * 2 + 1, i),
                        COLOR(x * 2, y * 2 + 1, i)
                    );
                }
            }
        }
    }
    else if (width > 1)
    {
        for (R8texturesize x = 0; x < scaledWidth; ++x)
        {
            for (R8int i = 0; i < 3; ++i)
            {
                scaled[x * 3 + i] = _color_box2_blur(
                    COLOR(x * 2, 0, i),
                    COLOR(x * 2 + 1, 0, i)
                );
            }
        }
    }
    else if (height > 1)
    {
        for (R8texturesize y = 0; y < scaledHeight; ++y)
        {
            for (R8int i = 0; i < 3; ++i)
            {
                scaled[y * scaledWidth * 3 + i] = _color_box2_blur(
                    COLOR(0, y * 2, i),
                    COLOR(0, y * 2 + 1, i)
                );
            }
        }
    }

    return scaled;

    #undef COLOR
}

static R8ubyte* _image_scale_down(R8texturesize width, R8texturesize height, R8enum format, const R8void* data)
{
    switch (format)
    {
    case R8_RGB:
        return _image_scale_down_ubyte_rgb(width, height, (const PRubyte*)data);
    default:
        break;
    }
    return NULL;
}


R8Texture* r8TextureGenerate()
{
    // Create texture
    R8Texture* texture = R8_MALLOC(R8Texture);

    texture->width = 0;
    texture->height = 0;
    texture->mips = 0;
    texture->texels = NULL;

    for (size_t i = 0; i < R8_MAX_MIP_MAPS; ++i)
        texture->mipTexels[i] = NULL;

    return texture;
}

R8void r8TextureDelete()
{
    if (texture != NULL)
    {
        R8_FREE(texture->texels);
        R8_FREE(texture);
    }
}

R8void r8TextureInit(R8Texture* texture)
{
    if (texture != NULL)
    {
        texture->width = 1;
        texture->height = 1;
        texture->mips = 0;
        texture->texels = R8_CALLOC(R8ColorBuffer, 1);
    }
}

R8void r8TextureClear(R8Texture* texture)
{
    if (texture != NULL)
    {
        R8_FREE(texture->texels);
    }
}

R8bool r8TextureImage2d(R8Texture* texture, R8texturesize width, R8texturesize height, R8enum format, const R8void* data, R8bool dither, R8bool generateMinmaps)
{

    // Validate parameters
    if (texture == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return R8_FALSE;
    }
    if (width == 0 || height == 0)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return R8_FALSE;
    }
    if (width > R8_MAX_TEXTURE_SIZE || height > R8_MAX_TEXTURE_SIZE)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return R8_FALSE;
    }

    // Determine number of texels
    R8ubyte mips = 0;
    size_t numTexels = 0;

    if (generateMinmaps != R8_FALSE)
    {
        R8texturesize w = width;
        R8texturesize h = height;

        while (1)
        {
            // Count number of texels
            numTexels += w * h;
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
        numTexels = width * height;
    }

    // Check if texels must be reallocated
    if (texture->width != width || texture->height != height || texture->mips != mips)
    {
        // Setup new texture dimension
        texture->width = width;
        texture->height = height;
        texture->mips = mips;

        // Free previous texels
        R8_FREE(texture->texels);

        // Create texels
        texture->texels = R8_CALLOC(R8ColorBuffer, numTexels);

        // Setup MIP texel offsets
        const R8ColorBuffer* texels = texture->texels;
        R8texturesize w = width, h = height;

        for (R8ubyte mip = 0; mip < texture->mips; ++mip)
        {
            // Store current texel offset
            texture->mipTexels[mip] = texels;

            // Goto next texel MIP level
            texels += w * h;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }

    // Fill image data of first MIP level
    R8ColorBuffer* texels = texture->texels;

    r8TextureSubImage2d(texels, 0, width, height, format, data, dither);

    if (generateMinmaps != R8_FALSE)
    {
        R8void* prevData = (R8void*)data;

        // Fill image data
        for (R8ubyte mip = 1; mip < texture->mips; ++mip)
        {
            // Goto next texel MIP level
            texels += width * height;

            // Scale down image data
            data = _image_scale_down(width, height, format, prevData);

            if (mip > 1)
                R8_FREE(prevData);
            prevData = (R8void*)data;

            if (data == NULL)
            {
                R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
                return R8_FALSE;
            }

            // Halve MIP size
            if (width > 1)
                width /= 2;
            if (height > 1)
                height /= 2;

            // Fill image data for current MIP level
            r8TextureSubImage2d(texels, mip, width, height, format, data, dither);
        }

        R8_FREE(prevData);
    }

    return R8_TRUE;

}

R8bool r8TextureSubImage2d(R8Texture* texture, R8ubyte mipmap, R8texturesize x, R8texturesize y, R8texturesize width, R8texturesize height, R8enum format, const R8void* data, R8bool dither)
{
    // Validate parameters
    if (texture == NULL)
    {
        R8_ERROR(R8_ERROR_NULL_POINTER);
        return R8_FALSE;
    }
    if (texture->texels == NULL || x < 0 || y < 0 || x + width >= texture->width || y + height >= texture->height || width <= 0 || height <= 0 || mip >= texture->mips)
    {
        R8_ERROR(R8_ERROR_INVALID_ARGUMENT);
        return R8_FALSE;
    }

    // Fill image data for specified MIP level
    r8TextureSubImage2dRect(texture, mip, x, y, width, height, format, data);

    return R8_TRUE;
}

R8ubyte r8TextureNumMipMaps(R8ubyte maxSize)
{
    return maxSize > 0 ? (R8ubyte)(floorf(log2f(maxSize))) + 1 : 0;
}

const R8ColorBuffer* r8TextureSelectMipmapLevel(R8Texture* texture, R8ubyte mipmap, R8texturesize* width, R8texturesize* height)
{
    // Return texel buffer (MIP-map 0) if there are no MIP-maps
    if (texture->mips == 0)
        return texture->texels;

    // Add MIP level offset
    //mip = R8_CLAMP((R8ubyte)(((R8int)mip) + _stateMachine->textureLodBias), 0, texture->mips - 1);
    mip = R8_CLAMP((R8ubyte)(((R8int)mip) + 0), 0, texture->mips - 1);

    // Store mip size in output parameters
    *width = R8_MIP_MAP_SIZE(texture->width, mip);
    *height = R8_MIP_MAP_SIZE(texture->height, mip);

    // Return MIP-map texel offset
    return texture->mipTexels[mip];
}

R8ColorBuffer r8TextureSampleFromNearestMipmap(const R8ColorBuffer* mipTexels, R8texturesize mipWidth, R8texturesize mipHeight, R8float u, R8float v)
{
    // Clamp texture coordinates
    R8int x = (R8int)((u - (R8int)u) * mipWidth);
    R8int y = (R8int)((v - (R8int)v) * mipHeight);

    if (x < 0)
        x += mipWidth;
    if (y < 0)
        y += mipHeight;

    // Sample from texels
    return mipTexels[y * mipWidth + x];
}

R8ColorBuffer r8TextureSampleNearestTexel(const R8Texture* texture, R8float u, R8float v, R8float ddx, R8float ddy)
{
    // Select MIP-level texels by tex-coord derivation
    const R8float dx = ddx * texture->width;
    const R8float dy = ddy * texture->height;

    const R8float deltaMaxSq = R8_MAX(dx * dx, dy * dy);

    const R8int lod = r8ApproxIntLog2(deltaMaxSq);// / 2;
    const R8ubyte mip = (R8ubyte)R8_CLAMP(lod, 0, texture->mips - 1);

    // Get texels from MIP-level
    R8texturesize w, h;
    const R8ColorBuffer* texels = r8TextureSelectMipmapLevel(texture, mip, &w, &h);

    // Sample nearest texel
    return r8TextureSampleFromNearestMipmap(texels, w, h, u, v);
}

R8int r8TextureGetMipmapLevel(const R8Texture* texture, R8ubyte mip, R8enum param)
{
    if (texture == NULL || mip >= texture->mips || param < R8_TEXTURE_WIDTH|| param > R8_TEXTURE_HEIGHT)
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


