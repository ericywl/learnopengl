#pragma once

#include <renderer/ibo.h>
#include <renderer/shader.h>
#include <renderer/vao.h>
#include <scene/mesh.h>

class Renderer {
   private:
   public:
    void Draw(const VertexArray& va, const IndexBuffer& ib) const;
    void Draw(const VertexArray& va, const unsigned int count) const;
    void Draw(const Mesh& mesh, Shader& shader) const;
    void Clear() const;

    void SetClearColor(const glm::vec4 color) const;
    void SetBlending(bool on) const;
    void SetLineMode(bool on) const;
    void SetDepthTest(bool on) const;
};
