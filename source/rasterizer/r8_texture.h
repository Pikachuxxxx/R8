/* r8_texture.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_TEXTURE_H
#define R_8_TEXTURE_H

#include "r8_types.h"
#include "r8_macros.h"
#include "r8_color.h"
#include "r8_structs.h"

// Maximal 11 MIP-maps restricts the textures to have a
// maximum size of (2^(11-1) = 1024) in width and height
#define R8_MAX_MIP_MAPS 11
#define R8_MAX_TEX_SIZE 1024

#define R8_MIP_MAP_SIZE(size, mipmap) ((size) >> (mipmap))
#define R8_DOES_TEX_HAS_MIP_MAPS(texture) ((texture)->mips > 1)


// Textures can have a maximum size of 256x256 texels
// Textures store all their mip maps in a single texel array for compact memory access

typedef struct R8Texture
{
    R8texturesize width;
    R8texturesize height;
    R8ubyte mips;
    R8ColorBuffer* texels;
    const R8ColorBuffer* mipTexels[R8_MAX_MIP_MAPS];
}R8Texture;

R8Texture* r8TextureGenerate();
R8void r8TextureDelete(R8Texture* texture);

R8void r8TextureInit(R8Texture* texture);
R8void r8TextureClear(R8Texture* texture);

/// Sets the single color to the specified texture
inline R8void r8TextureColor(R8Texture* texture, R8ColorBuffer colorbuffer)
{
    texture->texels[0] = colorbuffer;
}

/// Sets the 2d image data to the specified texture
R8bool r8TextureImage2d(R8Texture* texture, R8texturesize width, R8texturesize height, R8enum format, const R8void* data, R8bool dither, R8bool generateMinmaps);

R8bool r8TextureSubImage2d(R8Texture* texture, R8ubyte mipmap, R8texturesize x, R8texturesize y, R8texturesize width, R8texturesize height, R8enum format, const R8void* data, R8bool dither);

R8ubyte r8TextureNumMipMaps(R8ubyte maxSize);

const R8ColorBuffer* r8TextureSelectMipmapLevel(const R8Texture* texture, R8ubyte mipmap, R8texturesize* width, R8texturesize* height);

R8ColorBuffer r8TextureSampleFromNearestMipmap(const R8ColorBuffer* mipTexels, R8texturesize mipWidth, R8texturesize mipHeight, R8float u, R8float v);

R8ColorBuffer r8TextureSampleNearestTexel(const R8Texture* texture, R8float u, R8float v, R8float ddx, R8float ddy);

R8int r8TextureGetMipmapLevel(const R8Texture* texture, R8ubyte mip, R8enum param);

#endif
