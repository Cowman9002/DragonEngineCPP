#include "Mesh.h"
#include "d_internal.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>

namespace dgn
{
    Mesh::Mesh() : m_vao(0), m_vbo(0), m_ibo(0), m_length(0), vert_size(0), vert_offsets(0)
    {}

    Mesh::~Mesh()
    {
    }

    void Mesh::dispose()
    {
        if(m_disposed) return;

        glCall(glDeleteVertexArrays(1, &m_vao));
        glCall(glDeleteBuffers(1, &m_vbo));
        glCall(glDeleteBuffers(1, &m_ibo));
        m_length = 0;
        m_disposed = true;
    }

    Mesh& Mesh::createFromData(const std::vector<float>& vertex_data, const std::vector<unsigned>& index_data)
    {
        glCall(glGenVertexArrays(1, &m_vao));
        glCall(glGenBuffers(1, &m_vbo));
        glCall(glGenBuffers(1, &m_ibo));

        glCall(glBindVertexArray(m_vao));

        // -------- Index Data
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
        glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(unsigned), index_data.data(), GL_STATIC_DRAW));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        // -------- Vertex Data
        glCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        glCall(glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW));

        m_length = index_data.size();

        return *this;
    }

    Mesh& Mesh::createFromData(const std::vector<unsigned>& index_data)
    {
        glCall(glGenVertexArrays(1, &m_vao));
        glCall(glGenBuffers(1, &m_vbo));
        glCall(glGenBuffers(1, &m_ibo));

        glCall(glBindVertexArray(m_vao));

        // -------- Index Data
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
        glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(unsigned), index_data.data(), GL_STATIC_DRAW));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        // -------- Vertex Data
        glCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        glCall(glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW));

        m_length = index_data.size();

        return *this;
    }

    Mesh& Mesh::setVertexSize(unsigned size)
    {
        vert_size = size * sizeof(float);
        return *this;
    }

    Mesh& Mesh::addVertexAttrib(unsigned location, int size)
    {
        glCall(glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, vert_size, (void*)vert_offsets));
        glCall(glEnableVertexAttribArray(location));

        vert_offsets += size * sizeof(float);

        return *this;
    }

    Mesh& Mesh::complete()
    {
        glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        glCall(glBindVertexArray(0));

        m_disposed = false;

        return *this;
    }

    ////////////////////////////////////////
    //          MODEL LOADING             //
    ////////////////////////////////////////

    Mesh aiMeshConvert(struct aiMesh* mesh);
    std::vector<Mesh> Mesh::loadFromFile(std::string filepath)
    {
        std::vector<Mesh> res;

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // If the import failed, report it
        if(!scene)
        {
            logError("MESH LOADING", importer.GetErrorString());
            return res;
        }

        // Now we can access the file's contents
        for(unsigned i = 0; i < scene->mNumMeshes; i++)
        {
            res.push_back(aiMeshConvert(scene->mMeshes[i]));
        }

        // We're done. Release all resources associated with this import
        importer.FreeScene();
        return res;
    }

    Mesh aiMeshConvert(struct aiMesh* mesh)
    {
        unsigned char single_vertex_size = 0;
        bool attributes[4];
        unsigned sizes[4] = {3, 2, 3, 3};
        if(mesh->mVertices)
        {
            single_vertex_size += 3;
            attributes[0] = true;
        }

        if(mesh->mTextureCoords[0])
        {
            single_vertex_size += 2;
            attributes[1] = true;
        }

        if(mesh->mNormals)
        {
            single_vertex_size += 3;
            attributes[2] = true;
        }

        if(mesh->mTangents)
        {
            single_vertex_size += 3;
            attributes[3] = true;
        }


        std::vector<float> vertices;
        std::vector<unsigned> indices;

        for(uint32_t v = 0; v < mesh->mNumVertices; v++)
        {
            if(attributes[0])
            {
                vertices.push_back(mesh->mVertices[v].x);
                vertices.push_back(mesh->mVertices[v].y);
                vertices.push_back(mesh->mVertices[v].z);
            }

            if(attributes[1])
            {
                vertices.push_back(mesh->mTextureCoords[0][v].x);
                vertices.push_back(mesh->mTextureCoords[0][v].y);
            }

            if(attributes[2])
            {
                vertices.push_back(mesh->mNormals[v].x);
                vertices.push_back(mesh->mNormals[v].y);
                vertices.push_back(mesh->mNormals[v].z);
            }

            if(attributes[3])
            {
                vertices.push_back(mesh->mTangents[v].x);
                vertices.push_back(mesh->mTangents[v].y);
                vertices.push_back(mesh->mTangents[v].z);
            }
        }

        for(uint32_t f = 0; f < mesh->mNumFaces; f++)
        {
            struct aiFace face = mesh->mFaces[f];
            for(unsigned i = 0; i < face.mNumIndices; i++)
            {
                indices.push_back(face.mIndices[i]);
            }
        }

        Mesh m = Mesh().createFromData(vertices, indices);
        m.setVertexSize(single_vertex_size);

        for(int i = 0; i < 4; i++)
        {
            if(attributes[i])
                m.addVertexAttrib(i, sizes[i]);
        }

        m.complete();

        return m;
    }
}
