#include "DragonEngine/Shader.h"
#include "d_internal.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <m3d/vec2.h>
#include <m3d/vec3.h>
#include <m3d/vec4.h>
#include <m3d/mat3x3.h>
#include <m3d/mat4x4.h>

namespace dgn
{
    std::string fileToString(std::string filepath);

    std::unordered_map<std::string, int> Shader::econst_ints;

    Shader::Shader() : m_program(0) {}
    void Shader::dispose()
    {
        glCall(glDeleteProgram(m_program));
    }

    unsigned Shader::genShaderInternal(std::string data, GLenum shader_type)
    {
        if(data.empty()) return 0;

        std::istringstream stream(data);
        std::vector<std::string> lines;

        size_t k = 0;
        size_t j = 0;
        std::string line;
        while(std::getline(stream, line))
        {
            size_t p = line.find("#include");

            if(p != std::string::npos)
            {
                size_t n = line.find(" ");
                std::string filepath = line.substr(n + 1);

                std::string file_to_append = fileToString(filepath);

                k = data.find("#include", k);
                std::string first_half = data.substr(0, k);
                k += line.length();
                std::string second_half = file_to_append + data.substr(k);

                stream = std::istringstream(second_half);

                data = first_half;
                data += second_half;
            }
            else
            {
                p = line.find("econst");

                if(p != std::string::npos)
                {
                    size_t n = line.find(" ");
                    size_t m = line.find(" ", n + 1);
                    const char* type = line.substr(n + 1, m - n - 1).c_str();
                    const char* name = line.substr(m + 1, line.length() - m - 2).c_str();

                    j = data.find("econst", j);
                    std::string first_half = data.substr(0, j);
                    j += line.length();
                    std::string second_half;


                    if(type == std::string("int"))
                    {
                        auto v = Shader::econst_ints.find(name);

                        if(v != Shader::econst_ints.end())
                        {
                            int value = v->second;

                            second_half = std::string("const int ") + name +
                                        std::string(" = ") + std::to_string(value) + std::string(";") +
                                        data.substr(j);
                        }
                    }
                    stream = std::istringstream(second_half);

                    data = first_half;
                    data += second_half;

                }
                else
                {
                    lines.push_back(line);
                }

            }
        }

        //longMessagef("%s\n", data.c_str());

        const char* d = data.c_str();

        glCall(unsigned shader = glCreateShader(shader_type));
        glCall(glShaderSource(shader, 1, &d, NULL));
        glCall(glCompileShader(shader));

        int success;
        char buff[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, buff);
            logError("SHADER COMPILE STATUS", buff);

            std::string buff_s = buff;

            size_t debug_v = buff_s.find("ERROR: ");
            size_t total_line_num = lines.size();

            while((debug_v != std::string::npos))
            {
                debug_v = buff_s.find(":", debug_v + 7) + 1;
                std::string line_s = buff_s.substr(debug_v);
                size_t debug_n = line_s.find(":");
                int line_num = std::stoi(line_s.substr(0, debug_n));

                for(int i = -2; i < 3; i++)
                {
                    int line = line_num + i;

                    if(line > 0)
                    {
                        if(unsigned(line) < total_line_num)
                        {
                            logError(std::to_string(line).c_str(), lines[line - 1].c_str());
                        }
                    }
                }

                debug_v = buff_s.find("ERROR: ", debug_v);
            }

        }

        return shader;
    }

    Shader& Shader::createFromData(std::string vertex_code, std::string geometry_code, std::string fragment_code)
    {
        glCall(unsigned program = glCreateProgram());

        unsigned vertex   = genShaderInternal(vertex_code, GL_VERTEX_SHADER);
        unsigned geometry = genShaderInternal(geometry_code, GL_GEOMETRY_SHADER);
        unsigned fragment = genShaderInternal(fragment_code, GL_FRAGMENT_SHADER);

        if(vertex != 0)
        {
            glCall(glAttachShader(program, vertex));
        }

        if(geometry != 0)
        {
            glCall(glAttachShader(program, geometry));
        }

        if(fragment != 0)
        {
            glCall(glAttachShader(program, fragment));
        }

        glCall(glLinkProgram(program));

        #ifdef __DEBUG__
        int success;
        char buff[256];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(program, 256, NULL, buff);
            logError("SHADER LINKING STATUS", buff);
        }
        #endif // __DEBUG

        glCall(glDeleteShader(vertex));
        glCall(glDeleteShader(geometry));
        glCall(glDeleteShader(fragment));

        m_program = program;

        return *this;
    }

    std::string fileToString(std::string filepath)
    {
        if(filepath.empty()) return "";

        std::ifstream file;
        file.open(filepath.c_str());

        if(!file.is_open())
        {
            logError("FILE LOADING", filepath.c_str());
            return "";
        }

        std::string res;

        char line[512];

        while(!file.eof())
        {
            file.getline(line, 512);

            res += line;
            res += "\n";
        }

        file.close();

        return res;
    }

    Shader& Shader::loadFromFiles(std::string vertex_path, std::string geometry_path, std::string fragment_path)
    {
        std::string v_code = fileToString(vertex_path);
        std::string g_code = fileToString(geometry_path);
        std::string f_code = fileToString(fragment_path);

        return createFromData(v_code, g_code, f_code);
    }

    int Shader::getUniformLocation(std::string name) const
    {
        glCall(int loc = glGetUniformLocation(m_program, name.c_str()));

        if(loc == -1)
        {
            logError("UNIFORM NOT FOUND", name.c_str());
        }

        return loc;
    }

    void Shader::uniform(int loc, float value)
    {
        glCall(glUniform1f(loc, value));
    }

    void Shader::uniform(int loc, int value)
    {
        glCall(glUniform1i(loc, value));
    }

    void Shader::uniform(int loc, bool value)
    {
        glCall(glUniform1i(loc, value));
    }

    void Shader::uniform(int loc, m3d::vec2 value)
    {
        glCall(glUniform2f(loc, value.x, value.y));
    }

    void Shader::uniform(int loc, m3d::vec3 value)
    {
        glCall(glUniform3f(loc, value.x, value.y, value.z));
    }

    void Shader::uniform(int loc, m3d::vec4 value)
    {
        glCall(glUniform4f(loc, value.x, value.y, value.z, value.w));
    }

    void Shader::uniform(int loc, m3d::mat3x3 value)
    {
        glCall(glUniformMatrix3fv(loc, 1, GL_TRUE, value.m[0]));
    }

    void Shader::uniform(int loc, m3d::mat4x4 value)
    {
        glCall(glUniformMatrix4fv(loc, 1, GL_TRUE, value.m[0]));
    }

    void Shader::setEconst(std::string name, int value)
    {
        econst_ints[name] = value;
    }
}


