#pragma once

class UniformBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    UniformBuffer(unsigned int size);
    ~UniformBuffer();

    void Bind() const;
    void Unbind() const;

    void BindRange(unsigned int offset, unsigned int size, unsigned int binding = 0) const;
    void InsertData(unsigned int offset, const void* data, unsigned int size) const;
};
