#include "DragonEngine/Texture.h"
#include "d_internal.h"

#include "lodepng.h"

#include <glad/glad.h>
#include <vector>

#include <cmath>

namespace dgn
{
    float Texture::max_anisotrophic = -1;

    Texture::Texture() : m_texture(0), m_width{0}, m_height{0}, m_depth(0) {}

    void Texture::dispose()
    {
        glCall(glDeleteTextures(1, &m_texture));
    }

    //////////////////////////////////////////////
    //               Texture 1D                 //
    //////////////////////////////////////////////

    Texture& Texture::createAs1D(const void *data, TextureData data_type, unsigned width,
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage, float anisotropy)
    {
        m_type = TextureType::Texture1D;
        m_width[0] = width;
        m_depth = 0;

        glCall(glGenTextures(1, &m_texture));
        glCall(glBindTexture(GL_TEXTURE_1D, m_texture));

        setWrap(wrap);
        setFilter(filter);

        glCall(glTexImage1D(GL_TEXTURE_1D, 0, int(internal_storage), width, 0, int(given_storage), int(data_type), data));

        if(int(filter) >= int(TextureFilter::Trilinear))
        {
            generateMipmaps();
            setAnisotropy(anisotropy);
        }

        glCall(glBindTexture(GL_TEXTURE_1D, 0));

        return *this;
    }

    Texture& Texture::loadAs1D(std::string filepath, TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, float anisotropy)
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

