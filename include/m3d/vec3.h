#pragma once

namespace m3d
{
    class vec2;
    class vec3
    {
    public:

        float x, y, z;

        vec3();
        vec3(const float& v);
        vec3(const vec2& v, const float& z);
        vec3(const float& x, const vec2& v);
        vec3(const float& x, const float& y, const float& z);

        vec2 xy() const;
        vec2 yz() const;
        vec2 xz() const;

        vec2 yx() const;
        vec2 zy() const;
        vec2 zx() const;

        static float angle(const vec3& a, const vec3& b);
        static vec3 cross(const vec3& a, const vec3& b);
        static float distance(const vec3& a, const vec3& b);
        static float distanceSqr(const vec3& a, const vec3& b);
        static float dot(const vec3& a, const vec3& b);
        static float length(const vec3& v);
        static float lengthSqr(const vec3& v);
        static vec3 lerp(const vec3& a, const vec3& b, const float& t);
        static vec3 max(const vec3& a, const vec3& b);
        static vec3 min(const vec3& a, const vec3& b);
        static vec3 normalized(const vec3& v);
        static vec3 reflect(const vec3& v, const vec3& normal);
        static vec3 slerp(const vec3& a, const vec3& b, const float& t);

        static vec3 add(const vec3& a, const vec3& b);
        static vec3 add(const vec3& a, const float& b);
        static vec3 sub(const vec3& a, const vec3& b);
        static vec3 sub(const vec3& a, const float& b);
        static vec3 mul(const vec3& a, const float& b);
        static vec3 div(const vec3& a, const float& b);

        static bool equals(const vec3& a, const vec3& b);

        vec3 normalized() const;

        float length() const;
        float lengthSqr() const;

        float dot(const vec3& b) const;
    };
}

bool operator==(const m3d::vec3& a, const m3d::vec3&b);
bool operator!=(const m3d::vec3& a, const m3d::vec3&b);

m3d::vec3 operator-(const m3d::vec3& a);

m3d::vec3 operator+(const m3d::vec3& a, const m3d::vec3& b);
m3d::vec3 operator+(const m3d::vec3& a, const float& b);
m3d::vec3 operator-(const m3d::vec3& a, const m3d::vec3& b);
m3d::vec3 operator-(const m3d::vec3& a, const float& b);
m3d::vec3 operator*(const m3d::vec3& a, const float& b);
m3d::vec3 operator/(const m3d::vec3& a, const float& b);

m3d::vec3& operator+=(m3d::vec3& a, const m3d::vec3& b);
m3d::vec3& operator+=(m3d::vec3& a, const float& b);
m3d::vec3& operator-=(m3d::vec3& a, const m3d::vec3& b);
m3d::vec3& operator-=(m3d::vec3& a, const float& b);
m3d::vec3& operator*=(m3d::vec3& a, const float& b);
m3d::vec3& operator/=(m3d::vec3& a, const float& b);
