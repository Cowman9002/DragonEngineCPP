#pragma once

namespace dgn
{
    enum class TextureWrap
    {
        Repeat         = 0x2901,
        Mirror         = 0x8370,
        ClampToEdge    = 0x812F,
        ClampToBorder  = 0x812D
    };

    enum class TextureFilter
    {
        // no mipmaps are generated
        Nearest = 0,
        Bilinear,
        // These three mean mipmaps are generated
        Trilinear,
        NearestMip,
        BilinearMip,
    };

    enum class TextureStorage
    {
        RGB        = 0x1907,
        RGBA       = 0x1908,
        RGB16F     = 0x881B,
        RGBA16F    = 0x881A,
        RGB32F     = 0x8815,
        RGBA32F    = 0x8814,
        SRGB       = 0x8C40,
        SRGBA      = 0x8C42,
        Depth      = 0x1902,
    };

    enum class TextureData
    {
        Ubyte = 0x1401,
        Float = 0x1406
    };
}
