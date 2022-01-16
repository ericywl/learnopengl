#pragma once

#include <common.h>

#include <glm/glm.hpp>
#include <string>

enum class TextureMagFilter {
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
    TextureAttachment,
    DepthAttachment,
};

struct TextureOptions {
    TextureMinFilter MinFilter = TextureMinFilter::NearestMipMapLinear;
    TextureMagFilter MagFilter = TextureMagFilter::Linear;
    TextureWrap WrapS = TextureWrap::Repeat;
    TextureWrap WrapT = TextureWrap::Repeat;
    TextureWrap WrapR = TextureWrap::Repeat;
    bool GenerateMipMap = true;
    bool GammaCorrection = false;
    glm::vec4 BorderColor = glm::vec4(0.0f);

    TextureOptions() {}

    TextureOptions(TextureMinFilter minF, TextureMagFilter magF, TextureWrap ws, TextureWrap wt, TextureWrap wr,
                   bool genMipMap = true, bool gammaCorrect = false, glm::vec4 borderColor = glm::vec4(0.0f)) {
        MinFilter = minF;
        MagFilter = magF;
        WrapS = ws;
        WrapT = wt;
        WrapR = wr;
        GenerateMipMap = genMipMap;
        GammaCorrection = gammaCorrect;
        BorderColor = borderColor;
    }

    TextureOptions(TextureMinFilter minF, TextureMagFilter magF, TextureWrap ws, TextureWrap wt, bool genMipMap = true,
                   bool gammaCorrect = false, glm::vec4 borderColor = glm::vec4(0.0f)) {
        MinFilter = minF;
        MagFilter = magF;
        WrapS = ws;
        WrapT = wt;
        GenerateMipMap = genMipMap;
        GammaCorrection = gammaCorrect;
        BorderColor = borderColor;
    }

    TextureOptions(TextureWrap wrapS, TextureWrap wrapT, bool genMipMap = true, bool gammaCorrect = false,
                   glm::vec4 borderColor = glm::vec4(0.0f)) {
        WrapS = wrapS;
        WrapT = wrapT;
        GenerateMipMap = genMipMap;
        GammaCorrection = gammaCorrect;
        BorderColor = borderColor;
    }

    TextureOptions(TextureMinFilter minF, TextureMagFilter magF, bool genMipMap = true, bool gammaCorrect = false,
                   glm::vec4 borderColor = glm::vec4(0.0f)) {
        MinFilter = minF;
        MagFilter = magF;
        GenerateMipMap = genMipMap;
        GammaCorrection = gammaCorrect;
        BorderColor = borderColor;
    }

    TextureOptions(bool genMipMap, bool gammaCorrect) {
        GenerateMipMap = genMipMap;
        GammaCorrection = gammaCorrect;
    }
};

class Texture {
   private:
    unsigned int m_ReferenceID;
    std::string m_FilePath;
    int m_Width, m_Height, m_BPP;
    TextureType m_Type;

   public:
    Texture(const std::string& filePath, const TextureType type, const TextureOptions& options);
    Texture(const unsigned int w, const unsigned int h, const unsigned int bpp, const TextureType type,
            const TextureOptions& options);
    ~Texture();
    // Helper constructors
    Texture(const std::string& filePath) : Texture(filePath, TextureType::Texture, TextureOptions()) {}
    Texture(const std::string& filePath, const TextureOptions& opts) : Texture(filePath, TextureType::Texture, opts) {}
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
};

class CubeMap {
   private:
    unsigned int m_ReferenceID;
    std::string m_FilePaths[6];
    TextureType m_Type;

   public:
    CubeMap(const std::string filePaths[6], const TextureType = TextureType::Texture,
            const TextureOptions& options = TextureOptions());
    CubeMap(const unsigned int w, const unsigned int h, const TextureType type, const TextureOptions& options);
    ~CubeMap();
    // Helper constructors
    CubeMap(const std::string filePaths[6], const TextureOptions& options)
        : CubeMap(filePaths, TextureType::Texture, options) {}

    void Bind(const unsigned int slot = 0, const bool activate = true) const;
    void Unbind() const;

    inline TextureType GetType() const {
        return m_Type;
    }

    inline unsigned int GetReferenceID() const {
        return m_ReferenceID;
    }
};
