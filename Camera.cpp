#include "DragonEngine/Camera.h"

#include <m3d/Math1D.h>

namespace dgn
{
    Camera::Camera() : position(), rotation(), width(0), height(0), near(0.1f), far(100.0f), fov(90.0f * TO_RADS){}

    Camera::Camera(m3d::vec3 position, m3d::quat rotation, float width, float height, float near, float far, float fov) :
        position(position), rotation(rotation), width(width), height(height), near(near), far(far), fov(fov) {}

    m3d::mat4x4 Camera::getProjection() const
    {
        return m3d::mat4x4::initPerspective(width, height, fov, near, far);
    }

    m3d::mat4x4 Camera::getView() const
    {
        m3d::mat4x4 pos = m3d::mat4x4(1.0f).translate(-position);
        m3d::mat4x4 rot = m3d::mat4x4(1.0f).rotate(-rotation);

        return rot * pos;
    }

    m3d::mat4x4 Camera::getInverseView() const
    {
        m3d::mat4x4 pos = m3d::mat4x4(1.0f).translate(position);
        m3d::mat4x4 rot = m3d::mat4x4(1.0f).rotate(rotation);

        return pos * rot;
    }
}
