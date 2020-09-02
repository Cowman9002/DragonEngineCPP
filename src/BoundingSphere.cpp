#include "BoundingSphere.h"

#include <float.h>
#include <math.h>

namespace dgn
{
    BoundingSphere::BoundingSphere() : radius(0), position() {}

    BoundingSphere::BoundingSphere(m3d::vec3 position, float radius) : position(position), radius(radius) {}

    BoundingSphere& BoundingSphere::generateFromPoints(std::vector<m3d::vec3> points)
    {
        m3d::vec3 points_summed;
        float d = -FLT_MAX;

        size_t points_count = points.size();
        for(int i = 0; i < points_count; i++)
        {
            points_summed += points[i];
            for(int j = 0; j < i; j++)
            {
                float dist = m3d::vec3::distance(points[i], points[j]);
                d = fmaxf(dist, d);
            }
        }

        radius = d / 2.0f;
        position = points_summed / points_count;

        return *this;
    }
}
