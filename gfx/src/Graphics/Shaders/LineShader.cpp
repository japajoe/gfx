#include "LineShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;

uniform mat4 uMVP;

out vec4 oColor;

void main() {
    oColor = aColor;
    gl_Position = uMVP * vec4(aPosition, 1.0);
})";

	static std::string fragmentSource = R"(#version 330 core

in vec4 oColor;
out vec4 FragColor;

vec4 gamma_correction(vec4 color) {
    return vec4(pow(color.rgb, vec3(1.0/2.2)), color.a);
}

void main() {
	FragColor = gamma_correction(oColor);
})";

	Shader LineShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string LineShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string LineShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}