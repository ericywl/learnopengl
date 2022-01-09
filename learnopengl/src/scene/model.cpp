#include <assimp/postprocess.h>
#include <common.h>
#include <scene/model.h>

#include <assimp/Importer.hpp>
#include <stdexcept>

Model::Model(const std::string& filePath) : m_FilePath(filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Check for empty scene / empty root node / incomplete scene
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        spdlog::error("[Model Error] Import model error: {}", importer.GetErrorString());
        throw std::runtime_error("Failed to import model");
    }

    m_Directory = std::filesystem::path(filePath).parent_path();
    // Process root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::SetupDraw(Shader& shader) const {
    for (unsigned int i = 0; i < m_Meshes.size(); i++) {
        m_Meshes[i]->SetupDraw(shader);
    }
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Process any meshes in the node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }

    // Process each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    // Process vertices from mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        glm::vec2 texCoord(0.0f);
        // Check if mesh contains texture coordinates
        if (mesh->mTextureCoords[0]) {
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(Vertex{position, normal, texCoord});
    }

    // Process indices from mesh faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process mesh material texture
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        // Diffuse texture maps
        std::vector<std::shared_ptr<Texture>> diffuseMaps =
            loadMaterialTextures(mat, aiTextureType_DIFFUSE, TextureType::Diffuse);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // Specular textures maps
        std::vector<std::shared_ptr<Texture>> specularMaps =
            loadMaterialTextures(mat, aiTextureType_SPECULAR, TextureType::Specular);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return std::make_shared<Mesh>(vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                                  TextureType tType) {
    std::vector<std::shared_ptr<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string fStr = std::string(str.C_Str());

        std::unordered_map<std::string, std::shared_ptr<Texture>>::iterator it = m_LoadedTextures.find(fStr);
        if (it == m_LoadedTextures.end()) {
            // Texture not already loaded, initialize it
            m_LoadedTextures[fStr] = std::make_shared<Texture>((m_Directory / fStr).string(), tType);
        }

        // Push the loaded texture into return
        textures.push_back(m_LoadedTextures[fStr]);
    }

    return textures;
}
