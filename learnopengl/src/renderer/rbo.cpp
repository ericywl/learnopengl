#include <common.h>
#include <renderer/rbo.h>

RenderBuffer::RenderBuffer(const RenderBufferType type, const unsigned int width, const unsigned int height)
    : m_ReferenceID(0) {
    glGenRenderbuffers(1, &m_ReferenceID);
    Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(type), width, height);
}

RenderBuffer::~RenderBuffer() {
    spdlog::debug("RenderBuffer {} destroyed", m_ReferenceID);
    glDeleteRenderbuffers(1, &m_ReferenceID);
}

void RenderBuffer::Bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, m_ReferenceID);
}

void RenderBuffer::Unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
