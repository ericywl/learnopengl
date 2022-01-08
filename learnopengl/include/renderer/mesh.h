#pragma once

#include <common.h>
#include <renderer/ibo.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

class Mesh {
   private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture*> m_Textures;

    VertexBuffer* m_VBO;
    VertexArray* m_VAO;
    IndexBuffer* m_IBO;

   public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
    ~Mesh();

    void SetupDraw(Shader& shader) const;

    inline const VertexBuffer& GetVBO() const {
        return *m_VBO;
    }

    inline const VertexArray& GetVAO() const {
        return *m_VAO;
    }

    inline const IndexBuffer& GetIBO() const {
        return *m_IBO;
    }
};
