#pragma once

#include <common.h>

class VertexBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetReferenceID() {
        return m_ReferenceID;
    }
};
