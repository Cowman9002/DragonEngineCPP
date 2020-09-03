#include "BoundingBox.h"
#include "d_internal.h"

#include "BoundingSphere.h"

#include <algorithm>
#include <limits>
#include <string>

namespace dgn
{
    BoundingBox::BoundingBox() : BoundingBox(m3d::vec3(0), m3d::vec3(0)) {}

    BoundingBox::BoundingBox(m3d::vec3 max, m3d::vec3 min) :
        Collider(ColliderType::Box), max(max), min(min){}

    BoundingBox& BoundingBox::genFromPoints(std::vector<m3d::vec3> points)
    {
        float max_float = std::numeric_limits<float>::max();
        max = m3d::vec3(-max_float);
        min = m3d::vec3(max_float);

        for(const m3d::vec3& v : points)
        {
            max = m3d::vec3::max(max, v);
            min = m3d::vec3::min(min, v);
        }

        return *this;
    }

    BoundingBox& BoundingBox::normalize()
    {
        m3d::vec3 temp = max;
        max = m3d::vec3::max(max, min);
        min = m3d::vec3::min(temp, min);

        return *this;
    }

    CollisionData BoundingBox::checkCollision(const Collider* other)
    {
        CollisionData res;
        res.hit = false;

        switch(other->getType())
        {
        case ColliderType::Box:
            {
                BoundingBox* b = (BoundingBox*)other;
                if(min.x <= b->max.x && max.x >= b->min.x &&
                min.y <= b->max.y && max.y >= b->min.y &&
                min.z <= b->max.z && max.z >= b->min.z)
                {
                    res.hit = true;
                }
            break;
            }
        case ColliderType::Sphere:
            {
                BoundingSphere* b = (BoundingSphere*)other;

                // this is the same as isPointInsideSphere
                float dist = m3d::vec3::distance(b->position, nearestPoint(b->position));

                res.hit = dist <= b->radius;

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

    CollisionData BoundingBox::checkCollision(const m3d::vec3& point)
    {
        CollisionData res;
        res.hit = false;

        if(point.x > min.x && point.x < max.x &&
           point.y > min.y && point.y < max.y &&
           point.z > min.z && point.z < max.z)
        {
            res.hit = true;
        }

        return res;
    }

    m3d::vec3 BoundingBox::nearestPoint(const m3d::vec3& point)
    {
        m3d::vec3 nearest_point;
        nearest_point.x = std::max(min.x, std::min(point.x, max.x));
        nearest_point.y = std::max(min.y, std::min(point.y, max.y));
        nearest_point.z = std::max(min.z, std::min(point.z, max.z));

        return nearest_point;
    }
}

