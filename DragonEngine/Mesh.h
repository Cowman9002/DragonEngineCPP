#pragma once

#include <vector>
#include <string>

namespace m3d
{
    class vec3;
}

namespace dgn
{
    class Mesh
    {
        friend class Renderer;
    public:
        Mesh();
        virtual ~Mesh();
        void dispose();

        Mesh& createFromData(const std::vector<float>& vertex_data, const std::vector<unsigned>& index_data);
        Mesh& createFromData(const std::vector<unsigned>& index_data);
        Mesh& setVertexSize(unsigned size);
        Mesh& addVertexAttrib(unsigned location, int size);
        Mesh& complete();

        static std::vector<Mesh> loadFromFile(std::string filepath);

        /**
            Returns a vector of all positions of vertices loaded from given filepath
        */
        static std::vector<m3d::vec3> loadVertices(std::string filepath);

    private:
        unsigned m_vao;
        unsigned m_vbo;
        unsigned m_ibo;
        unsigned m_length;

        unsigned vert_size;
        unsigned vert_offsets;

        bool m_disposed;
    };

    typedef std::vector<Mesh> Model;
}
