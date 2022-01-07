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
    float cutOff;
    float constant;
    float linear;
    float quadratic;
};

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform DirectionalLight u_DirLight;
uniform PointLight u_PtLight;
uniform SpotLight u_SpLight;

out vec4 fragColor;

vec3 calcSpecular(vec3 dirToLight, vec3 lightSpecular) {
    // Get view direction from camera position - fragment position since we are in world space
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    // Flip the direction to light and reflect against vertex normal to get the direction of light reflection
    vec3 reflectDir = reflect(-dirToLight, normalize(v_Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    return  lightSpecular * spec * vec3(texture(u_Material.specular, v_TexCoord));
}

vec3 calcAmbient(vec3 lightAmbient) {
    return lightAmbient * vec3(texture(u_Material.diffuse, v_TexCoord));
}

vec3 calcDiffuse(vec3 dirToLight, vec3 lightDiffuse) {
    vec3 norm = normalize(v_Normal);
    float diff = max(dot(norm, dirToLight), 0.0);

    return lightDiffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoord));
}

float calcAttenuation(vec3 lightPos, float constant, float linear, float quadratic) {
    float dist = length(lightPos - v_FragPos);
    float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

    return attenuation;
}

vec3 calcPointLightContribution(PointLight ptLight) {
    vec3 dirToLight = normalize(ptLight.position - v_FragPos);
    vec3 ambient = calcAmbient(ptLight.ambient);
    vec3 diffuse = calcDiffuse(dirToLight, ptLight.diffuse);
    vec3 specular = calcSpecular(dirToLight, ptLight.specular);
    float attenuation = calcAttenuation(ptLight.position, ptLight.constant, ptLight.linear, ptLight.quadratic);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcDirectionalLightContribution(DirectionalLight dLight) {
    vec3 dirToLight = normalize(dLight.direction);

    vec3 ambient = calcAmbient(dLight.ambient);
    vec3 diffuse = calcDiffuse(dirToLight, dLight.diffuse);
    vec3 specular = calcSpecular(dirToLight, dLight.specular);

    return ambient + diffuse + specular;
}

vec3 calcSpotLightContribution(SpotLight spLight) {
    vec3 dirToLight = normalize(spLight.position - v_FragPos);
    float theta = dot(dirToLight, -spLight.direction);

    if (theta < spLight.cutOff) {
        // Fragment is not in the spotlight
        return vec3(0.0);
    } 

	vec3 ambient = calcAmbient(spLight.ambient);
    vec3 diffuse = calcDiffuse(dirToLight, spLight.diffuse);
    vec3 specular = calcSpecular(dirToLight, spLight.specular);
    float attenuation = calcAttenuation(spLight.position, spLight.constant, spLight.linear, spLight.quadratic);

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    // Point lighting
    vec3 ptLightContrib = calcPointLightContribution(u_PtLight);
    vec3 dirLightContrib = calcDirectionalLightContribution(u_DirLight);
    vec3 spLightContrib = calcSpotLightContribution(u_SpLight);

    // Final result
    fragColor = vec4(spLightContrib, 1.0);
} 