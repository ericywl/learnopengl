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

void Renderer::Draw(const Model& model, Shader& shader) const {
    model.SetupDraw(shader);
    for (std::shared_ptr<Mesh> mesh : model.GetMeshes()) {
        Draw(*mesh, shader);
    }
}

void Renderer::Clear(ClearBit cb) const {
    glClear(static_cast<GLbitfield>(cb));
}

void Renderer::SetClearColor(const glm::vec4 color) const {
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a) const {
    glClearColor(r, g, b, a);
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

void Renderer::SetDepthMask(bool on) const {
    glDepthMask(on ? GL_TRUE : GL_FALSE);
}

void Renderer::SetDepthFunc(TestFunc fn) const {
    glDepthFunc(static_cast<GLenum>(fn));
}

void Renderer::SetStencilTest(bool on) const {
    if (on) {
        glEnable(GL_STENCIL_TEST);
    } else {
        glDisable(GL_STENCIL_TEST);
    }
}

void Renderer::SetStencilMask(unsigned int mask) const {
    glStencilMask(mask);
}

void Renderer::SetStencilFunc(TestFunc fn, int ref, unsigned int mask) const {
    glStencilFunc(static_cast<GLenum>(fn), ref, mask);
}

void Renderer::SetStencilAction(TestAction stencilFail, TestAction depthFail, TestAction bothPass) const {
    glStencilOp(static_cast<GLenum>(stencilFail), static_cast<GLenum>(depthFail), static_cast<GLenum>(bothPass));
}

void Renderer::SetFaceCulling(bool on) const {
    if (on) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::SetCulledFace(CulledFace f) const {
    glCullFace(static_cast<GLenum>(f));
}

void Renderer::SetFrontFaceWinding(FaceWinding w) const {
    glFrontFace(static_cast<GLenum>(w));
}
