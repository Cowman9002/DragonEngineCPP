#include "BoundingBox.h"
#include "d_internal.h"

#include "BoundingSphere.h"
#include "Plane.h"
#include "Triangle.h"

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
            //https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        case ColliderType::Plane:
            {
                Plane* b = (Plane*)other;

                // Convert AABB to center-extents representation
                m3d::vec3 c = (max + min) * 0.5f; // Compute AABB center
                m3d::vec3 e = max - c; // Compute positive extents

                // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
                float r = m3d::vec3::dot(e, m3d::vec3(std::abs(b->normal.x), std::abs(b->normal.y), std::abs(b->normal.z)));

                // Compute distance of box center from plane
                float s = m3d::vec3::dot(b->normal, c) - b->distance;

                // Intersection occurs when distance s falls within [-r,+r] interval
                res.hit = std::abs(s) <= r;

                break;
            }
        case ColliderType::Triangle:
            {
                Triangle* b = (Triangle*)other;

                m3d::vec3 v0 = b->p1;
                m3d::vec3 v1 = b->p2;
                m3d::vec3 v2 = b->p3;

                // Convert AABB to center-extents form
                m3d::vec3 c = (max + min) * 0.5f; // Compute AABB center
                m3d::vec3 e = max - c;

                // Translate the triangle as conceptually moving the AABB to origin
                // This is the same as we did with the point in triangle test
                v0 -= c;
                v1 -= c;
                v2 -= c;

                // Compute the edge vectors of the triangle  (ABC)
                // That is, get the lines between the points as vectors
                m3d::vec3 f0 = v1 - v0; // B - A
                m3d::vec3 f1 = v2 - v1; // C - B
                m3d::vec3 f2 = v0 - v2; // A - C

                // Compute the face normals of the AABB, because the AABB
                // is at center, and of course axis aligned, we know that
                // it's normals are the X, Y and Z axis.
                m3d::vec3 u0 = m3d::vec3(1.0f, 0.0f, 0.0f);
                m3d::vec3 u1 = m3d::vec3(0.0f, 1.0f, 0.0f);
                m3d::vec3 u2 = m3d::vec3(0.0f, 0.0f, 1.0f);

                // There are a total of 13 axis to test!

                // We first test against 9 axis, these axis are given by
                // cross product combinations of the edges of the triangle
                // and the edges of the AABB. You need to get an axis testing
                // each of the 3 sides of the AABB against each of the 3 sides
                // of the triangle. The result is 9 axis of seperation
                // https://awwapp.com/b/umzoc8tiv/

                // Compute the 9 axis
                m3d::vec3 axis_u0_f0 = m3d::vec3::cross(u0, f0);
                m3d::vec3 axis_u0_f1 = m3d::vec3::cross(u0, f1);
                m3d::vec3 axis_u0_f2 = m3d::vec3::cross(u0, f2);

                m3d::vec3 axis_u1_f0 = m3d::vec3::cross(u1, f0);
                m3d::vec3 axis_u1_f1 = m3d::vec3::cross(u1, f1);
                m3d::vec3 axis_u1_f2 = m3d::vec3::cross(u2, f2);

                m3d::vec3 axis_u2_f0 = m3d::vec3::cross(u2, f0);
                m3d::vec3 axis_u2_f1 = m3d::vec3::cross(u2, f1);
                m3d::vec3 axis_u2_f2 = m3d::vec3::cross(u2, f2);

                // Testing axis: axis_u0_f0
                // Project all 3 vertices of the triangle onto the Seperating axis
                float p0 = m3d::vec3::dot(v0, axis_u0_f0);
                float p1 = m3d::vec3::dot(v1, axis_u0_f0);
                float p2 = m3d::vec3::dot(v2, axis_u0_f0);
                // Project the AABB onto the seperating axis
                // We don't care about the end points of the prjection
                // just the length of the half-size of the AABB
                // That is, we're only casting the extents onto the
                // seperating axis, not the AABB center. We don't
                // need to cast the center, because we know that the
                // aabb is at origin compared to the triangle!
                float r = e.x * std::abs(m3d::vec3::dot(u0, axis_u0_f0)) +
                            e.y * std::abs(m3d::vec3::dot(u1, axis_u0_f0)) +
                            e.z * std::abs(m3d::vec3::dot(u2, axis_u0_f0));
                // Now do the actual test, basically see if either of
                // the most extreme of the triangle points intersects r
                // You might need to write Min & Max functions that take 3 arguments
                if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
                    // This means BOTH of the points of the projected triangle
                    // are outside the projected half-length of the AABB
                    // Therefore the axis is seperating and we can exit
                    res.hit = false;
                    break;
                }

                // Repeat this test for the other 8 seperating axis
                // You may wish to make some kind of a helper function to keep
                // things readable
                //TODO: 8 more SAT tests

                // Next, we have 3 face normals from the AABB
                // for these tests we are conceptually checking if the bounding box
                // of the triangle intersects the bounding box of the AABB
                // that is to say, the seperating axis for all tests are axis aligned:
                // axis1: (1, 0, 0), axis2: (0, 1, 0), axis3 (0, 0, 1)
                //TODO: 3 SAT tests
                // Do the SAT given the 3 primary axis of the AABB
                // You already have vectors for this: u0, u1 & u2

                // Finally, we have one last axis to test, the face normal of the triangle
                // We can get the normal of the triangle by crossing the first two line segments
                m3d::vec3 triangleNormal = m3d::vec3::cross(f0, f1);
                //TODO: 1 SAT test

                // Passed testing for all 13 seperating axis that exist!
                res.hit = true;

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

