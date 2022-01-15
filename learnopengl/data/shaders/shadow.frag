#version 330 core
in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 fragPosLightSpace;
} fs_in;

out vec4 fragColor;

uniform sampler2D u_TextureDiffuse;
uniform sampler2D u_ShadowMap;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	// Convert it into range of [-1, 1]
	vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Convert [-1, 1] to [0, 1]
	projCoord = projCoord * 0.5 + 0.5;

	if (projCoord.z > 1.0) {
		return 0.0;
	}

	// Compare depth from shadow map with current depth
	// If current depth is higher than shadow map, it means the pixel is in shadow
	float closestDepth = texture(u_ShadowMap, projCoord.xy).r;
	float currentDepth = projCoord.z;

	// Apply shadow bias to offset shadow map and fix artifacts
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

	// Percentage closer filtering
	// Sample surrounding texels in shadow map and average the result for shadow
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(u_ShadowMap, projCoord.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadow / 9.0;
}

void main() {
	vec3 color = texture(u_TextureDiffuse, fs_in.texCoord).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 lightColor = vec3(1.0);

	// Ambient
	vec3 ambient = 0.15 * lightColor;

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
	float shadow = ShadowCalculation(fs_in.fragPosLightSpace, normal, lightDir);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	fragColor = vec4(lighting, 1.0);
}