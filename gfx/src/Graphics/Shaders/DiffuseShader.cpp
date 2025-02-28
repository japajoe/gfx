#include "DiffuseShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat3 uModelInverted;
uniform mat4 uMVP;

out vec3 oNormal;
out vec3 oFragPosition;
out vec2 oUV;

void main() {
    gl_Position = uMVP * vec4(aPosition, 1.0);
    oNormal = normalize(uModelInverted * aNormal);
    oFragPosition = vec3(uModel * vec4(aPosition, 1.0));
    oUV = aUV;
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>

uniform sampler2D uDiffuseTexture;
uniform vec4 uDiffuseColor;
uniform float uAmbientStrength;
uniform float uShininess;
uniform vec2 uUVScale;
uniform vec2 uUVOffset;

in vec3 oNormal;
in vec3 oFragPosition;
in vec2 oUV;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uDiffuseTexture, (oUV + uUVOffset) * uUVScale);
    vec3 normal = normalize(oNormal);
    vec3 lighting = calculate_lighting(oFragPosition, uCamera.position.xyz, normal, texColor.rgb, uDiffuseColor.rgb, uAmbientStrength, uShininess);

    if(uWorld.fogEnabled > 0) {
        float visibility = calculate_fog(uWorld.fogDensity, uWorld.fogGradient, uCamera.position.xyz, oFragPosition);
        lighting = mix(uWorld.fogColor.rgb, lighting, visibility);
    }

	vec4 outputColor = tone_map(vec4(lighting, texColor.a));
    FragColor = gamma_correction(outputColor);
})";

	Shader DiffuseShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string DiffuseShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string DiffuseShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}