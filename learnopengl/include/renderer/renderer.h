#pragma once

#include <common.h>
#include <renderer/ibo.h>
#include <renderer/vao.h>

class Renderer {
   private:
   public:
    void Draw(const VertexArray& va, const IndexBuffer& ib) const;
    void Draw(const VertexArray& va, const unsigned int count) const;
    void Clear() const;

    void SetClearColor(const Color& color) const;
    void SetBlending(bool on) const;
    void SetLineMode(bool on) const;
    void SetDepthTest(bool on) const;
};
