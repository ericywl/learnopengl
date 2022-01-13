#include <common.h>
#include <renderer/vao.h>

VertexArray::VertexArray() : m_ReferenceID(0), m_CurrCount(0) {
    glGenVertexArrays(1, &m_ReferenceID);
    glBindVertexArray(m_ReferenceID);
}

VertexArray::~VertexArray() {
    spdlog::debug("VertexArray {} destroyed", m_ReferenceID);
    glDeleteVertexArrays(1, &m_ReferenceID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, const bool instanced) {
    Bind();
    vb.Bind();

    const auto& elements = layout.GetElements();
    unsigned int size = (unsigned int)elements.size();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < size; i++) {
        const auto& element = elements[i];
        unsigned int idx = i + m_CurrCount;
        glEnableVertexAttribArray(idx);
        glVertexAttribPointer(idx, element.count, element.type, element.GetNormalizedByte(), layout.GetStride(),
                              (const void*)offset);
        offset += element.GetTotalSize();

        if (instanced) {
            glVertexAttribDivisor(idx, 1);
        }
    }

    m_CurrCount += size;
}

void VertexArray::Bind() const {
    glBindVertexArray(m_ReferenceID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}