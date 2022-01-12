#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

out vec3 v_Position;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_InvTModel;

void main() {
	v_Normal = mat3(u_InvTModel) * a_Normal;
	v_Position = vec3(u_Model * vec4(a_Position, 1.0));
	gl_Position = u_Projection * u_View * vec4(v_Position, 1.0);
}