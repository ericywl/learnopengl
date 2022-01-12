#version 330 core
in vec3 v_TextureDir;

out vec4 fragColor;

uniform samplerCube u_Cubemap;

void main() {
	fragColor = texture(u_Cubemap, v_TextureDir);
}