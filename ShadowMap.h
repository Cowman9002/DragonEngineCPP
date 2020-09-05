#pragma once

#include "Texture.h"
#include "Framebuffer.h"

#include <m3d/mat4x4.h>
#include <m3d/vec3.h>

namespace dgn
{
    class Camera;
    class ShadowMap
    {
        private:
            Texture m_texture;
            Framebuffer m_buffer;

            m3d::mat4x4 m_projection;
            m3d::mat4x4 light_view;
        public:
            ShadowMap();

            ShadowMap& initialize(unsigned width, unsigned height);
            void dispose();

            ShadowMap& updateViewMat(const m3d::vec3& dir);
            ShadowMap& updateProjectionMat(const m3d::mat4x4& mat);
            ShadowMap& updateProjectionMatFitted(Camera cam, float near, float far, float near_pull = 0.0f, float scale_value = 1.0f / 1.414314f);

            Framebuffer getFramebuffer();
            Texture getTexture();
            m3d::mat4x4 getLightMat();
    };
}
