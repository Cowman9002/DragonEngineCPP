#pragma once

namespace dgn
{
    class Texture;
    class Cubemap;
    class Framebuffer
    {
        friend class Renderer;
    private:
        unsigned m_buffer;
        unsigned m_rbuffer;
    public:
        Framebuffer();
        void dispose();

        Framebuffer& create();

        Framebuffer& setColorAttachment(dgn::Texture texture, unsigned slot, unsigned mip = 0);
        Framebuffer& setColorAttachment(dgn::Cubemap cubemap, unsigned face, unsigned slot, unsigned mip = 0);

        Framebuffer& setDepthAttachment(dgn::Texture texture);
        Framebuffer& createDepthBit(unsigned width, unsigned height);
        Framebuffer& removeDepthBit();

        Framebuffer& complete();
    };

}
