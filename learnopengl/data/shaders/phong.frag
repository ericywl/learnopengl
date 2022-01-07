#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

struct SpotLight {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;
    float dropOff;
    float cutOff;
    float constant;
    float linear;
    float quadratic;
};

// Inputs from vertex shader
in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

// Camera position
uniform vec3 u_ViewPos;
// Fragment material
uniform Material u_Material;
// Lights
uniform DirectionalLight u_DirLight;
uniform PointLight u_PtLight;
uniform SpotLight u_SpLight;

// Fragment shader output
out vec4 fragColor;

vec3 CalcSpecular(vec3 dirToLight, vec3 lightSpecular, vec3 viewDir) {
    // Flip the direction to light and reflect against vertex normal to get the direction of light reflection
    vec3 reflectDir = reflect(-dirToLight, normalize(v_Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    return  lightSpecular * spec * vec3(texture(u_Material.specular, v_TexCoord));
}

vec3 CalcAmbient(vec3 lightAmbient) {
    return lightAmbient * vec3(texture(u_Material.diffuse, v_TexCoord));
}

vec3 CalcDiffuse(vec3 dirToLight, vec3 lightDiffuse, vec3 norm) {
    float diff = max(dot(norm, dirToLight), 0.0);

    return lightDiffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoord));
}

float CalcAttenuation(vec3 lightPos, float constant, float linear, float quadratic) {
    float dist = length(lightPos - v_FragPos);

    return 1.0 / (constant + linear * dist + quadratic * (dist * dist));
}

vec3 CalcPointLightContribution(PointLight ptLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(ptLight.position - v_FragPos);
    vec3 ambient = CalcAmbient(ptLight.ambient);
    vec3 diffuse = CalcDiffuse(dirToLight, ptLight.diffuse, norm);
    vec3 specular = CalcSpecular(dirToLight, ptLight.specular, viewDir);
    float attenuation = CalcAttenuation(ptLight.position, ptLight.constant, ptLight.linear, ptLight.quadratic);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcDirectionalLightContribution(DirectionalLight dLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(dLight.direction);

    vec3 ambient = CalcAmbient(dLight.ambient);
    vec3 diffuse = CalcDiffuse(dirToLight, dLight.diffuse, norm);
    vec3 specular = CalcSpecular(dirToLight, dLight.specular, viewDir);

    return ambient + diffuse + specular;
}

vec3 CalcSpotLightContribution(SpotLight spLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(spLight.position - v_FragPos);
    float theta = dot(dirToLight, -spLight.direction);
    float attenuation = CalcAttenuation(spLight.position, spLight.constant, spLight.linear, spLight.quadratic);

    if (theta < spLight.cutOff) {
        // Fragment is not in the spotlight
        // Return at least the ambient so that there is still some light
        return CalcAmbient(spLight.ambient) * attenuation;
    } 

    // Smoothen edge of spot light with drop-off
    float epsilon = spLight.dropOff - spLight.cutOff;
    float intensity = clamp((theta - spLight.cutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = CalcAmbient(spLight.ambient);
    vec3 diffuse = CalcDiffuse(dirToLight, spLight.diffuse, norm);
    vec3 specular = CalcSpecular(dirToLight, spLight.specular, viewDir);

    return (ambient + (diffuse * intensity) + (specular * intensity)) * attenuation;
}

void main() {
    vec3 norm = normalize(v_Normal);
    // Get view direction from camera position - fragment position since we are in world space
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

    vec3 ptLightContrib = CalcPointLightContribution(u_PtLight, norm, viewDir);
    vec3 dirLightContrib = CalcDirectionalLightContribution(u_DirLight, norm, viewDir);
    vec3 spLightContrib = CalcSpotLightContribution(u_SpLight, norm, viewDir);

    // Final result
    fragColor = vec4(dirLightContrib + spLightContrib + ptLightContrib, 1.0);
} 