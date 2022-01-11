#pragma once

enum class RenderBufferType : int {
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,
};

class RenderBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    RenderBuffer(const RenderBufferType type, const unsigned int width, const unsigned int height);
    ~RenderBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetReferenceID() const {
        return m_ReferenceID;
    }
};
