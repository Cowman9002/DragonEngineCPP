#pragma once

namespace m3d
{
    class vec3;
    class vec4;
    class quat;
    class mat3x3;
    class mat4x4
    {
    public:
        float m[4][4] = {0};

        mat4x4();
        mat4x4(const float& diagonal);

        static mat4x4 initOrtho(const float& r, const float& l, const float& t, const float& b, const float& n, const float& f);
        static mat4x4 initOrthoCentered(const float& w, const float& h, const float& n, const float& f);
        static mat4x4 initPerspective(const float& w, const float& h, const float& fov, const float& n, const float& f);

        static mat4x4 mul(const mat4x4& a, const mat4x4& b);
        static vec4 mul(const mat4x4& a, const vec4& b);

        static mat4x4 fromMat3x3(const mat3x3& m);

        static mat4x4 inverseHomogeneous(const mat4x4& mat);

        mat4x4& rotateX(const float& rotation);
        mat4x4& rotateY(const float& rotation);
        mat4x4& rotateZ(const float& rotation);

        mat4x4& rotate(const quat& rotation);
        mat4x4& scale(const vec3& scale);
        mat4x4& translate(const vec3& translation);
        mat3x3 toMat3x3();
    };
}

m3d::mat4x4 operator*(const m3d::mat4x4& a, const m3d::mat4x4& b);
m3d::vec4 operator*(const m3d::mat4x4& a, const m3d::vec4& b);
m3d::vec4 operator*(const m3d::vec4& a, const m3d::mat4x4& b);

m3d::mat4x4& operator*=(m3d::mat4x4& a, const m3d::mat4x4& b);
m3d::vec4& operator*=(m3d::vec4& a, const m3d::mat4x4& b);
