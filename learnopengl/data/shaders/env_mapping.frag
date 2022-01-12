#version 330 core
in vec3 v_Position;
in vec3 v_Normal;

out vec4 fragColor;

uniform vec3 u_CameraPos;
uniform samplerCube u_Skybox;
uniform 

void main() {
	vec3 I = normalize(v_Position - u_CameraPos);

	// Add reflection to model
	// vec3 R = reflect(I, normalize(v_Normal));

	// Add refraction to model
	float ratio = 1.00 / 1.52;
	vec3 R = refract(I, normalize(v_Normal), ratio);

	// Final result
	fragColor = vec4(texture(u_Skybox, R).rgb, 1.0);
}