#include <common.h>
#include <renderer/fbo.h>

FrameBuffer::FrameBuffer() : m_ReferenceID(0) {
    glGenFramebuffers(1, &m_ReferenceID);
    Bind();
}

FrameBuffer::~FrameBuffer() {
    spdlog::debug("FrameBuffer {} destroyed", m_ReferenceID);
    glDeleteFramebuffers(1, &m_ReferenceID);
}

void FrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_ReferenceID);
}

void FrameBuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AddTextureAttachment(const Texture& tex, const unsigned int slot, const int level) const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, tex.GetReferenceID(), level);
}

void FrameBuffer::AddRenderBufferAttachment(const AttachmentType type, const RenderBuffer& rb) const {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<GLenum>(type), GL_RENDERBUFFER, rb.GetReferenceID());
}

bool FrameBuffer::IsComplete() const {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
