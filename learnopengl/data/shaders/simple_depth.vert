#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;

void main() {
	// Transform positions to light-space so that we can use it to check depth
	gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
}