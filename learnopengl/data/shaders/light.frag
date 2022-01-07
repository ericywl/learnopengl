#version 330 core
out vec4 fragColor;

uniform vec3 u_LightColor;

void main() 
{
	fragColor = vec4(u_LightColor, 1.0);
}