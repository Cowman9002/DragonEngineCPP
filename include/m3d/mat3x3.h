#pragma once

namespace m3d
{
    class vec2;
    class vec3;
    class quat;
    class mat4x4;
    class mat3x3
    {
    public:
        float m[3][3] = {0};

        mat3x3();
        mat3x3(const float& diagonal);

        static mat3x3 initOrtho(const float& right, const float& left, const float& top, const float& bottom);
        static mat3x3 initOrthoCentered(const float& width, const float& height);
        static mat3x3 initRotationFromQuat(const quat& quat);

        static mat3x3 mul(const mat3x3& a, const mat3x3& b);
        static vec3 mul(const mat3x3& a, const vec3& b);

        static mat3x3 fromMat4x4(const mat4x4& m);

        mat3x3& rotate(const float& radians);
        mat3x3& scale(const vec2& scale);
        mat3x3& translate(const vec2& translation);
        mat4x4 toMat4x4();
    };
}

m3d::mat3x3 operator*(const m3d::mat3x3& a, const m3d::mat3x3& b);
m3d::vec3 operator*(const m3d::mat3x3& a, const m3d::vec3& b);
m3d::vec3 operator*(const m3d::vec3& a, const m3d::mat3x3& b);

m3d::mat3x3& operator*=(m3d::mat3x3& a, const m3d::mat3x3& b);
m3d::vec3& operator*=(m3d::vec3& a, const m3d::mat3x3& b);
