#version 330 core
in vec2 v_TexCoord;

out vec4 fragColor;

uniform sampler2D u_ScreenTexture;

const float offset = 1.0 / 300.0;

vec4 kernelEffect() {
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float sharpenKernel[9] = float[](
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0
    );

    float blurKernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16 
    );

    float edgeDetectKernel[9] = float[](
        1,  1,  1,
        1, -8,  1,
        1,  1,  1
    );

    float embossKernel[9] = float[](
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2 
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(u_ScreenTexture, v_TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        col += sampleTex[i] * sharpenKernel[i];
    }

    return vec4(col, 1.0);
}

void main() {
	vec4 texColor = texture(u_ScreenTexture, v_TexCoord);
	// Inverse
	vec4 inverseColor = vec4(vec3(1.0 - texColor), 1.0);
	// Greyscale
	float avg = (texColor.r + texColor.g + texColor.b) / 3.0;
	float weightedAvg = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
	vec4 greyScale = vec4(weightedAvg, weightedAvg, weightedAvg, 1.0);

	// Final result
	fragColor = kernelEffect();
}