#include <common.h>
#include <renderer/ubo.h>

UniformBuffer::UniformBuffer(unsigned int size) {
    glGenBuffers(1, &m_ReferenceID);
    Bind();
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &m_ReferenceID);
}

void UniformBuffer::Bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, m_ReferenceID);
}

void UniformBuffer::Unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::BindRange(unsigned int offset, unsigned int size, unsigned int binding) const {
    // For OpenGL versions < 4.2, we will need to do the following before calling this function:
    // unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.GetReferenceID(), "ExampleBlock");
    // glUniformBlockBinding(shader.GetReferenceID(), uniformBlockIndex, binding);
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_ReferenceID, offset, size);
}

void UniformBuffer::InsertData(unsigned int offset, const void* data, unsigned int size) const {
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}
