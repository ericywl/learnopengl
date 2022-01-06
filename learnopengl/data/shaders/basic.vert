#version 330 core
layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec4 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out vec4 v_FragPos;
out vec4 v_Normal;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_InvTModel;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * a_Position;
    v_FragPos = u_Model * a_Position;
    v_Normal = u_InvTModel * a_Normal;
    v_TexCoord = a_TexCoord;
}
