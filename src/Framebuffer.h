#pragma once

#include "textureEnums.h"

namespace dgn
{
    class Texture;
    class Framebuffer
    {
        friend class Renderer;
    private:
        unsigned m_buffer;
        unsigned m_rbuffer;

        Framebuffer& setAttachment(Texture texture, TextureType type, unsigned attachment, unsigned mip, unsigned layer, CubemapFace face);

    public:
        Framebuffer();
        void dispose();

        Framebuffer& create();

        Framebuffer& setColorAttachment(Texture texture, unsigned slot, unsigned mip = 0);
        Framebuffer& setColorAttachment(Texture texture, unsigned slot, unsigned layer, unsigned mip = 0);
        Framebuffer& setColorAttachment(Texture texture, unsigned slot, CubemapFace face, unsigned mip = 0);

        Framebuffer& setDepthAttachment(Texture texture, unsigned mip = 0);
        Framebuffer& setDepthAttachment(Texture texture, unsigned layer, unsigned mip = 0);
        Framebuffer& setDepthAttachment(Texture texture, CubemapFace face, unsigned mip = 0);

        Framebuffer& createDepthBit(unsigned width, unsigned height);
        Framebuffer& removeDepthBit();

        Framebuffer& complete();
    };

}
