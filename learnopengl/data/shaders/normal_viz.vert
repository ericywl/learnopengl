#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

out VS_OUT {
	vec3 Normal;
} vs_out;

uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_InvTViewModel;

void main() {
	gl_Position = u_View * u_Model * vec4(a_Position, 1.0);
	vs_out.Normal = normalize(vec3(u_InvTViewModel * vec4(a_Normal, 1.0)));
}
