#pragma once

#include <vector>
#include <string>

namespace dgn
{
    typedef std::vector<Mesh> Model;

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
