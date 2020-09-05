#include "Framebuffer.h"
#include "d_internal.h"

#include "Texture.h"
#include "Cubemap.h"

#include <glad/glad.h>
#include <vector>

namespace dgn
{
    Framebuffer::Framebuffer() : m_buffer(0), m_rbuffer(0) {}

    void Framebuffer::dispose()
    {
        glDeleteFramebuffers(1, &m_buffer);
        glDeleteRenderbuffers(1, &m_rbuffer);
    }

    Framebuffer& Framebuffer::create()
    {
        glCall(glGenFramebuffers(1, &m_buffer));
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_buffer));

        glCall(glGenRenderbuffers(1, &m_rbuffer));
        glCall(glBindRenderbuffer(GL_RENDERBUFFER, m_rbuffer));

        return *this;
    }

    Framebuffer& Framebuffer::setAttachment(Texture texture, TextureType type, unsigned attachment, unsigned mip, unsigned layer, CubemapFace face)
    {
        switch(type)
        {
        case TextureType::Texture1D:
            {
                glCall(glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_1D, texture.getNativeTexture(), mip));
                break;
            }
        case TextureType::Texture2D:
            {
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.getNativeTexture(), mip));
                break;
            }
        case TextureType::Texture3D:
            {
                glCall(glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, texture.getNativeTexture(), mip, layer));
                break;
            }
        case TextureType::TextureCube:
            {
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (unsigned)face, texture.getNativeTexture(), mip));
                break;
            }
        }

        return *this;
    }

    Framebuffer& Framebuffer::setColorAttachment(dgn::Texture texture, unsigned slot, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_COLOR_ATTACHMENT0 + slot, mip, 0, CubemapFace::East);
    }

    Framebuffer& Framebuffer::setColorAttachment(dgn::Texture texture, unsigned slot, unsigned layer, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_COLOR_ATTACHMENT0 + slot, mip, layer, CubemapFace::East);
    }

    Framebuffer& Framebuffer::setColorAttachment(dgn::Texture texture, unsigned slot, CubemapFace face, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_COLOR_ATTACHMENT0 + slot, mip, 0, face);
    }

    Framebuffer& Framebuffer::setDepthAttachment(dgn::Texture texture, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_DEPTH_ATTACHMENT, mip, 0, CubemapFace::East);
    }

    Framebuffer& Framebuffer::setDepthAttachment(dgn::Texture texture, unsigned layer, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_DEPTH_ATTACHMENT, mip, layer, CubemapFace::East);
    }

    Framebuffer& Framebuffer::setDepthAttachment(dgn::Texture texture, CubemapFace face, unsigned mip)
    {
        return setAttachment(texture, texture.getTextureType(), GL_DEPTH_ATTACHMENT, mip, 0, face);
    }

    Framebuffer& Framebuffer::createDepthBit(unsigned width, unsigned height)
    {
        glCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height));
        glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbuffer));
        return *this;
    }

    Framebuffer& Framebuffer::removeDepthBit()
    {
        glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
        return *this;
    }

    Framebuffer& Framebuffer::complete()
    {
        glCall(unsigned status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if(status != GL_FRAMEBUFFER_COMPLETE)
        {
            switch(status)
            {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                logError("FRAMEBUFFER STATUS INCOMPLETE", "Not all framebuffer attachment points are framebuffer attachment complete.\n"
                         "This means that at least one attachment point with a renderbuffer\n"
                         "or texture attached has its attached object no longer in existence\n"
                         "or has an attached image with a width or height of zero, \n"
                         "or the color attachment point has a non-color-renderable image attached, \n"
                         "or the depth attachment point has a non-depth-renderable image attached, \n"
                         "or the stencil attachment point has a non-stencil-renderable image attached.");
                         break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                logError("FRAMEBUFFER STATUS INCOMPLETE", "Not all attached images have the same width and height");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                logError("FRAMEBUFFER STATUS INCOMPLETE", "No images are attached to the framebuffer.");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                logError("FRAMEBUFFER STATUS INCOMPLETE", "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
                break;
            default:
                logError("FRAMEBUFFER STATUS INCOMPLETE", "unknown error code");
                break;
            }
        }

        glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

        return *this;
    }
}
