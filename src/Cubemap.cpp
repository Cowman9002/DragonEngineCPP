#include "Cubemap.h"
#include "d_internal.h"

#include "lodepng.h"

#include <glad/glad.h>
namespace dgn
{
    Cubemap::Cubemap() : m_texture(0) {}

    void Cubemap::dispose()
    {
        glCall(glDeleteTextures(1, &m_texture));
    }

    Cubemap& Cubemap::createFromData(const void *data[6], TextureData data_type,
            unsigned width[6], unsigned height[6],
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage)
    {
        glCall(glGenTextures(1, &m_texture));

        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));

        Cubemap::setWrap(wrap);
        Cubemap::setFilter(filter);

        for(int i = 0; i < 6; i++)
        {

            m_widths[i] = width[i];
            m_heights[i] = height[i];

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
            glCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        }

        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        return *this;
    }

    Cubemap& Cubemap::createFromData(const void *data[6], TextureData data_type, unsigned width, unsigned height,
            TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage, TextureStorage given_storage)
    {
        unsigned w[6];
        unsigned h[6];

        for(int i = 0; i < 6; i++)
        {
            w[i] = width;
            h[i] = height;
        }

        return createFromData(data, data_type, w, h, wrap, filter, internal_storage, given_storage);
    }

    Cubemap& Cubemap::loadFromFiles(std::string filepath[6], TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage)
    {

        std::vector<unsigned char> pixels[6];
        unsigned width[6], height[6];

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
        }


        const void *data[6];
        for(int i = 0; i < 6; i++)
        {
            data[i] = pixels[i].data();
        }

        return createFromData(data, TextureData::Ubyte, width, height, wrap, filter, internal_storage, TextureStorage::RGBA);
    }

    /** Individual file names must be N.png, E.png, S.png, W.png, U.png, D.png**/
    Cubemap& Cubemap::loadFromDirectory(std::string filepath, TextureWrap wrap, TextureFilter filter,
            TextureStorage internal_storage)
    {

        std::string filepaths[6] =
        {
            filepath + "E.png",
            filepath + "W.png",
            filepath + "U.png",
            filepath + "D.png",
            filepath + "N.png",
            filepath + "S.png"
        };

        return loadFromFiles(filepaths, wrap, filter, internal_storage);
    }

    unsigned Cubemap::getNativeTexture()
    {
        return m_texture;
    }

    unsigned Cubemap::getWidth(unsigned f)
    {
        if(f > 5) return 0;
        return m_widths[f];
    }

    unsigned Cubemap::getHeight(unsigned f)
    {
        if(f > 5) return 0;
        return m_heights[f];
    }

    void Cubemap::setWrap(TextureWrap wrap)
    {
        glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, int(wrap)));
        glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, int(wrap)));
        glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, int(wrap)));
    }

    void Cubemap::setFilter(TextureFilter filter)
    {
        switch(filter)
        {
        case TextureFilter::Nearest:
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::Bilinear:
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::Trilinear:
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case TextureFilter::NearestMip:
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        case TextureFilter::BilinearMip:
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
            glCall(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        }
    }

    void Cubemap::generateMipmaps()
    {
         glCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    }
}




