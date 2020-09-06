#pragma once

#include <string>

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

    enum class TextureType
    {
        Texture1D = 0x0DE0,
        Texture2D = 0x0DE1,
        Texture3D = 0x806F,
        TextureCube = 0x8513
    };

    enum class CubemapFace
    {
        East = 0,
        West,
        Up,
        Down,
        North,
        South
    };

    class Texture
    {
        friend class Renderer;
    private:
        unsigned m_texture;
        unsigned m_width[6], m_height[6], m_depth;
        TextureType m_type;

        static float max_anisotrophic;
    public:
        Texture();
        void dispose();

        //////////////////////////////////////////////
        //               Texture 1D                 //
        //////////////////////////////////////////////

        Texture& createAs1D(const void *data, TextureData data_type, unsigned width,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy = 0.0f);

        Texture& loadAs1D(std::string filepath, TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage, float anisotropy = 0.0f);

        //////////////////////////////////////////////
        //               Texture 2D                 //
        //////////////////////////////////////////////

        Texture& createAs2D(const void *data, TextureData data_type, unsigned width, unsigned height,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy = 0.0f);

        Texture& loadAs2D(std::string filepath, TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage, float anisotropy = -1.0f);

        //////////////////////////////////////////////
        //               Texture 3D                 //
        //////////////////////////////////////////////

        Texture& createAs3D(const void *data, TextureData data_type, unsigned width, unsigned height, unsigned depth,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy = 0.0f);

        Texture& loadAs3D(std::string filepath, unsigned depth, TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage, float anisotropy = 0.0f);

        //////////////////////////////////////////////
        //                Cubemap                   //
        //////////////////////////////////////////////

        Texture& createAsCube(const void *data[6], TextureData data_type, unsigned width[6], unsigned height[6],
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy = 0.0f);

        Texture& createAsCube(const void *data[6], TextureData data_type, unsigned width, unsigned height,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy = 0.0f);

        Texture& loadAsCube(std::string filepath[6], TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage, float anisotropy = 0.0f);

        Texture& loadAsCube(std::string filedir, TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage, float anisotropy = 0.0f);

        unsigned getNativeTexture();

        TextureType getTextureType() const;

        unsigned getWidth(CubemapFace face = CubemapFace::East) const;
        unsigned getHeight(CubemapFace face = CubemapFace::East) const;
        unsigned getDepth() const;

        // ******   IMPORTANT     **************
        // texture must be bound by renderer in slot 0 before calling any following functions

        Texture& setBorderColor(float r, float g, float b, float a);
        Texture& setWrap(TextureWrap wrap);
        Texture& setFilter(TextureFilter filter);
        Texture& generateMipmaps();
        Texture& setAnisotropy(float anisotropy);


        static CubemapFace intToFace(unsigned i);

    };

}
