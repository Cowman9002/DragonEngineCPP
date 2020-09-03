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

        Framebuffer& setColorAttachment(dgn::Texture texture, unsigned slot);
        Framebuffer& setColorAttachment(dgn::Cubemap cubemap, unsigned face, unsigned slot);

        Framebuffer& setDepthAttachment(dgn::Texture texture);
        Framebuffer& createDepthBit(unsigned width, unsigned height);

        Framebuffer& complete();
    };

}
