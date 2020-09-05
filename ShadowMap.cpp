#include "ShadowMap.h"

#include <m3d/quat.h>
#include <m3d/vec4.h>
#include <m3d/vec2.h>
#include "Camera.h"

#include "math.h"

#include "BoundingSphere.h"

namespace dgn
{
    ShadowMap::ShadowMap() : m_texture(), m_buffer(), m_projection(1.0f), light_view(1.0f) {}

    ShadowMap& ShadowMap::initialize(unsigned width, unsigned height)
    {
        m_texture.createAs2D(nullptr, dgn::TextureData::Float, width, height, dgn::TextureWrap::ClampToBorder,
                                 dgn::TextureFilter::Nearest, dgn::TextureStorage::Depth, dgn::TextureStorage::Depth);
        m_texture.setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_buffer.create();
        m_buffer.setDepthAttachment(m_texture);
       // m_buffer.createDepthBit(width, height);
        m_buffer.complete();

        return *this;
    }

    ShadowMap& ShadowMap::updateViewMat(const m3d::vec3& dir)
    {
        m3d::quat light_quat = m3d::quat::face(dir, m3d::vec3(0.0f, 1.0f, 0.0f));
        light_view.rotate(-light_quat);

        return *this;
    }

    ShadowMap& ShadowMap::updateProjectionMat(const m3d::mat4x4& mat)
    {
        m_projection = mat;
        return *this;
    }

    ShadowMap& ShadowMap::updateProjectionMatFitted(Camera cam, float near, float far, float near_pull, float scale_value)
    {
        float ratio = cam.width / cam.height;
        float tanHalfHFOV = tanf(cam.fov * ratio / 2.0f);
        float tanHalfVFOV = tanf(cam.fov / 2.0f);

        float proj_near_z = -near;
        float proj_far_z = -far;
        float proj_near_y = proj_near_z * tanHalfVFOV;
        float proj_far_y = proj_far_z   * tanHalfVFOV;
        float proj_near_x = proj_near_z * tanHalfHFOV;
        float proj_far_x = proj_far_z   * tanHalfHFOV;

        // local positions
        m3d::vec4 frustum_corners[] =
        {
            // near
            m3d::vec4(proj_near_x,  -proj_near_y, proj_near_z, 1.0f),
            m3d::vec4(proj_near_x,   proj_near_y, proj_near_z, 1.0f),
            m3d::vec4(-proj_near_x, -proj_near_y, proj_near_z, 1.0f),
            m3d::vec4(-proj_near_x,  proj_near_y, proj_near_z, 1.0f),

            // far plane
            m3d::vec4(proj_far_x,  -proj_far_y, proj_far_z, 1.0f),
            m3d::vec4(proj_far_x,   proj_far_y, proj_far_z, 1.0f),
            m3d::vec4(-proj_far_x, -proj_far_y, proj_far_z, 1.0f),
            m3d::vec4(-proj_far_x,  proj_far_y, proj_far_z, 1.0f)
        };

        // transform to world space

        //Vec3 frustum_corners_W[8];
        std::vector<m3d::vec3> frustum_corners_L(8);
        m3d::mat4x4 inv_view = cam.getInverseView();

        for (uint8_t j = 0 ; j < 8 ; j++)
        {

            // Transform the frustum coordinate from view to world space
            m3d::vec4 vW = inv_view * frustum_corners[j];

            // Transform the frustum coordinate from world to light space
            m3d::vec4 vL = light_view * vW;
            frustum_corners_L[j] = m3d::vec3(vL.x, vL.y, vL.z);
            //frustum_corners_L[j] = m3d::vec3(vW.x, vW.y, vW.z);
        }

        /** ---- Fix Shadow Shimmering ---- **/

        /** -- Rotation shimmering -- **/

        dgn::BoundingSphere sphere;
        sphere.generateFromPoints(frustum_corners_L);
        sphere.radius *= scale_value;

        /** -- Position shimmering -- **/
        float rx2 = sphere.radius * 2.0f;
        m3d::vec2 texel_world_size = m3d::vec2(rx2 / m_texture.getWidth(),
                                rx2 / m_texture.getHeight());

        sphere.position.x /= texel_world_size.x;
        sphere.position.y /= texel_world_size.y;

        sphere.position.x = floor(sphere.position.x);
        sphere.position.y = floor(sphere.position.y);

        sphere.position.x *= texel_world_size.x;
        sphere.position.y *= texel_world_size.y;

        m3d::vec3 max = sphere.position + m3d::vec3(sphere.radius);
        m3d::vec3 min = sphere.position - m3d::vec3(sphere.radius);

        m_projection = m3d::mat4x4::initOrtho(max.x, min.x, max.y, min.y, min.z - near_pull, max.z);
        return *this;
    }

    Framebuffer ShadowMap::getFramebuffer()
    {
        return m_buffer;
    }

    Texture ShadowMap::getTexture()
    {
        return m_texture;
    }

    m3d::mat4x4 ShadowMap::getLightMat()
    {
        return m_projection * light_view;
    }
}
