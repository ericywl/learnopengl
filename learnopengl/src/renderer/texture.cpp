#include <renderer/texture.h>
#include <stb_image/stb_image.h>

Texture::Texture(const std::string& filePath, const TextureOptions options)
    : m_ReferenceID(0), m_FilePath(filePath), m_Width(0), m_Height(0), m_BPP(0) {
    // Flip the image since OpenGL expects image coordinates to start from bottom-left
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
    if (!data) {
        spdlog::error("[Texture Error] Texture '{}' failed to load", filePath);
        throw "Cannot load texture image";
    }

    glGenTextures(1, &m_ReferenceID);
    glBindTexture(GL_TEXTURE_2D, m_ReferenceID);

    // Texture minification and magnification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)options.MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)options.MaxFilter);

    // Wrap texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)options.WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)options.WrapT);

    // Set border color
    if (options.BorderColor) {
        float bc[] = {options.BorderColor->Red, options.BorderColor->Green, options.BorderColor->Blue,
                      options.BorderColor->Alpha};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);
    }

    // Create texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Generate mip-map
    if (options.MinFilter == TextureMinFilter::LinearMipMapLinear ||
        options.MinFilter == TextureMinFilter::NearestMipMapLinear ||
        options.MinFilter == TextureMinFilter::LinearMipMapNearest ||
        options.MinFilter == TextureMinFilter::NearestMipMapNearest) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Unbind texture and free the local buffer
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_ReferenceID);
}

void Texture::Bind(unsigned int slot) const {
    // Activate the texture in position specified by slot
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_ReferenceID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}