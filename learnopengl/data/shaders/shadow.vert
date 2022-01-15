#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 fragPosLightSpace;
} vs_out;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_InvTModel;

void main() {
	vs_out.fragPos = vec3(u_Model * vec4(a_Position, 1.0));
	vs_out.normal = mat3(u_InvTModel) * a_Normal;
	vs_out.texCoord = a_TexCoord;
	vs_out.fragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.fragPos, 1.0);

	gl_Position = u_Projection * u_View * vec4(vs_out.fragPos, 1.0);
}