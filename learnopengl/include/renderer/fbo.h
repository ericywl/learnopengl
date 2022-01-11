#pragma once

#include <renderer/rbo.h>
#include <renderer/texture.h>

enum class AttachmentType {
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
};

class FrameBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void AddTextureAttachment(const Texture &tex, const unsigned int slot = 0, const int level = 0) const;
    void AddRenderBufferAttachment(const AttachmentType type, const RenderBuffer &rb) const;
    bool IsComplete() const;
};
