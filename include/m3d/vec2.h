#pragma once

namespace m3d
{
    class vec2
    {
    public:

        float x, y;

        vec2();
        vec2(const float& v);
        vec2(const float& x, const float& y);

        static float angle(const vec2& a, const vec2& b);
        static float distance(const vec2& a, const vec2& b);
        static float distanceSqr(const vec2& a, const vec2& b);
        static float dot(const vec2& a, const vec2& b);
        static float length(const vec2& v);
        static float lengthSqr(const vec2& v);
        static vec2 lerp(const vec2& a, const vec2& b, const float& t);
        static vec2 max(const vec2& a, const vec2& b);
        static vec2 min(const vec2& a, const vec2& b);
        static vec2 normalized(const vec2& v);
        static vec2 reflect(const vec2& v, const vec2& normal);
        static vec2 slerp(const vec2& a, const vec2& b, const float& t);


        static vec2 add(const vec2& a, const vec2& b);
        static vec2 add(const vec2& a, const float& b);
        static vec2 sub(const vec2& a, const vec2& b);
        static vec2 sub(const vec2& a, const float& b);
        static vec2 mul(const vec2& a, const float& b);
        static vec2 div(const vec2& a, const float& b);

        static bool equals(const vec2& a, const vec2& b);

        vec2 normalized() const;

        float length() const;
        float lengthSqr() const;

        float dot(const vec2& b) const;
    };
}

bool operator==(const m3d::vec2& a, const m3d::vec2&b);
bool operator!=(const m3d::vec2& a, const m3d::vec2&b);

m3d::vec2 operator-(const m3d::vec2& a);

m3d::vec2 operator+(const m3d::vec2& a, const m3d::vec2& b);
m3d::vec2 operator+(const m3d::vec2& a, const float& b);
m3d::vec2 operator-(const m3d::vec2& a, const m3d::vec2& b);
m3d::vec2 operator-(const m3d::vec2& a, const float& b);
m3d::vec2 operator*(const m3d::vec2& a, const float& b);
m3d::vec2 operator/(const m3d::vec2& a, const float& b);

m3d::vec2& operator+=(m3d::vec2& a, const m3d::vec2& b);
m3d::vec2& operator+=(m3d::vec2& a, const float& b);
m3d::vec2& operator-=(m3d::vec2& a, const m3d::vec2& b);
m3d::vec2& operator-=(m3d::vec2& a, const float& b);
m3d::vec2& operator*=(m3d::vec2& a, const float& b);
m3d::vec2& operator/=(m3d::vec2& a, const float& b);
