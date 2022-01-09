#pragma once

#include <assimp/scene.h>
#include <scene/mesh.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

class Model {
   private:
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_LoadedTextures;
    std::string m_FilePath;
    std::filesystem::path m_Directory;

   public:
    Model(const std::string& filePath);

    void SetupDraw(Shader& shader) const;

   private:
    void processNode(aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType tType);
};
