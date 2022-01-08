#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct BasicLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    BasicLight inner;
    vec3 position;
    Attenuation atten;
};

struct DirectionalLight {
    BasicLight inner;
    vec3 direction;
};

struct SpotLight {
	BasicLight inner;
    vec3 position;
    vec3 direction;
    float dropOff;
    float cutOff;
    Attenuation atten;
};

// Inputs from vertex shader
in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

// Camera position
uniform vec3 u_ViewPos;
// Fragment material
uniform Material u_Material;
// Lights
uniform int u_NumPtLights;
uniform int u_NumSpLights;
uniform DirectionalLight u_DirLight;
uniform PointLight u_PtLights[MAX_POINT_LIGHTS];
uniform SpotLight u_SpLights[MAX_SPOT_LIGHTS];

// Fragment shader output
out vec4 fragColor;

vec3 CalcBasicLight(BasicLight innerLight, vec3 dirToLight, vec3 norm, vec3 viewDir) {
    // Ambient
    vec3 ambientLight = innerLight.ambient * vec3(texture(u_Material.diffuse, v_TexCoord));

    // Diffuse
    float diff = max(dot(norm, dirToLight), 0.0);
    vec3 diffuseLight = innerLight.diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoord));

    // Specular
    // Flip the direction to light and reflect against vertex normal to get the direction of light reflection
    vec3 reflectDir = reflect(-dirToLight, normalize(v_Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec3 specularLight = innerLight.specular * spec * vec3(texture(u_Material.specular, v_TexCoord));

    return ambientLight + diffuseLight + specularLight;
}

float CalcAttenuation(vec3 lightPos, Attenuation atten) {
    float dist = length(lightPos - v_FragPos);

    return 1.0 / (atten.constant + atten.linear * dist + atten.quadratic * (dist * dist));
}

vec3 CalcPointLightContribution(PointLight ptLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(ptLight.position - v_FragPos);
    vec3 light = CalcBasicLight(ptLight.inner, dirToLight, norm, viewDir);
    float attenuation = CalcAttenuation(ptLight.position, ptLight.atten);

    return light * attenuation;
}

vec3 CalcDirectionalLightContribution(DirectionalLight dLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(dLight.direction);

    return CalcBasicLight(dLight.inner, dirToLight, norm, viewDir);
}

vec3 CalcSpotLightContribution(SpotLight spLight, vec3 norm, vec3 viewDir) {
    vec3 dirToLight = normalize(spLight.position - v_FragPos);
    float theta = dot(dirToLight, -spLight.direction);
    float attenuation = CalcAttenuation(spLight.position, spLight.atten);

    if (theta < spLight.cutOff) {
        // Fragment is not in the spotlight
        return vec3(0.0);
    } 

    // Smoothen edge of spotlight with drop-off
    float epsilon = spLight.dropOff - spLight.cutOff;
    float intensity = clamp((theta - spLight.cutOff) / epsilon, 0.0, 1.0);

    return CalcBasicLight(spLight.inner, dirToLight, norm, viewDir) * intensity * attenuation;
}

void main() {
    vec3 norm = normalize(v_Normal);
    // Get view direction from camera position - fragment position since we are in world space
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

    // Calculate contribution from all light sources
    vec3 light = CalcDirectionalLightContribution(u_DirLight, norm, viewDir);
    for (int i = 0; i < u_NumPtLights; i++) {
		light += CalcPointLightContribution(u_PtLights[i], norm, viewDir);
    }
    for (int i = 0; i < u_NumSpLights; i++) {
        light += CalcSpotLightContribution(u_SpLights[i], norm, viewDir);
    }

    // Final result
    fragColor = vec4(light, 1.0);
} 