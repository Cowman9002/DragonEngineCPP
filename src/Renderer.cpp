#include "Renderer.h"

#include "d_internal.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace dgn
{
    bool Renderer::initialize()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            return false;
        }
        return true;
    }

    void Renderer::terminate()
    {

    }

    void Renderer::clear()
    {
        glCall(glClear(clear_flags));
    }

    /////////////////////////////////////
    //            BINDING              //
    /////////////////////////////////////

    void Renderer::bindMesh(const Mesh& mesh)
    {
        glCall(glBindVertexArray(mesh.m_vao));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_ibo));
        bound_mesh_size = mesh.m_length;
    }

    void Renderer::bindShader(const Shader& shader)
    {
        glCall(glUseProgram(shader.m_program));
    }

    void Renderer::bindTexture(const Texture& texture, unsigned slot)
    {
        glCall(glActiveTexture(GL_TEXTURE0 + slot));

        glCall(glBindTexture(GL_TEXTURE_2D, texture.m_texture));
    }

    void Renderer::bindCubemap(const Cubemap& cubemap, unsigned slot)
    {
        glCall(glActiveTexture(GL_TEXTURE0 + slot));

        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.m_texture));
    }

    void Renderer::bindFramebuffer(const Framebuffer& framebuffer)
    {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.m_buffer));
    }

    void Renderer::unbindMesh()
    {
        glCall(glBindVertexArray(0));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        bound_mesh_size = 0;
    }

    void Renderer::unbindShader()
    {
        glCall(glUseProgram(0));
    }

    void Renderer::unbindTexture(unsigned slot)
    {
        glCall(glActiveTexture(GL_TEXTURE0 + slot));

        glCall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void Renderer::unbindCubemap(unsigned slot)
    {
        glCall(glActiveTexture(GL_TEXTURE0 + slot));

        glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    }

    void Renderer::unbindFramebuffer()
    {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void Renderer::drawBoundMesh()
    {
        glCall(glDrawElements(int(draw_mode), bound_mesh_size, GL_UNSIGNED_INT, nullptr));
    }

    /////////////////////////////////////
    //            SETTERS              //
    /////////////////////////////////////

    void Renderer::setDepthTest(DepthTest func)
    {
        glCall(glDepthFunc(GL_NEVER + int(func)));
    }

    void Renderer::setClearColor(float red, float green, float blue)
    {
        glCall(glClearColor(red, green, blue, 1.0f));
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        draw_mode = mode;
    }

    void Renderer::setLineWidth(float width)
    {
        glCall(glLineWidth(width));
    }

    void Renderer::setViewport(unsigned x, unsigned y, unsigned width, unsigned height)
    {
        glCall(glViewport(x, y, width, height));
    }

    void Renderer::setCullFace(Face face)
    {
        glCall(glCullFace(GL_FRONT + int(face)));
    }

    void Renderer::setWinding(Winding winding)
    {
        glCall(glFrontFace(GL_CW + int(winding)));
    }

    void Renderer::setAlphaBlend(AlphaFactor source_factor, AlphaFactor dest_factor)
    {
        glBlendFunc(int(source_factor), int(dest_factor));
    }

    void Renderer::setClipMode(ClipMode mode)
    {
        glClipControl(GL_LOWER_LEFT, int(mode));
    }

    /////////////////////////////////
    //            FLAGS            //
    /////////////////////////////////

    void Renderer::enableClearFlag(ClearFlag flag)
    {
        clear_flags |= int(flag);
    }

    void Renderer::disableClearFlag(ClearFlag flag)
    {
        clear_flags &= ~int(flag);
    }

    void Renderer::enableFlag(RenderFlag value)
    {
        glCall(glEnable(int(value)));
    }

    void Renderer::disableFlag(RenderFlag value)
    {
        glCall(glDisable(int(value)));
    }
}
