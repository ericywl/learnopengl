#version 330 core
in vec4 v_FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() {
	float lightDist = length(v_FragPos.xyz - u_LightPos);
	// Map light distance to [0, 1] range
	lightDist = lightDist / u_FarPlane;

	gl_FragDepth = lightDist;
}