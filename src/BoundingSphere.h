#pragma once

#include <m3d/vec3.h>
#include <vector>

namespace dgn
{
   class BoundingSphere
    {
        public:
            m3d::vec3 position;
            float radius;

            BoundingSphere();
            BoundingSphere(m3d::vec3 position, float radius);

            BoundingSphere& generateFromPoints(std::vector<m3d::vec3> points);
    };
}
