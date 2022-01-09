#pragma once

#include <assimp/scene.h>
#include <scene/mesh.h>

#include <memory>

class Model {
   private:
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
    std::string m_Directory;

   public:
    Model(const std::string& filePath);

    void SetupDraw(Shader& shader) const;

   private:
    void processNode(aiNode* node);
};
