#pragma once

#include "Collider.h"

#include <m3d/vec3.h>
#include <vector>

namespace dgn
{
   class BoundingSphere : public Collider
    {
        public:
            m3d::vec3 position;
            float radius;

            BoundingSphere();
            BoundingSphere(m3d::vec3 position, float radius);

            BoundingSphere& generateFromPoints(std::vector<m3d::vec3> points);

            virtual CollisionData checkCollision(const Collider* other) override;
            virtual CollisionData checkCollision(const m3d::vec3& point) override;
            virtual m3d::vec3 nearestPoint(const m3d::vec3& point) override;
    };
}
