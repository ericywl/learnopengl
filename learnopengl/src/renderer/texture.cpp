#include <common.h>
#include <renderer/texture.h>
#include <stb_image/stb_image.h>

#include <filesystem>

struct X {
    GLenum internalFormat, externalFormat, dataType;
};

X texInit(const GLenum target, unsigned int* referenceID, const TextureType type, const TextureOptions& options) {
    glGenTextures(1, referenceID);
    glBindTexture(target, *referenceID);

    // Texture minification and magnification filters
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (GLint)options.MinFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, (GLint)options.MagFilter);

    // Wrap texture coordinates
    glTexParameteri(target, GL_TEXTURE_WRAP_S, (GLint)options.WrapS);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, (GLint)options.WrapT);
    if (target == GL_TEXTURE_CUBE_MAP) {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, (GLint)options.WrapR);
    }

    if (options.BorderColor != glm::vec4(0.0f)) {
        float borderColor[] = {options.BorderColor.r, options.BorderColor.g, options.BorderColor.b,
                               options.BorderColor.a};
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    // Customize formats and data types
    GLenum interFormat = options.GammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    GLenum exterFormat = GL_RGBA;
    GLenum dataType = GL_UNSIGNED_BYTE;
    if (type == TextureType::DepthAttachment) {
        interFormat = GL_DEPTH_COMPONENT;
        exterFormat = GL_DEPTH_COMPONENT;
        dataType = GL_FLOAT;
    }

    return {interFormat, exterFormat, dataType};
}

Texture::Texture(const unsigned int w, const unsigned int h, const unsigned int bpp, const TextureType type,
                 const TextureOptions& options)
    : m_ReferenceID(0), m_FilePath(""), m_Width(w), m_Height(h), m_BPP(bpp), m_Type(type) {
    X v = texInit(GL_TEXTURE_2D, &m_ReferenceID, type, options);

    // Create texture
    glTexImage2D(GL_TEXTURE_2D, 0, v.internalFormat, m_Width, m_Height, 0, v.externalFormat, v.dataType, nullptr);

    // Generate mip-map
    if (options.GenerateMipMap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    Unbind();
}

Texture::Texture(const std::string& filePath, const TextureType type, const TextureOptions& options)
    : m_ReferenceID(0), m_FilePath(filePath), m_Width(0), m_Height(0), m_BPP(0), m_Type(type) {
    // Flip the image since OpenGL expects image coordinates to start from bottom-left
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, STBI_rgb_alpha);
    if (!data) {
        spdlog::error("[Texture Error] Texture '{}' failed to load", filePath);
        stbi_image_free(data);
        throw "Cannot load texture image";
    }

    X v = texInit(GL_TEXTURE_2D, &m_ReferenceID, type, options);

    // Create texture
    glTexImage2D(GL_TEXTURE_2D, 0, v.internalFormat, m_Width, m_Height, 0, v.externalFormat, v.dataType, data);

    // Generate mip-map
    if (options.GenerateMipMap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Unbind texture and free data buffer
    Unbind();
    stbi_image_free(data);
}

Texture::~Texture() {
    spdlog::debug("Texture {} destroyed", m_ReferenceID);
    glDeleteTextures(1, &m_ReferenceID);
}

void Texture::Bind(const unsigned int slot, const bool activate) const {
    if (activate) {
        // Activate the texture in position specified by slot
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    glBindTexture(GL_TEXTURE_2D, m_ReferenceID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

CubeMap::CubeMap(const std::string filePaths[6], const TextureType type, const TextureOptions& options) : m_Type(type) {
    X v = texInit(GL_TEXTURE_CUBE_MAP, &m_ReferenceID, type, options);

    int width, height, bpp;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(filePaths[i].c_str(), &width, &height, &bpp, STBI_rgb_alpha);
        if (!data) {
            spdlog::error("[Texture Error] CubeMap '{}' failed to load", filePaths[i]);
            stbi_image_free(data);
            throw "Cannot load texture image";
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, v.internalFormat, width, height, 0, v.externalFormat,
                     v.dataType, data);
        stbi_image_free(data);
    }

    if (options.GenerateMipMap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    Unbind();
}

CubeMap::CubeMap(const unsigned int w, const unsigned int h, const TextureType type, const TextureOptions& options) {
    X v = texInit(GL_TEXTURE_CUBE_MAP, &m_ReferenceID, type, options);

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, v.internalFormat, w, h, 0, v.externalFormat, v.dataType,
                     nullptr);
    }

    if (options.GenerateMipMap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    Unbind();
}

CubeMap::~CubeMap() {
    spdlog::debug("CubeMap {} destroyed", m_ReferenceID);
    glDeleteTextures(1, &m_ReferenceID);
}

void CubeMap::Bind(const unsigned int slot, const bool activate) const {
    if (activate) {
        // Activate the texture in position specified by slot
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ReferenceID);
}

void CubeMap::Unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
