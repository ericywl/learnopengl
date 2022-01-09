#version 330 core
in vec2 v_TexCoord;

out vec4 fragColor;

uniform sampler2D u_TextureDiffuse1;

void main()
{
    fragColor = texture(u_TextureDiffuse1, v_TexCoord);
} 