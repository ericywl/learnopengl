#pragma once

#include <renderer/ibo.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

class Mesh {
   private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<std::shared_ptr<Texture>> m_Textures;

    std::shared_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<VertexArray> m_VAO;
    std::shared_ptr<IndexBuffer> m_IBO;

   public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<std::shared_ptr<Texture>> textures);

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
