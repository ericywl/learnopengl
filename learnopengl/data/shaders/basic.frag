#version 330 core
in vec4 v_Color;
in vec2 v_TexCoord;

out vec4 fragColor;

uniform vec4 u_Color;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;

void main()
{
    fragColor = mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, v_TexCoord), 0.5);
} 