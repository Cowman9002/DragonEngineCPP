#include "Triangle.h"
#include "d_internal.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "Plane.h"

#include <algorithm>
#include <cmath>
#include <string>

#include <m3d/math1D.h>
#include <m3d/vec2.h>

namespace dgn
{

    Triangle::Triangle() :Triangle(m3d::vec3(), m3d::vec3(), m3d::vec3()) {}
    Triangle::Triangle(m3d::vec3 p1, m3d::vec3 p2, m3d::vec3 p3) :
        Collider(ColliderType::Triangle), p1(p1), p2(p2), p3(p3) {}

    bool TriTriSAT(const Triangle& tri1, const Triangle& tri2, const m3d::vec3& axis)
    {
        // get intervals

        float j1 = m3d::vec3::dot(tri1.p1, axis);
        float j2 = m3d::vec3::dot(tri1.p2, axis);
        float j3 = m3d::vec3::dot(tri1.p3, axis);

        float k1 = m3d::vec3::dot(tri2.p1, axis);
        float k2 = m3d::vec3::dot(tri2.p2, axis);
        float k3 = m3d::vec3::dot(tri2.p3, axis);

        m3d::vec2 tri1_int = m3d::vec2(std::max(j1, std::max(j2, j3)), std::min(j1, std::min(j2, j3)));
        m3d::vec2 tri2_int = m3d::vec2(std::max(k1, std::max(k2, k3)), std::min(k1, std::min(k2, k3)));

        if (tri1_int.x < tri2_int.y || tri2_int.x < tri1_int.y)
        {
            return false;
        }

        return true;
    }

    CollisionData Triangle::checkCollision(const Collider* other)
    {
        CollisionData res;

        switch(other->getType())
        {
        case ColliderType::Triangle:
            {
                Triangle *b = (Triangle*)other;


                m3d::vec3 f0 = p2 - p1; // B - A
                m3d::vec3 f1 = p3 - p2; // C - B
                m3d::vec3 f2 = p1 - p3; // A - C

                m3d::vec3 u0 = b->p2 - b->p1; // B - A
                m3d::vec3 u1 = b->p3 - b->p2; // C - B
                m3d::vec3 u2 = b->p1 - b->p3; // A - C


                m3d::vec3 axis[] =
                {
                    m3d::vec3::cross(f0, f1).normalized(),
                    m3d::vec3::cross(u0, u1).normalized(),

                    m3d::vec3::cross(u0, f0).normalized(),
                    m3d::vec3::cross(u0, f1).normalized(),
                    m3d::vec3::cross(u0, f2).normalized(),

                    m3d::vec3::cross(u1, f0).normalized(),
                    m3d::vec3::cross(u1, f1).normalized(),
                    m3d::vec3::cross(u1, f2).normalized(),

                    m3d::vec3::cross(u2, f0).normalized(),
                    m3d::vec3::cross(u2, f1).normalized(),
                    m3d::vec3::cross(u2, f2).normalized()
                };

                res.hit = true;

                for(int i = 0; i < 11; i++)
                {
                    if(!TriTriSAT(*this, *b, axis[i]))
                    {
                        res.hit = false;
                        break;
                    }
                }

                break;
            }
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
                res = b->checkCollision(this);
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

    CollisionData Triangle::checkCollision(const m3d::vec3& point)
    {
        CollisionData res;
        res.hit = false;

        // move triangle so point is triangle's origin
        m3d::vec3 p1_c = p1 - point;
        m3d::vec3 p2_c = p2 - point;
        m3d::vec3 p3_c = p3 - point;

        // u = normal of PBC
        // v = normal of PCA
        // w = normal of PAB

        m3d::vec3 u = m3d::vec3::cross(p2_c, p3_c);
        m3d::vec3 v = m3d::vec3::cross(p3_c, p1_c);
        m3d::vec3 w = m3d::vec3::cross(p1_c, p2_c);

        // Test to see if the normals are facing
        // the same direction
        if (m3d::vec3::dot(u, v) < 0.0f)
        {
            res.hit = false;
        }
        else if (m3d::vec3::dot(u, w) < 0.0f)
        {
            res.hit = false;
        }
        else
        {
            res.hit = true;
        }

        return res;
    }

    m3d::vec3 Triangle::nearestPoint(const m3d::vec3& point)
    {
        // convert to plane and find nearest point on plane
        Plane plane;
        plane.generateFromPoints(p1, p2, p3);
        m3d::vec3 nearest_point = plane.nearestPoint(point);
        // check if point is inside plane, if so, return that point
        if(checkCollision(nearest_point).hit)
        {
            return nearest_point;
        }
        // find the nearest point on each of the edges

        m3d::vec3 edge1_p1 = p1;
        m3d::vec3 edge1_p2 = p2;

        m3d::vec3 edge2_p1 = p1;
        m3d::vec3 edge2_p2 = p3;

        m3d::vec3 edge3_p1 = p2;
        m3d::vec3 edge3_p2 = p3;

        m3d::vec3 ep1 = Collider::nearestPoint(edge1_p1, edge1_p2, point);
        m3d::vec3 ep2 = Collider::nearestPoint(edge2_p1, edge2_p2, point);
        m3d::vec3 ep3 = Collider::nearestPoint(edge3_p1, edge3_p2, point);

        // return the point with the smallest distance from point
        float dist1 = m3d::vec3::lengthSqr(point - ep1);
        float dist2 = m3d::vec3::lengthSqr(point - ep2);
        float dist3 = m3d::vec3::lengthSqr(point - ep3);

        float min_dist = std::min(std::min(dist1, dist2), dist3);

        if(min_dist == dist1)
        {
            return ep1;
        }
        else if(min_dist == dist2)
        {
            return ep2;
        }
        else
        {
            return ep3;
        }
    }

    CollisionData Collider::checkCollision(m3d::vec3 line_1, m3d::vec3 line_2, m3d::vec3 point)
    {
        CollisionData res;
        res.hit = false;

        float m = (line_2.y - line_1.y) / (line_2.x - line_1.x);
        float b = line_1.y - m * line_1.x;

        float pass = point.y - (m * point.x + b);

        if (std::abs(pass) < 0.001f)
        {
            res.hit = true;
        }

        return res;
    }

    m3d::vec3 Collider::nearestPoint(m3d::vec3 line_1, m3d::vec3 line_2, m3d::vec3 point)
    {
        m3d::vec3 l = line_2 - line_1;
        m3d::vec3 p = point - line_1;
        float d = m3d::vec3::dot(l, p) / m3d::vec3::lengthSqr(l);
        d = m3d::clamp(d, 0.0f, 1.0f);

        return line_1 + l * d;
    }
}
