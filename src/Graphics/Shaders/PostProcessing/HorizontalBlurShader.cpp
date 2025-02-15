#include "HorizontalBlurShader.hpp"
#include "../PostProcessingShader.hpp"

namespace GFX
{
	static std::string fragmentSource = R"(#version 330 core
#include <Core>
uniform sampler2D colorBuffer;

// Gaussian kernel
const float weight[5] = float[] (
	0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);

in vec2 oUV;
out vec4 FragColor;

vec4 horizontal() {
	vec2 tex_offset = 1.0 / uCamera.resolution.xy; // size of a single texel
	vec3 result = texture(colorBuffer, oUV).rgb * weight[0];

	for (int i = 1; i < 5; i++)
	{
		result += texture(colorBuffer, oUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		result += texture(colorBuffer, oUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
	}

	return vec4(result, 1.0);
}

void main() {
	vec4 color = horizontal();
	FragColor = color;
})";

	Shader HorizontalBlurShader::Create()
	{
		return Shader(GetVertexSource(), fragmentSource);
	}

	std::string HorizontalBlurShader::GetVertexSource()
	{
		return PostProcessingShader::GetVertexSource();
	}

	std::string HorizontalBlurShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}