        return createAs1D(pixels.data(), TextureData::Ubyte, pixels.size() / 4, wrap, filter, internal_storage, TextureStorage::RGBA, anisotropy);
    }

    //////////////////////////////////////////////
    //               Texture 2D                 //
    //////////////////////////////////////////////

    Texture& Texture::createAs2D(const void *data, TextureData data_type, unsigned width, unsigned height,
                                TextureWrap wrap, TextureFilter filter,
                                TextureStorage internal_storage, TextureStorage given_storage, float anisotropy)
    {
        m_type = TextureType::Texture2D;
        m_width[0] = width;
        m_height[0] = height;
        m_depth = 0;

        glCall(glGenTextures(1, &m_texture));
        glCall(glBindTexture(GL_TEXTURE_2D, m_texture));

        setWrap(wrap);
        setFilter(filter);

        glCall(glTexImage2D(GL_TEXTURE_2D, 0, int(internal_storage), width, height, 0, int(given_storage), int(data_type), data));

        if(int(filter) >= int(TextureFilter::Trilinear))
        {
            generateMipmaps();
            setAnisotropy(anisotropy);
        }

        glCall(glBindTexture(GL_TEXTURE_2D, 0));

        return *this;
    }

    Texture& Texture::loadAs2D(std::string filepath, TextureWrap wrap, TextureFilter filter,
                                   TextureStorage internal_storage, float anisotropy)
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

        return createAs2D(pixels.data(), TextureData::Ubyte, width, height, wrap, filter, internal_storage, TextureStorage::RGBA, anisotropy);
    }

    //////////////////////////////////////////////
    //               Texture 3D                 //
    //////////////////////////////////////////////

    Texture& Texture::createAs3D(const void *data, TextureData data_type, unsigned width, unsigned height, unsigned depth,
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage, float anisotropy)
    {
        m_type = TextureType::Texture3D;
        m_width[0] = width;
        m_height[0] = height;
        m_depth = depth;

        glCall(glGenTextures(1, &m_texture));
        glCall(glBindTexture(GL_TEXTURE_3D, m_texture));

        setWrap(wrap);
        setFilter(filter);

        glCall(glTexImage3D(GL_TEXTURE_3D, 0, int(internal_storage), width, height, depth, 0, int(given_storage), int(data_type), data));

        if(int(filter) >= int(TextureFilter::Trilinear))
        {
            generateMipmaps();
            setAnisotropy(anisotropy);
        }

        glCall(glBindTexture(GL_TEXTURE_3D, 0));

        return *this;
    }

    Texture& Texture::loadAs3D(std::string filepath, unsigned depth, TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, float anisotropy)
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

        return createAs3D(pixels.data(), TextureData::Ubyte, width, height / depth, depth, wrap, filter, internal_storage, TextureStorage::RGBA, anisotropy);
    }


    //////////////////////////////////////////////
    //                Cubemap                   //
    //////////////////////////////////////////////

    Texture& Texture::createAsCube(const void *data[6], TextureData data_type, unsigned width[6], unsigned height[6],
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage, float anisotropy)
    {
        m_type = TextureType::TextureCube;
        m_depth = 0;

        glCall(glGenTextures(1, &m_texture));
        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));

        setWrap(wrap);
        setFilter(filter);

        for(int i = 0; i < 6; i++)
        {
            m_width[i] = width[i];
            m_height[i] = height[i];

            if(data != nullptr)
            {
                glCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, int(internal_storage), width[i], height[i], 0, int(given_storage), int(data_type), data[i]));
            }
            else
            {
                glCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, int(internal_storage), width[i], height[i], 0, int(given_storage), int(data_type), nullptr));
            }
        }

        if(int(filter) >= int(TextureFilter::Trilinear))
        {
            generateMipmaps();
            setAnisotropy(anisotropy);
        }

        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        return *this;
    }

    Texture& Texture::createAsCube(const void *data[6], TextureData data_type, unsigned width, unsigned height,
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage, float anisotropy)
    {
        unsigned w[6];
        unsigned h[6];

        for(int i = 0; i < 6; i++)
        {
            w[i] = width;
            h[i] = height;
        }

        return createAsCube(data, data_type, w, h, wrap, filter, internal_storage, given_storage, anisotropy);
    }

    Texture& Texture::loadAsCube(std::string filepath[6], TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, float anisotropy)
    {
        std::vector<unsigned char> pixels[6];
        unsigned width[6], height[6];
        const void *data[6];

        for(int i = 0; i < 6; i++)
        {
            unsigned error = lodepng::decode(pixels[i], width[i], height[i], filepath[i]);
            if(error)
            {
                std::string s = lodepng_error_text(error);
                s += "\n\tFile" + filepath[i];
                logError("PNG LOADING", s.c_str());
                return *this;
            }

            data[i] = pixels[i].data();
        }


        return createAsCube(data, TextureData::Ubyte, width, height, wrap, filter, internal_storage, TextureStorage::RGBA, anisotropy);
    }

    Texture& Texture::loadAsCube(std::string filedir, TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, float anisotropy)
    {
        std::string filepaths[6] =
        {
            filedir + "/E.png",
            filedir + "/W.png",
            filedir + "/U.png",
            filedir + "/D.png",
            filedir + "/N.png",
            filedir + "/S.png"
        };

        return loadAsCube(filepaths, wrap, filter, internal_storage, anisotropy);
    }

    unsigned Texture::getNativeTexture()
    {
        return m_texture;
    }

    TextureType Texture::getTextureType() const
    {
        return m_type;
    }

    unsigned Texture::getWidth(CubemapFace face) const
    {
        return m_width[(unsigned)face];
    }

    unsigned Texture::getHeight(CubemapFace face) const
    {
        return m_height[(unsigned)face];
    }

    Texture& Texture::setBorderColor(float r, float g, float b, float a)
    {
        float color[] = {r, g, b, a};

        glCall(glTexParameterfv((int)m_type, GL_TEXTURE_BORDER_COLOR, color));

        return *this;
    }

    Texture& Texture::setWrap(TextureWrap wrap)
    {
        glCall(glTexParameterf((int)m_type, GL_TEXTURE_WRAP_S, int(wrap)));
        glCall(glTexParameterf((int)m_type, GL_TEXTURE_WRAP_T, int(wrap)));
        glCall(glTexParameterf((int)m_type, GL_TEXTURE_WRAP_R, int(wrap)));

        return *this;
    }

    Texture& Texture::setFilter(TextureFilter filter)
    {
        switch(filter)
        {
        case TextureFilter::Nearest:
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::Bilinear:
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::Trilinear:
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::NearestMip:
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::BilinearMip:
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
            glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        }

        return *this;
    }

    Texture& Texture::generateMipmaps()
    {
        glCall(glGenerateMipmap((int)m_type));

        return *this;
    }

    Texture& Texture::setAnisotropy(float anisotropy)
    {
        if(anisotropy == 0) return *this;

        if(Texture::max_anisotrophic < 0)
        {
            glCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Texture::max_anisotrophic));
        }

        float v = Texture::max_anisotrophic;

        if(anisotropy > 0 && anisotropy < Texture::max_anisotrophic)
        {
            v = anisotropy;
        }

        glCall(glTexParameterf((int)m_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, v));

        return *this;
    }

    CubemapFace Texture::intToFace(unsigned i)
    {
        switch(i)
        {
        default:
        case 0:
            return CubemapFace::East;
        case 1:
            return CubemapFace::West;
        case 2:
            return CubemapFace::Up;
        case 3:
            return CubemapFace::Down;
        case 4:
            return CubemapFace::North;
        case 5:
            return CubemapFace::South;
        }
    }
}
