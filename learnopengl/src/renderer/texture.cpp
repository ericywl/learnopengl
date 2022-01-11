#include <common.h>
#include <renderer/texture.h>
#include <stb_image/stb_image.h>

#include <filesystem>

Texture::Texture(unsigned char* data, const unsigned int w, const unsigned int h, const unsigned int bpp,
                 const TextureType type, const TextureOptions options)
    : m_ReferenceID(0), m_FilePath(""), m_Width(w), m_Height(h), m_BPP(bpp), m_Type(type) {
    init(data, options);
}

Texture::Texture(const std::string& filePath, const TextureType type, const TextureOptions options)
    : m_ReferenceID(0), m_FilePath(filePath), m_Width(0), m_Height(0), m_BPP(0), m_Type(type) {
    // Flip the image since OpenGL expects image coordinates to start from bottom-left
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, STBI_rgb_alpha);
    if (!data) {
        spdlog::error("[Texture Error] Texture '{}' failed to load", filePath);
        stbi_image_free(data);
        throw "Cannot load texture image";
    }

    init(data, options);
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

void Texture::init(unsigned char* data, const TextureOptions options) {
    glGenTextures(1, &m_ReferenceID);
    glBindTexture(GL_TEXTURE_2D, m_ReferenceID);

    // Texture minification and magnification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)options.MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)options.MaxFilter);

    // Wrap texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)options.WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)options.WrapT);

    // Set border color
    if (options.BorderColor.a > 0.0f) {
        float bc[] = {options.BorderColor.r, options.BorderColor.g, options.BorderColor.b, options.BorderColor.a};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);
    }

    // Create texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Generate mip-map
    if (options.GenerateMipMap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Unbind texture and free the local buffer
    glBindTexture(GL_TEXTURE_2D, 0);
}

CubeMap::CubeMap(const std::string filePaths[6], const TextureOptions options) {
    glGenTextures(1, &m_ReferenceID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ReferenceID);

    int width, height, bpp;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(filePaths[i].c_str(), &width, &height, &bpp, STBI_rgb_alpha);
        if (!data) {
            spdlog::error("[Texture Error] CubeMap '{}' failed to load", filePaths[i]);
            stbi_image_free(data);
            throw "Cannot load texture image";
        }

        // Texture minification and magnification filters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (GLint)options.MinFilter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (GLint)options.MaxFilter);

        // Wrap texture coordinates
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (GLint)options.WrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (GLint)options.WrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint)options.WrapR);

        // Set border color
        if (options.BorderColor.a > 0.0f) {
            float bc[] = {options.BorderColor.r, options.BorderColor.g, options.BorderColor.b, options.BorderColor.a};
            glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, bc);
        }

        // Generate mip-map
        if (options.GenerateMipMap) {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
    }

    // Unbind texture and free the local buffer
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap() {
    spdlog::debug("Texture {} destroyed", m_ReferenceID);
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