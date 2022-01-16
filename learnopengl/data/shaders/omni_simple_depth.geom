#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_ShadowMatrices[6];

out vec4 v_FragPos;

void main() {
	// For each face on cubemap
	for (int face = 0; face < 6; ++face) {
		// Specifies which cubemap face to emite primitive to
		gl_Layer = face;
		// For each triangle vertex
		for (int i = 0; i < 3; ++i) {
			v_FragPos = gl_in[i].gl_Position;
			gl_Position = u_ShadowMatrices[face] * v_FragPos;

			EmitVertex();
		}

		EndPrimitive();
	}
}