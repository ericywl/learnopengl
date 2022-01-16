#pragma once

#include <renderer/rbo.h>
#include <renderer/texture.h>

enum class AttachmentType {
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
};

enum class BufferValue {
    None = GL_NONE,
};

class FrameBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void AddColorAttachment(const Texture &tex, const unsigned int slot = 0, const int level = 0) const;
    void AddDepthAttachment(const Texture &tex, const int level = 0) const;
    void AddCubeDepthAttachment(const CubeMap &cm, const int level = 0) const;
    void AddRenderBufferAttachment(const AttachmentType type, const RenderBuffer &rb) const;

    void SetReadBuffer(const BufferValue val) const;
    void SetDrawBuffer(const BufferValue val) const;
    void SetReadAndDrawBuffer(const BufferValue val) const;
    bool IsComplete() const;
};
