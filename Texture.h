#pragma once

#include <string>

#include "textureEnums.h"

namespace dgn
{
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
