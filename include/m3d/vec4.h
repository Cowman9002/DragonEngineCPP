#pragma once

namespace m3d
{
    class vec3;
    class vec4
    {
    public:

        float x, y, z, w;

        vec4();
        vec4(const float& v);
        vec4(const vec3& v);
        vec4(const vec3& v, const float& w);
        vec4(const float& x, const float& y, const float& z, const float& w);

        vec3 xyz() const;

        static vec4 add(const vec4& a, const vec4& b);
        static vec4 add(const vec4& a, const float& b);
        static vec4 sub(const vec4& a, const vec4& b);
        static vec4 sub(const vec4& a, const float& b);
        static vec4 mul(const vec4& a, const float& b);
        static vec4 div(const vec4& a, const float& b);

        static bool equals(const vec4& a, const vec4& b);
    };
}

bool operator==(const m3d::vec4& a, const m3d::vec4&b);
bool operator!=(const m3d::vec4& a, const m3d::vec4&b);

m3d::vec4 operator-(const m3d::vec4& a);

m3d::vec4 operator+(const m3d::vec4& a, const m3d::vec4& b);
m3d::vec4 operator+(const m3d::vec4& a, const float& b);
m3d::vec4 operator-(const m3d::vec4& a, const m3d::vec4& b);
m3d::vec4 operator-(const m3d::vec4& a, const float& b);
m3d::vec4 operator*(const m3d::vec4& a, const float& b);
m3d::vec4 operator/(const m3d::vec4& a, const float& b);

m3d::vec4& operator+=(m3d::vec4& a, const m3d::vec4& b);
m3d::vec4& operator+=(m3d::vec4& a, const float& b);
m3d::vec4& operator-=(m3d::vec4& a, const m3d::vec4& b);
m3d::vec4& operator-=(m3d::vec4& a, const float& b);
m3d::vec4& operator*=(m3d::vec4& a, const float& b);
m3d::vec4& operator/=(m3d::vec4& a, const float& b);
