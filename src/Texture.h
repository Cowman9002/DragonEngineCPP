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
        unsigned m_width, m_height;

        static float max_anisotrophic;
    public:
        Texture();
        void dispose();

        Texture& createFromData(const void *data, TextureData data_type, unsigned width, unsigned height,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage);

        Texture& loadFromFile(std::string filepath, TextureWrap wrap, TextureFilter filter,
                               TextureStorage internal_storage);

        Texture& setBorderColor(float r, float g, float b, float a);

        unsigned getNativeTexture();

        unsigned getWidth();
        unsigned getHeight();

        static void setWrap(TextureWrap wrap);
        static void setFilter(TextureFilter filter);
    };

}
