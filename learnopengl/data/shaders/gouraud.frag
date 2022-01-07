#version 330 core
in vec4 v_Gouraud;
in vec2 v_TexCoord;

uniform vec4 u_ObjColor;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;

out vec4 fragColor;

void main()
{
   // Final result
    fragColor = v_Gouraud * u_ObjColor;
} 