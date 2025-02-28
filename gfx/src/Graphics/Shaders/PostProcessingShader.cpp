#include "PostProcessingShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

out vec2 oUV;

void main() {
	gl_Position = vec4(aPosition, 1.0);
	oUV = aUV;
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>
uniform sampler2D colorBuffer;

in vec2 oUV;
out vec4 FragColor;

void main() {
	vec4 color = texture(colorBuffer, oUV);
	FragColor = color;
})";

	Shader PostProcessingShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string PostProcessingShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string PostProcessingShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}