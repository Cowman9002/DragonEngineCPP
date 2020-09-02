#include "Texture.h"
#include "d_internal.h"

#include "lodepng.h"

#include <glad/glad.h>

namespace dgn
{
    float Texture::max_anisotrophic = -1;

    Texture::Texture() : m_texture(0), m_width(0), m_height(0) {}

    void Texture::dispose()
    {
        glCall(glDeleteTextures(1, &m_texture));
        m_height = 0;
        m_width = 0;
    }

    Texture& Texture::createFromData(const void *data, TextureData data_type, unsigned width, unsigned height,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage)
    {
        glCall(glGenTextures(1, &m_texture));

        glCall(glBindTexture(GL_TEXTURE_2D, m_texture));

        Texture::setWrap(wrap);
        Texture::setFilter(filter);

        glCall(glTexImage2D(GL_TEXTURE_2D, 0, int(internal_storage), width, height, 0, int(given_storage), int(data_type), data));

        if(int(filter) >= int(TextureFilter::Trilinear))
        {
            glCall(glGenerateMipmap(GL_TEXTURE_2D));

            if(Texture::max_anisotrophic < 0)
            {
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Texture::max_anisotrophic);
                logMessagef("Anisotropic: %f\n", Texture::max_anisotrophic);
            }

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Texture::max_anisotrophic);
        }

        glCall(glBindTexture(GL_TEXTURE_2D, 0));

        m_width = width;
        m_height = height;

        return *this;
    }

    Texture& Texture::loadFromFile(std::string filepath, TextureWrap wrap, TextureFilter filter,
                                   TextureStorage internal_storage)
    {
        std::vector<unsigned char> pixels;
        unsigned width, height;

        unsigned error = lodepng::decode(pixels, width, height, filepath);
        if(error)
        {
            std::string s = lodepng_error_text(error);
            s += "\n\tFile" + filepath;
            logError("PNG LOADING", s.c_str());
            return *this;
        }

        createFromData(pixels.data(), TextureData::Ubyte, width, height, wrap, filter, internal_storage, TextureStorage::RGBA);

        return *this;
    }

    Texture& Texture::setBorderColor(float r, float g, float b, float a)
    {
        float color[] = {r, g, b, a};

        glCall(glBindTexture(GL_TEXTURE_2D, m_texture));
        glCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color));
        glCall(glBindTexture(GL_TEXTURE_2D, 0));

        return *this;
    }

    unsigned Texture::getNativeTexture()
    {
        return m_texture;
    }

    unsigned Texture::getWidth()
    {
        return m_width;
    }

    unsigned Texture::getHeight()
    {
        return m_height;
    }

    void Texture::setWrap(TextureWrap wrap)
    {
        glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, int(wrap)));
        glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, int(wrap)));
    }

    void Texture::setFilter(TextureFilter filter)
    {
        switch(filter)
        {
        case TextureFilter::Nearest:
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::Bilinear:
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::Trilinear:
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::NearestMip:
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::BilinearMip:
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        }
    }
}
