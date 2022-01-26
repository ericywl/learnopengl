#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoord;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_InvTModel;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

void main() {
	vec3 T = normalize(mat3(u_InvTModel) * a_Tangent);
	vec3 N = normalize(mat3(u_InvTModel) * a_Normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	vs_out.TexCoord = a_TexCoord;
	vs_out.TangentLightPos = TBN * u_LightPos;
	vs_out.TangentViewPos = TBN * u_ViewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
