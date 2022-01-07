#version 330 core
in vec3 v_Gouraud;
in vec2 v_TexCoord;

uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;

out vec4 fragColor;

void main()
{
   // Final result
    fragColor = vec4(v_Gouraud, 1.0);
} 