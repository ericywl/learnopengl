#pragma once

#include <common.h>

#include <glm/glm.hpp>
#include <string>

enum class TextureMaxFilter { Nearest = GL_NEAREST, Linear = GL_LINEAR };

enum class TextureMinFilter {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
    NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
    LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

enum class TextureWrap {
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    Repeat = GL_REPEAT,
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE,
};

enum class TextureType {
    Texture,
    Diffuse,
    Specular,
    Normal,
    Height,
};

struct TextureOptions {
    TextureMinFilter MinFilter;
    TextureMaxFilter MaxFilter;
    TextureWrap WrapS;
    TextureWrap WrapT;
    glm::vec4 BorderColor;
};

const TextureOptions defaultOptions = {
    TextureMinFilter::NearestMipMapLinear, TextureMaxFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat,
    glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
};

class Texture {
   private:
    unsigned int m_ReferenceID;
    std::string m_FilePath;
    int m_Width, m_Height, m_BPP;
    TextureType m_Type;

   public:
    Texture(const std::string& filePath, const TextureType type = TextureType::Texture,
            const TextureOptions options = defaultOptions);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const {
        return m_Width;
    }

    inline int GetHeight() const {
        return m_Height;
    }

    inline TextureType GetType() const {
        return m_Type;
    }

    inline unsigned int GetReferenceID() const {
        return m_ReferenceID;
    }
};