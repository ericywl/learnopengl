#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_InvTModel;
uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;

out vec3 v_Gouraud;
out vec2 v_TexCoord;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    vec3 fragPos = vec3(u_Model * vec4(a_Position, 1.0));

	// Ambient lighting
    vec3 ambient = u_Light.ambient * u_Material.ambient;

    // Diffuse lighting
    vec3 norm = normalize(mat3(u_InvTModel) * a_Normal);
    vec3 lightDir = normalize(u_Light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * diff * u_Material.diffuse;

    // Specular lighting
    float specularStrength = 1.0;
    vec3 viewDir = normalize(u_ViewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec3 specular = u_Light.specular * spec * u_Material.specular;

    v_Gouraud = ambient + diffuse + specular;
    v_TexCoord = a_TexCoord;
}
