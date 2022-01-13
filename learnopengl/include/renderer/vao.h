#pragma once

#include <common.h>
#include <renderer/vbo.h>

#include <glm/glm.hpp>
#include <vector>

struct VertexBufferElement {
    unsigned int count;
    unsigned int type;
    bool normalized;

    unsigned char GetNormalizedByte() const {
        return normalized ? GL_TRUE : GL_FALSE;
    }

    unsigned int GetSizeOfType() const {
        switch (type) {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }

        assert(false);
        return 0;
    }

    unsigned int GetTotalSize() const {
        return count * GetSizeOfType();
    }
};

class VertexBufferLayout {
   private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

   public:
    VertexBufferLayout() : m_Stride(0){};

    template <typename T>
    void Push(unsigned int count) {
        static_assert(false);
    }

    template <>
    void Push<float>(unsigned int count) {
        VertexBufferElement element = {count, GL_FLOAT, false};
        m_Elements.push_back(element);
        m_Stride += element.GetTotalSize();
    }

    template <>
    void Push<unsigned int>(unsigned int count) {
        VertexBufferElement element = {count, GL_UNSIGNED_INT, false};
        m_Elements.push_back(element);
        m_Stride += element.GetTotalSize();
    }

    template <>
    void Push<unsigned char>(unsigned int count) {
        VertexBufferElement element = {count, GL_BYTE, false};
        m_Elements.push_back(element);
        m_Stride += element.GetTotalSize();
    }

    inline const std::vector<VertexBufferElement> GetElements() const {
        return m_Elements;
    }

    inline const unsigned int GetStride() const {
        return m_Stride;
    }
};

class VertexArray {
   private:
    unsigned int m_ReferenceID;
    unsigned int m_CurrCount;

   public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, const bool instanced = false);
    void Bind() const;
    void Unbind() const;

    inline unsigned int GetReferenceID() {
        return m_ReferenceID;
    }
};
