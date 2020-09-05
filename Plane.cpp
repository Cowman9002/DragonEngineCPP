#include "Plane.h"
#include "d_internal.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "Triangle.h"

#include <algorithm>
#include <limits>
#include <string>

namespace dgn
{
    Plane::Plane() : Plane(m3d::vec3(), 0.0f) {}
    Plane::Plane(m3d::vec3 normal, float distance):
        Collider(ColliderType::Plane), normal(normal.normalized()), distance(distance) {}

    Plane& Plane::generateFromPoints(m3d::vec3 p1, m3d::vec3 p2, m3d::vec3 p3)
    {
        normal = m3d::vec3::cross(p2 - p1, p3 - p1).normalized();
        distance = m3d::vec3::dot(normal, p1);
        return *this;
    }

    float Plane::distanceFrom(const m3d::vec3& point)
    {
        return m3d::vec3::dot(point, normal) - distance;
    }

    int sign(float v)
    {
        return (0.0f < v) - (v < 0.0f);
    }

    CollisionData Plane::checkCollision(const Collider* other)
    {
        CollisionData res;
        res.hit = false;


        switch(other->getType())
        {
        case ColliderType::Sphere:
            {
                BoundingSphere* b = (BoundingSphere*)other;
                res = b->checkCollision(this);
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

                m3d::vec3 d = m3d::vec3::cross(b->normal, normal);

                res.hit = d.lengthSqr() > 0.0001f;

                break;
            }
        case ColliderType::Triangle:
            {
                Triangle* b = (Triangle*)other;

                // get signed distances from plane
                float d1 = distanceFrom(b->p1);
                float d2 = distanceFrom(b->p2);
                float d3 = distanceFrom(b->p3);

                if(abs(d1 + d2 + d3) < 0.001) // all equal 0, on plane
                {
                    res.hit = true;
                }
                else if(!(sign(d1) == sign(d2) && sign(d2) == sign(d3)))
                {
                    res.hit = true;
                }

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

    CollisionData Plane::checkCollision(const m3d::vec3& point)
    {
        CollisionData res;
        res.hit = false;

        res.hit = std::abs(distanceFrom(point)) < 0.001;

        return res;
    }

    m3d::vec3 Plane::nearestPoint(const m3d::vec3& point)
    {
        return point - normal * distanceFrom(point);
    }
}
