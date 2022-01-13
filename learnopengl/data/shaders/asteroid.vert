#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in mat4 a_InstancedModel;

out vec2 v_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
	gl_Position = u_Projection * u_View * a_InstancedModel * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}