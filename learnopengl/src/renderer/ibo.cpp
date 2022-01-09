#include <common.h>
#include <renderer/ibo.h>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_ReferenceID(0), m_Count(count) {
    glGenBuffers(1, &m_ReferenceID);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(GLuint), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    spdlog::debug("IndexBuffer {} destroyed", m_ReferenceID);
    glDeleteBuffers(1, &m_ReferenceID);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ReferenceID);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}