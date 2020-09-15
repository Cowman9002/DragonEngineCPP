#pragma once

#include <m3d/vec3.h>
#include <m3d/quat.h>
#include <m3d/mat4x4.h>

namespace dgn
{
    class Camera
    {
    public:

        m3d::vec3 position;
        m3d::quat rotation;
        float width, height;
        float near, far;
        float fov;

        Camera();
        Camera(m3d::vec3 position, m3d::quat rotation, float width, float height, float fov, float near, float far);

        m3d::mat4x4 getProjection() const;
        m3d::mat4x4 getView() const;
        m3d::mat4x4 getInverseView() const;
    };
}
