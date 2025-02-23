#include "ParticleShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core
#include <Core>

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 aInstanceModel;
layout (location = 7) in vec4 aInstanceColor;

out vec3 oFragPosition;
out vec3 oNormal;
out vec2 oUV;
out vec4 oColor;

void main() {
    oFragPosition = vec3(aInstanceModel * vec4(aPosition, 1.0)); // Vertex in world space
    oNormal = inverse(transpose(mat3(aInstanceModel))) * aNormal;
    oUV = aUV;
    oColor = aInstanceColor;
    gl_Position = uCamera.viewProjection * aInstanceModel * vec4(aPosition, 1.0);
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>

uniform sampler2D uDiffuseTexture;
uniform vec2 uUVOffset;
uniform vec2 uUVScale;
uniform float uAlphaCutOff;
uniform float uAmbientStrength;
uniform float uShininess;

in vec3 oFragPosition;
in vec3 oNormal;
in vec2 oUV;
in vec4 oColor;

out vec4 FragColor;

void main() {
    vec4 texColor = texture2D(uDiffuseTexture, (oUV + uUVOffset) * uUVScale);

    if(texColor.a < uAlphaCutOff)
        discard;

    vec3 normal = normalize(oNormal);
    vec3 lighting = calculate_lighting(oFragPosition, uCamera.position.xyz, normal, texColor.rgb, oColor.rgb, uAmbientStrength, uShininess);

    if(uWorld.fogEnabled > 0) {
        float visibility = calculate_fog(uWorld.fogDensity, uWorld.fogGradient, uCamera.position.xyz, oFragPosition);
        lighting.rgb = mix(uWorld.fogColor.rgb, lighting.rgb, visibility);
    }

    vec4 outputColor = tone_map(vec4(lighting, texColor.a));
    FragColor = gamma_correction(outputColor);
})";

	Shader ParticleShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string ParticleShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string ParticleShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}