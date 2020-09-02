#pragma once

#include <string>

namespace m3d
{
    class vec2;
    class vec3;
    class vec4;
    class mat3x3;
    class mat4x4;
}

namespace dgn
{
    class Shader
    {
        friend class Renderer;

    private:
        unsigned m_program;

    public:
        Shader();

        void dispose();

        Shader& createFromData(std::string vertex_code, std::string geometry_code, std::string fragment_code);
        Shader& loadFromFiles(std::string vertex_path, std::string geometry_path, std::string fragment_path);

        int getUniformLocation(std::string name) const;

        static void uniform(int loc, float value);
        static void uniform(int loc, int value);
        static void uniform(int loc, bool value);
        static void uniform(int loc, m3d::vec2 value);
        static void uniform(int loc, m3d::vec3 value);
        static void uniform(int loc, m3d::vec4 value);
        static void uniform(int loc, m3d::mat3x3 value);
        static void uniform(int loc, m3d::mat4x4 value);

        static void setEconst(std::string name, int value);
    };
}
