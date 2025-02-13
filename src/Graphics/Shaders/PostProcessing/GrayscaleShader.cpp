#include "GrayscaleShader.hpp"

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

in vec2 oUV;
out vec4 FragColor;

void main() {
	vec4 color = texture(colorBuffer, oUV);
	float pixel = (color.r + color.g + color.b) / 3.0;
	FragColor = vec4(pixel, pixel, pixel, color.a);
})";

	Shader GrayscaleShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string GrayscaleShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string GrayscaleShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}