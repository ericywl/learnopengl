#version 330 core
in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D u_TextureDiffuse;
uniform samplerCube u_DepthMap;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_FarPlane;

float ShadowCalculation(vec3 fragPos) {
	vec3 fragToLight = fragPos - u_LightPos;
	float currentDepth = length(fragToLight);

	// Apply PCF with some offset directions
	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	); 

	float bias = 0.15;
	int samples = 20;
	// Scale disk radius based on the distance from view to fragment
	float viewDist = length(u_ViewPos - fragPos);
	float diskRadius = (1.0 + (viewDist / u_FarPlane)) / 25.0;
	float shadow = 0.0;
	for (int i = 0; i < samples; i++) {
		float closestDepth = texture(u_DepthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r * u_FarPlane;
		if (currentDepth - bias > closestDepth) {
			shadow += 1.0;
		}
	}

	return shadow / float(samples);
}

void main() {
	vec3 color = texture(u_TextureDiffuse, fs_in.texCoord).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 lightColor = vec3(0.3);

	// Ambient
	vec3 ambient = 0.3 * lightColor;

	// Diffuse
	vec3 lightDir = normalize(u_LightPos - fs_in.fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// Specular
	vec3 viewDir = normalize(u_ViewPos - fs_in.fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	// Shadow and final lighting
	float shadow = ShadowCalculation(fs_in.fragPos);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	fragColor = vec4(lighting, 1.0);
}
