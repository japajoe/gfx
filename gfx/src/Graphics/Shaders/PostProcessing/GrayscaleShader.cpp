#include "GrayscaleShader.hpp"
#include "../PostProcessingShader.hpp"

namespace GFX
{
	static std::string fragmentSource = R"(#version 330 core
#include <Core>
uniform sampler2D colorBuffer;

in vec2 oUV;
out vec4 FragColor;

void main() {
	vec4 color = texture(colorBuffer, oUV);
	float pixel = (color.r + color.g + color.b) / 3.0;
	FragColor = vec4(pixel, pixel, pixel, color.a);
})";

	Shader GrayscaleShader::Create()
	{
		return Shader(GetVertexSource(), fragmentSource);
	}

	std::string GrayscaleShader::GetVertexSource()
	{
		return PostProcessingShader::GetVertexSource();
	}

	std::string GrayscaleShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}