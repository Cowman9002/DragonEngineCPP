#pragma once

namespace dgn
{
    class Texture;
    class Framebuffer
    {
        friend class Renderer;
    private:
        unsigned m_buffer;
        unsigned m_rbuffer;
        unsigned char num_color_attachments;
    public:
        Framebuffer();
        void dispose();

        Framebuffer& create();

        Framebuffer& addColorAttachment(dgn::Texture texture);
        Framebuffer& setDepthAttachment(dgn::Texture texture);
        Framebuffer& createDepthBit(unsigned width, unsigned height);

        Framebuffer& complete();
    };

}
