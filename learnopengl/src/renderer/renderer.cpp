#include <common.h>
#include <renderer/renderer.h>

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib) const {
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(const VertexArray& va, const unsigned int count) const {
    va.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void Renderer::Draw(const Mesh& mesh, Shader& shader) const {
    mesh.SetupDraw(shader);
    Draw(mesh.GetVAO(), mesh.GetIBO());
}

void Renderer::Clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetClearColor(const glm::vec4 color) const {
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::SetBlending(bool on) const {
    if (on) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void Renderer::SetLineMode(bool on) const {
    if (on) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::SetDepthTest(bool on) const {
    if (on) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}