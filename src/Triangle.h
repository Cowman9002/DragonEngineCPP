#include "Collider.h"

#include <m3d/vec3.h>

namespace dgn
{
    class Triangle : public Collider
    {
    public:

        m3d::vec3 p1, p2, p3;

        Triangle();
        Triangle(m3d::vec3 p1, m3d::vec3 p2, m3d::vec3 p3);

        virtual CollisionData checkCollision(const Collider* other) override;
        virtual CollisionData checkCollision(const m3d::vec3& point) override;
        virtual m3d::vec3 nearestPoint(const m3d::vec3& point) override;
    };
}
