#pragma once

#include <renderer/shader.h>

#include <glm/glm.hpp>

struct BasicLight {
    glm::vec3 Color;
    float AmbientIntensity, DiffuseIntensity, SpecularIntensity;

    inline glm::vec3 Ambient() const {
        return Color * AmbientIntensity;
    }

    inline glm::vec3 Diffuse() const {
        return Color * DiffuseIntensity;
    }

    inline glm::vec3 Specular() const {
        return Color * SpecularIntensity;
    }
};

struct Attenuation {
    float Constant, Linear, Quadratic;
};

struct PointLight {
    BasicLight Inner;
    glm::vec3 Position;
    Attenuation Atten;
};

struct DirectionalLight {
    BasicLight Inner;
    glm::vec3 Direction;
};

struct SpotLight {
    BasicLight Inner;
    glm::vec3 Position;
    glm::vec3 Direction;
    Attenuation Atten;
    float DropOff, CutOff;
};

class Lighting {
   public:
    static void SetDirectionalLight(Shader& shader, const std::string& name, const DirectionalLight& directLight);
    static void SetPointLights(Shader& shader, const std::string& numName, const unsigned int numLights,
                               const std::string& name, const PointLight* ptLights);
    static void SetSpotLights(Shader& shader, const std::string& numName, const unsigned int numLights,
                              const std::string& name, const SpotLight* spLights);
};