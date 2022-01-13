#include <renderer/light.h>

void Lighting::SetDirectionalLight(Shader& shader, const std::string& name, const DirectionalLight& directLight) {
    shader.SetUniform1i("u_EnableDirLight", 1);
    shader.SetUniform3f(name + ".inner.ambient", directLight.Inner.Ambient());
    shader.SetUniform3f(name + ".inner.diffuse", directLight.Inner.Diffuse());
    shader.SetUniform3f(name + ".inner.specular", directLight.Inner.Specular());
    shader.SetUniform3f(name + ".direction", directLight.Direction);
}

void Lighting::SetPointLights(Shader& shader, const std::string& numName, const unsigned int numLights,
                              const std::string& name, const PointLight* ptLights) {
    shader.SetUniform1i(numName, numLights);

    for (unsigned int i = 0; i < numLights; i++) {
        std::string indexedName = name + "[" + std::to_string(i) + "]";
        shader.SetUniform3f(indexedName + ".inner.ambient", ptLights[i].Inner.Ambient());
        shader.SetUniform3f(indexedName + ".inner.diffuse", ptLights[i].Inner.Diffuse());
        shader.SetUniform3f(indexedName + ".inner.specular", ptLights[i].Inner.Specular());

        shader.SetUniform3f(indexedName + ".position", ptLights[i].Position);
        shader.SetUniform1f(indexedName + ".atten.constant", ptLights[i].Atten.Constant);
        shader.SetUniform1f(indexedName + ".atten.linear", ptLights[i].Atten.Linear);
        shader.SetUniform1f(indexedName + ".atten.quadratic", ptLights[i].Atten.Quadratic);
    }
}

void Lighting::SetSpotLights(Shader& shader, const std::string& numName, const unsigned int numLights,
                             const std::string& name, const SpotLight* spLights) {
    shader.SetUniform1i(numName, numLights);

    for (unsigned int i = 0; i < numLights; i++) {
        std::string indexedName = name + "[" + std::to_string(i) + "]";
        shader.SetUniform3f(indexedName + ".inner.ambient", spLights[i].Inner.Ambient());
        shader.SetUniform3f(indexedName + ".inner.diffuse", spLights[i].Inner.Diffuse());
        shader.SetUniform3f(indexedName + ".inner.specular", spLights[i].Inner.Specular());

        shader.SetUniform3f(indexedName + ".position", spLights[i].Position);
        shader.SetUniform3f(indexedName + ".direction", spLights[i].Direction);
        shader.SetUniform1f(indexedName + ".atten.constant", spLights[i].Atten.Constant);
        shader.SetUniform1f(indexedName + ".atten.linear", spLights[i].Atten.Linear);
        shader.SetUniform1f(indexedName + ".atten.quadratic", spLights[i].Atten.Quadratic);
        shader.SetUniform1f(indexedName + ".dropOff", spLights[i].DropOff);
        shader.SetUniform1f(indexedName + ".cutOff", spLights[i].CutOff);
    }
}
