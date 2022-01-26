#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_Offset;

out vec3 v_Color;

void main() {
	// Scale the triangle based on instance ID
	vec2 pos = a_Position * (gl_InstanceID / 100.0);
	gl_Position = vec4(pos + a_Offset, 0.0, 1.0);
	v_Color = a_Color;
}
