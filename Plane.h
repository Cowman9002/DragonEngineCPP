#pragma once

#include "Collider.h"

#include <m3d/vec3.h>
#include <vector>

namespace dgn
{
   class Plane : public Collider
    {
        public:
            m3d::vec3 normal;
            float distance;

            Plane();
            Plane(m3d::vec3 normal, float distance);

            Plane& generateFromPoints(m3d::vec3 p1, m3d::vec3 p2, m3d::vec3 p3);

            float distanceFrom(const m3d::vec3& point);

            virtual CollisionData checkCollision(const Collider* other) override;
            virtual CollisionData checkCollision(const m3d::vec3& point) override;
            virtual m3d::vec3 nearestPoint(const m3d::vec3& point) override;
    };
}
