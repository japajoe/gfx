#include "BlurShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core

const vec2[6] positions = vec2[](
	vec2(  1.0,  1.0),
	vec2( -1.0,  1.0),
	vec2( -1.0, -1.0),

	vec2( -1.0, -1.0),
	vec2(  1.0, -1.0),
	vec2(  1.0,  1.0)
);

out vec2 oUV;

void main() {
    vec2 vertexPos = positions[gl_VertexID];
	gl_Position = vec4(vertexPos, 0.0, 1.0);
	oUV = 0.5 * (vertexPos + vec2(1.0, 1.0));
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>
uniform sampler2D colorBuffer;
uniform float blurRadius = 4.0;

// Gaussian kernel
const float weight[5] = float[] (
	0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);

in vec2 oUV;
out vec4 FragColor;

vec4 blur() {
    vec2 tex_offset = 1.0 / uCamera.resolution.xy; // size of a single texel
    vec4 result = vec4(0.0);

    // Horizontal blur
    for (int i = -4; i <= 4; i++) {
        float offset = float(i) * blurRadius;
        result += texture(colorBuffer, oUV + vec2(tex_offset.x * offset, 0.0)) * weight[abs(i)];
    }
    
    // Vertical blur
    vec4 result2 = vec4(0.0);
    for (int i = -4; i <= 4; i++) {
        float offset = float(i) * blurRadius;
        result2 += texture(colorBuffer, oUV + vec2(0.0, tex_offset.y * offset)) * weight[abs(i)];
    }

    vec4 outputResult = 0.5 * result + 0.5 * result2;
	return outputResult;
}

void main() {
	vec4 color = blur();
	FragColor = color;
})";

	Shader BlurShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string BlurShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string BlurShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}