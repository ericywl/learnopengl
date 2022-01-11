#pragma once

#include <common.h>

#include <glm/glm.hpp>
#include <string>

enum class TextureMaxFilter {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
};

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
    Attachment,
};

struct TextureOptions {
    TextureMinFilter MinFilter = TextureMinFilter::NearestMipMapLinear;
    TextureMaxFilter MaxFilter = TextureMaxFilter::Linear;
    TextureWrap WrapS = TextureWrap::Repeat;
    TextureWrap WrapT = TextureWrap::Repeat;
    TextureWrap WrapR = TextureWrap::Repeat;
    glm::vec4 BorderColor = glm::vec4{0.0f, 0.0f, 0.0f, 0.0f};
    bool GenerateMipMap = true;

    TextureOptions() {}

    TextureOptions(TextureMinFilter minF, TextureMaxFilter maxF, TextureWrap ws, TextureWrap wt, TextureWrap wr,
                   glm::vec4 bc, bool genMipMap) {
        MinFilter = minF;
        MaxFilter = maxF;
        WrapS = ws;
        WrapT = wt;
        WrapR = wr;
        BorderColor = bc;
        GenerateMipMap = genMipMap;
    }

    TextureOptions(TextureMinFilter minF, TextureMaxFilter maxF, TextureWrap ws, TextureWrap wt, glm::vec4 bc,
                   bool genMipMap) {
        MinFilter = minF;
        MaxFilter = maxF;
        WrapS = ws;
        WrapT = wt;
        BorderColor = bc;
        GenerateMipMap = genMipMap;
    }

    TextureOptions(TextureMinFilter minF, TextureMaxFilter maxF, TextureWrap ws, TextureWrap wt) {
        MinFilter = minF;
        MaxFilter = maxF;
        WrapS = ws;
        WrapT = wt;
    }

    TextureOptions(TextureMinFilter minF, TextureMaxFilter maxF, TextureWrap ws, TextureWrap wt, TextureWrap wr) {
        MinFilter = minF;
        MaxFilter = maxF;
        WrapS = ws;
        WrapT = wt;
        WrapR = wr;
    }

    TextureOptions(TextureWrap wrapS, TextureWrap wrapT, bool genMipMap = true) {
        WrapS = wrapS;
        WrapT = wrapT;
        GenerateMipMap = genMipMap;
    }

    TextureOptions(TextureMinFilter minF, TextureMaxFilter maxF, bool genMipMap = true) {
        MinFilter = minF;
        MaxFilter = maxF;
        GenerateMipMap = genMipMap;
    }
};

class Texture {
   private:
    unsigned int m_ReferenceID;
    std::string m_FilePath;
    int m_Width, m_Height, m_BPP;
    TextureType m_Type;

   public:
    Texture(const std::string& filePath, const TextureType type, const TextureOptions options);
    Texture(unsigned char* data, const unsigned int w, const unsigned int h, const unsigned int bpp,
            const TextureType type, const TextureOptions options);
    ~Texture();
    // Helper constructors
    Texture(const std::string& filePath) : Texture(filePath, TextureType::Texture, TextureOptions()) {}
    Texture(const std::string& filePath, const TextureOptions opts) : Texture(filePath, TextureType::Texture, opts) {}
    Texture(const std::string& filePath, const TextureType type) : Texture(filePath, type, TextureOptions()) {}

    void Bind(const unsigned int slot = 0, const bool activate = true) const;
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

   private:
    void init(unsigned char* data, const TextureOptions options);
};

class CubeMap {
   private:
    unsigned int m_ReferenceID;
    std::string m_FilePaths[6];

   public:
    CubeMap(const std::string filePaths[6], const TextureOptions options = TextureOptions());
    ~CubeMap();

    void Bind(const unsigned int slot = 0, const bool activate = true) const;
    void Unbind() const;
};