#include <renderer/vbo.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size) : m_ReferenceID(0) {
    glGenBuffers(1, &m_ReferenceID);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_ReferenceID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_ReferenceID);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
