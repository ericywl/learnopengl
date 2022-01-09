#include <common.h>
#include <renderer/vao.h>

VertexArray::VertexArray() : m_ReferenceID(0) {
    glGenVertexArrays(1, &m_ReferenceID);
    glBindVertexArray(m_ReferenceID);
}

VertexArray::~VertexArray() {
    spdlog::debug("VertexArray {} destroyed", m_ReferenceID);
    glDeleteVertexArrays(1, &m_ReferenceID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
    Bind();
    vb.Bind();

    const auto& elements = layout.GetElements();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.GetNormalizedByte(), layout.GetStride(),
                              (const void*)offset);
        offset += element.count * element.GetSizeOfType();
    }
}

void VertexArray::Bind() const {
    glBindVertexArray(m_ReferenceID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}