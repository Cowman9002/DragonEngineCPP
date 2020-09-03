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
        Line,
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
        virtual m3d::vec3 nearestPoint(const m3d::vec3& point) = 0;
    };
}
