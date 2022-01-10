#pragma once

#include <common.h>
#include <renderer/texture.h>

class FrameBuffer {
   private:
    unsigned int m_ReferenceID;

   public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void AddTextureAttachment(const Texture &tex, const unsigned int slot = 0, const int level = 0) const;
};
