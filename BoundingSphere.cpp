#include "BoundingSphere.h"
#include "d_internal.h"

#include "BoundingBox.h"
#include "Plane.h"
#include "Triangle.h"

#include <algorithm>
#include <limits>
#include <string>

namespace dgn
{
    BoundingSphere::BoundingSphere() : BoundingSphere(m3d::vec3(), 0.0f) {}

    BoundingSphere::BoundingSphere(m3d::vec3 position, float radius) :
        Collider(ColliderType::Sphere), position(position), radius(radius) {}

    BoundingSphere& BoundingSphere::generateFromPoints(std::vector<m3d::vec3> points)
    {
        m3d::vec3 points_summed;
        float d = -std::numeric_limits<float>::max();

        size_t points_count = points.size();
        for(unsigned i = 0; i < points_count; i++)
        {
            points_summed += points[i];
            for(unsigned j = 0; j < i; j++)
            {
                float dist = m3d::vec3::distance(points[i], points[j]);
                d = std::max(dist, d);
            }
        }

        radius = d / 2.0f;
        position = points_summed / points_count;

        return *this;
    }

    CollisionData BoundingSphere::checkCollision(const Collider* other)
    {
        CollisionData res;

        switch(other->getType())
        {
        case ColliderType::Sphere:
            {
                BoundingSphere* b = (BoundingSphere*)other;
                float dist = m3d::vec3::distance(position, b->position);
                res.hit = dist <= (radius + b->radius);
            break;
            }
        case ColliderType::Box:
            {
                BoundingBox* b = (BoundingBox*)other;
                res = b->checkCollision(this);
            break;
            }
        case ColliderType::Plane:
            {
                Plane* b = (Plane*)other;
                float dist = m3d::vec3::distance(b->nearestPoint(position), position);
                res.hit = dist <= radius;

                break;
            }
        case ColliderType::Triangle:
            {
                Triangle* b = (Triangle*)other;
                float dist = m3d::vec3::distance(b->nearestPoint(position), position);
                res.hit = dist <= radius;

                break;
            }
        default:
            logError("COLLISION", ("Unsupported combination of colliders: " +
                                   std::to_string(int(getType())) +
                                   " " +
                                   std::to_string(int(other->getType()))).c_str());
            break;
        }

        return res;
    }

    CollisionData BoundingSphere::checkCollision(const m3d::vec3& point)
    {
        CollisionData res;
        res.hit = false;

        float dist = m3d::vec3::distance(position, point);
        res.hit = dist <= radius;

        return res;
    }

    m3d::vec3 BoundingSphere::nearestPoint(const m3d::vec3& point)
    {
        m3d::vec3 res;

        m3d::vec3 dir = point - position;
        dir = dir.normalized();

        res = position + dir * radius;

        return res;
    }

}
