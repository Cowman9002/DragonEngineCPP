#pragma once

#include "Collider.h"
#include <vector>
#include <m3d/vec3.h>

namespace dgn
{
    class BoundingBox : public Collider
    {
    public:
        m3d::vec3 max, min;

        BoundingBox();
        BoundingBox(m3d::vec3 max, m3d::vec3 min);
        BoundingBox& genFromPoints(std::vector<m3d::vec3> points);

        BoundingBox& normalize();

        virtual CollisionData checkCollision(const Collider* other) override;
        virtual CollisionData checkCollision(const m3d::vec3& point) override;
        virtual m3d::vec3 nearestPoint(const m3d::vec3& point) override;
    };
}
