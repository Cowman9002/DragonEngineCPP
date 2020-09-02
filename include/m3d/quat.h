#pragma once

namespace m3d
{
    class vec3;
    class quat
    {
    public:
        float i, j, k, w;

        quat();
        quat(const float& angle, const vec3& axis);
        quat(const float& i, const float& j, const float& k, const float& w);

        static float angle(const quat& a, const quat& b);
        static quat angleVec3(const vec3& a, const vec3& b, const vec3& up);
        static quat conjugate(const quat& v);
        static vec3 euler(const quat& v);
        static quat face(const vec3& dir, const vec3& up);
        static float length(const quat& v);
        static float lengthSqr(const quat& v);
        static quat lerp(const quat& a, const quat& b, const float& t);
        static quat normalized(const quat& v);
        static vec3 rotateVec3(const quat& a, const vec3& b);
        static quat slerp(const quat& a, const quat& b, const float& t);

        static quat add(const quat& a, const quat& b);
        static quat sub(const quat& a, const quat& b);
        static quat mul(const quat& a, const quat& b);

        static quat mul(const quat& a, const float& b);
        static quat div(const quat& a, const float& b);

        static bool equals(const quat& a, const quat& b);


        float length() const;
        float lengthSqr() const;
        quat normalized()const;
        quat conjugate() const;
    };
}

bool operator==(const m3d::quat& a, const m3d::quat&b);
bool operator!=(const m3d::quat& a, const m3d::quat&b);

m3d::quat operator-(const m3d::quat& a);

m3d::quat operator+(const m3d::quat& a, const m3d::quat& b);
m3d::quat operator-(const m3d::quat& a, const m3d::quat& b);
m3d::quat operator*(const m3d::quat& a, const m3d::quat& b);
m3d::quat operator*(const m3d::quat& a, const float& b);
m3d::quat operator/(const m3d::quat& a, const float& b);

m3d::vec3 operator*(const m3d::quat& a, const m3d::vec3& b);
m3d::vec3 operator*(const m3d::vec3& a, const m3d::quat& b);
m3d::vec3 operator*=(m3d::vec3& a, const m3d::quat& b);

m3d::quat& operator+=(m3d::quat& a, const m3d::quat& b);
m3d::quat& operator-=(m3d::quat& a, const m3d::quat& b);
m3d::quat& operator*=(m3d::quat& a, const m3d::quat& b);
m3d::quat& operator*=(m3d::quat& a, const float& b);
m3d::quat& operator/=(m3d::quat& a, const float& b);
