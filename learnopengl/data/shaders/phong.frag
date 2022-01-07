#version 330 core
in vec4 v_FragPos;
in vec4 v_Normal;
in vec4 v_LightPos;
in vec2 v_TexCoord;

uniform vec4 u_ObjColor;
uniform vec4 u_LightColor;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;

out vec4 fragColor;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.1;
    vec4 ambient = ambientStrength * u_LightColor;

    // Diffuse lighting
    vec4 norm = normalize(v_Normal);
    vec4 lightDir = normalize(v_LightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * u_LightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec4 viewDir = normalize(-v_FragPos);
    vec4 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = specularStrength * spec * u_LightColor;

    // Final result
    fragColor = (ambient + diffuse + specular) * u_ObjColor;
} 