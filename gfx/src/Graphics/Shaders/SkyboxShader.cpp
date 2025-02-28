#include "SkyboxShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 oUV;
out vec3 oFragPosition;
out float oBr;

void main() {
    //oUV = aUV;
	oUV = aPosition;
    oFragPosition = vec3(uModel * vec4(aPosition, 1.0));
	gl_Position = uProjection * mat4(mat3(uView)) * vec4(aPosition, 1.0);
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>

uniform samplerCube uTexture;
uniform vec4 uDiffuseColor;

in vec3 oUV;
in vec3 oFragPosition;
out vec4 FragColor;

void main() {
    vec3 uv = oUV;
    uv.y *= -1.0;
    vec4 result = texture(uTexture, uv) * uDiffuseColor;
	
    vec4 outputColor = tone_map(result);
    FragColor = gamma_correction(outputColor);
})";

	Shader SkyboxShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string SkyboxShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string SkyboxShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}