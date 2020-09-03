#pragma once

namespace m3d
{
    class vec3;
}

namespace dgn
{
    enum class ColliderType
    {
        Box,
        Sphere,
        Plane,
        Triangle,
        Frustum
    };

    struct CollisionData
    {
        bool hit;
    };

    class Collider
    {
    private:
        const ColliderType collider_type;
    protected:
        inline Collider(const ColliderType& type) : collider_type(type) {};
    public:
        inline ColliderType getType() const { return collider_type; };
        virtual CollisionData checkCollision(const Collider* other) = 0;

        virtual CollisionData checkCollision(const m3d::vec3& point) = 0;
        //virtual CollisionData checkCollision(const m3d::vec3& point1, const m3d::vec3& p2) = 0;
        //virtual CollisionData checkCollision(const m3d::vec3& start, const m3d::vec3& dir) = 0;
        virtual m3d::vec3 nearestPoint(const m3d::vec3& point) = 0;


        static CollisionData checkCollision(m3d::vec3 line_1, m3d::vec3 line_2, m3d::vec3 point);
        static m3d::vec3 nearestPoint(m3d::vec3 line_1, m3d::vec3 line_2, m3d::vec3 point);
    };
}
