#version 330 core
in vec2 v_TexCoord;

out vec4 fragColor;

uniform sampler2D u_DepthMap;
uniform float u_Near;
uniform float u_Far;

// Linearize non-linear depth buffer values for visualization
float LinearizeDepth(float depth) {
	float ndc = depth * 2.0 - 1.0;
    return (2.0 * u_Near * u_Far) / (u_Far + u_Near - ndc * (u_Far - u_Near));
}

void main()
{
    float depthValue = texture(u_DepthMap, v_TexCoord).r;
    fragColor = vec4(vec3(depthValue), 1.0);
} 
