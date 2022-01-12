#version 330 core
layout (location = 0) in vec3 a_Position;

out vec3 v_TextureDir;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
	v_TextureDir = a_Position;
	vec4 finalPos = u_Projection * u_View * vec4(a_Position, 1.0);
	gl_Position = finalPos.xyww;
}
