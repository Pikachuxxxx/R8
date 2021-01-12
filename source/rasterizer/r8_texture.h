/*
 * r8_texture.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_TEXTURE_H
#define R8_TEXTURE_H


#include "r8_types.h"
#include "r8_macros.h"
#include "r8_vector2.h"
#include "r8_color.h"


// Maximal 11 MIP-maps restricts the textures to have a
// maximum size of (2^(11-1) = 1024) in width and height.
#define R8_MAX_NUM_MIPS             11
#define R8_MAX_TEX_SIZE             1024

#define R8_MIP_SIZE(size, mip)      ((size) >> (mip))
#define R8_TEXTURE_HAS_MIPS(tex)    ((tex)->mips > 1)


/// Textures can have a maximum size of 256x256 texels.
/// Textures store all their mip maps in a single texel array for compact memory access.
typedef struct R8Texture
{
    R8texsize           width;                      /// Width of the first MIP level.
    R8texsize           height;                     /// Height of the first MIP level.
    R8ubyte             mips;                       /// Number of MIP levels.
    R8ColorBuffer*       texels;                     /// Texel MIP chain.
    const R8ColorBuffer* mipTexels[R8_MAX_NUM_MIPS]; ///< Texel offsets for the MIP chain (Use a static array for better cache locality).
}
R8Texture;


R8Texture* r8_texture_create();
void r8_texture_delete(R8Texture* texture);

void r8_texture_singular_init(R8Texture* texture);
void r8_texture_singular_clear(R8Texture* texture);

/// Sets the single color to the specified texture. No null pointer assertion!
R8_INLINE void r8_texture_singular_color(R8Texture* texture, R8ColorBuffer colorIndex)
{
    texture->texels[0] = colorIndex;
}

/// Sets the 2D image data to the specified texture.
R8boolean r8_texture_image2d(
    R8Texture* texture,
    R8texsize width, R8texsize height,
    R8enum format, const R8void* data, R8boolean dither, R8boolean generateMips
);

/// Fill image data for specified MIP level
R8boolean r8_texture_subimage2d(
    R8Texture* texture,
    R8ubyte mip, R8texsize x, R8texsize y,
    R8texsize width, R8texsize height,
    R8enum format, const R8void* data, R8boolean dither
);

/// Returns the number of MIP levels for the specified maximal texture dimension (width or height).
R8ubyte r8_texture_num_mips(R8ubyte maxSize);

/// Returns a pointer to the specified texture MIP level.
const R8ColorBuffer* r8_texture_select_miplevel(const R8Texture* texture, R8ubyte mip, R8texsize* width, R8texsize* height);

/// Returns the MIP level index for the specified texture.
//R8ubyte _r8_texture_compute_miplevel(const r8_texture* texture, R8float r1x, R8float r1y, R8float r2x, R8float r2y);

/// Samples the nearest texel from the specified MIP-map level.
R8ColorBuffer r8_texture_sample_nearest_from_mipmap(const R8ColorBuffer* mipTexels, R8texsize mipWidth, R8texsize mipHeight, R8float u, R8float v);

/// Samples the nearest texel from the specified texture. MIP-map selection is compuited by tex-coord derivations ddx and ddy.
R8ColorBuffer r8_texture_sample_nearest(const R8Texture* texture, R8float u, R8float v, R8float ddx, R8float ddy);

/// Returns a parameter of the specified texture MIP-map level.
R8int r8_texture_get_mip_parameter(const R8Texture* texture, R8ubyte mip, R8enum param);


#endif
