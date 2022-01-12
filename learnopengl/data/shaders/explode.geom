#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 TexCoord;
} gs_in[];

out vec2 v_TexCoord;

uniform float u_Time;

vec4 Explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(u_Time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

vec3 GetNormal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main() {
	vec3 normal = GetNormal();

	gl_Position = Explode(gl_in[0].gl_Position, normal);
	v_TexCoord = gs_in[0].TexCoord;
	EmitVertex();

	gl_Position = Explode(gl_in[1].gl_Position, normal);
	v_TexCoord = gs_in[1].TexCoord;
	EmitVertex();

	gl_Position = Explode(gl_in[2].gl_Position, normal);
	v_TexCoord = gs_in[2].TexCoord;
	EmitVertex();

	EndPrimitive();
}