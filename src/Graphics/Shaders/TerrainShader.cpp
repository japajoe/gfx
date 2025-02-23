#include "TerrainShader.hpp"
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

uniform vec4 uDiffuseColor;
uniform float uAmbientStrength;
uniform float uShininess;
uniform sampler2D uSplatMap;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform sampler2D uTexture3;
uniform sampler2D uTexture4;
uniform vec2 uUVScale1;
uniform vec2 uUVScale2;
uniform vec2 uUVScale3;
uniform vec2 uUVScale4;

in vec3 oNormal;
in vec3 oFragPosition;
in vec2 oUV;

out vec4 FragColor;

vec4 calculate_texture_color() {
    vec4 blendMapColor = texture(uSplatMap, oUV);
    float backgroundTextureAmount = 1.0 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);

    vec4 backgroundTextureColor = texture(uTexture1, oUV * uUVScale1) * backgroundTextureAmount;
    vec4 rTextureColor = texture(uTexture2, oUV * uUVScale2) * blendMapColor.r;
    vec4 gTextureColor = texture(uTexture3, oUV * uUVScale3) * blendMapColor.g;
    vec4 bTextureColor = texture(uTexture4, oUV * uUVScale4) * blendMapColor.b;
    vec4 result = backgroundTextureColor + rTextureColor + gTextureColor + bTextureColor;
    return result;
}

void main() {
    vec4 texColor = calculate_texture_color();
    vec3 normal = normalize(oNormal);
    vec3 lighting = calculate_lighting(oFragPosition, uCamera.position.xyz, normal, texColor.rgb, uDiffuseColor.rgb, uAmbientStrength, uShininess);

    if(uWorld.fogEnabled > 0) {
        float visibility = calculate_fog(uWorld.fogDensity, uWorld.fogGradient, uCamera.position.xyz, oFragPosition);
        lighting = mix(uWorld.fogColor.rgb, lighting, visibility);
    }

    vec4 outputColor = tone_map(vec4(lighting, texColor.a));
    FragColor = gamma_correction(outputColor);
})";

	Shader TerrainShader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string TerrainShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string TerrainShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}