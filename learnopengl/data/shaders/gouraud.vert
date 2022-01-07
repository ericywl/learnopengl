#version 330 core
layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec4 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_TrInvModelView;
uniform vec4 u_LightPos;
uniform vec4 u_LightColor;

out vec4 v_Gouraud;
out vec2 v_TexCoord;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * a_Position;
    vec4 fragPos =  u_View * u_Model * a_Position;

	// Ambient lighting
    float ambientStrength = 0.1;
    vec4 ambient = ambientStrength * u_LightColor;

    // Diffuse lighting
    vec4 norm = normalize(u_TrInvModelView * a_Normal);
    vec4 lightDir = normalize((u_View * u_LightPos) - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * u_LightColor;

    // Specular lighting
    float specularStrength = 1.0;
    vec4 viewDir = normalize(-fragPos);
    vec4 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = specularStrength * spec * u_LightColor;

    v_Gouraud = ambient + diffuse + specular;
    v_TexCoord = a_TexCoord;
}
