#pragma once

#include <renderer/ibo.h>
#include <renderer/shader.h>
#include <renderer/vao.h>
#include <scene/mesh.h>
#include <scene/model.h>

enum class TestFunc : int {
    Always = GL_ALWAYS,
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    LessEqual = GL_LEQUAL,
    GreaterEqual = GL_GEQUAL,
    Greater = GL_GREATER,
    NotEqual = GL_NOTEQUAL,
};

enum class ClearBit : int {
    Color = GL_COLOR_BUFFER_BIT,
    Depth = GL_DEPTH_BUFFER_BIT,
    Stencil = GL_STENCIL_BUFFER_BIT,
    All = Color | Depth | Stencil,
};

inline ClearBit operator|(const ClearBit& a, const ClearBit& b) {
    return static_cast<ClearBit>(static_cast<int>(a) | static_cast<int>(b));
}

enum class TestAction {
    Keep = GL_KEEP,
    Replace = GL_REPLACE,
    Increment = GL_INCR,
    IncrementWrap = GL_INCR_WRAP,
    Decrement = GL_DECR,
    DecrementWrap = GL_DECR_WRAP,
    Invert = GL_INVERT,
};

enum class CulledFace {
    Front = GL_FRONT,
    Back = GL_BACK,
    FrontBack = GL_FRONT_AND_BACK,
};

enum class FaceWinding {
    Clockwise = GL_CW,
    CounterClockwise = GL_CCW,
};

class Renderer {
   private:
   public:
    void Draw(const VertexArray& va, const IndexBuffer& ib) const;
    void Draw(const VertexArray& va, const unsigned int count) const;
    void Draw(const Mesh& mesh, Shader& shader) const;
    void Draw(const Model& model, Shader& shader) const;
    void DrawInstanced(const VertexArray& va, const IndexBuffer& ib, const unsigned int instances) const;
    void DrawInstanced(const VertexArray& va, const unsigned int count, const unsigned int instances) const;
    void DrawInstanced(const Mesh& mesh, Shader& shader, const unsigned int instances) const;
    void DrawInstanced(const Model& model, Shader& shader, const unsigned int instances) const;
    void Clear(ClearBit cb = ClearBit::All) const;

    void SetClearColor(const glm::vec4 color) const;
    void SetClearColor(const float r, const float g, const float b, const float a) const;
    void SetBlending(bool on) const;
    void SetLineMode(bool on) const;
    void SetMSAA(bool on) const;
    void SetGammaCorrection(bool on) const;
    // Depth testing
    void SetDepthTest(bool on) const;
    void SetDepthMask(bool on) const;
    void SetDepthFunc(TestFunc fn) const;
    // Stencil testing
    void SetStencilTest(bool on) const;
    void SetStencilMask(unsigned int mask) const;
    void SetStencilFunc(TestFunc fn, int ref, unsigned int mask) const;
    void SetStencilAction(TestAction stencilFail, TestAction depthFail, TestAction bothPass) const;
    // Face culling
    void SetFaceCulling(bool on) const;
    void SetCulledFace(CulledFace f) const;
    void SetFrontFaceWinding(FaceWinding w) const;
};
