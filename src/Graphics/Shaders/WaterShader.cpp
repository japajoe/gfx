#include "WaterShader.hpp"

namespace GFX
{
        static std::string vertexSource = R"(#version 330 core
#include <Core>

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

#define MAX_WAVES 10

struct Wave {
    vec2 direction;
    float steepness;
    float waveLength;
    float amplitude;
    float speed;
};

uniform mat4 uModel;
uniform mat3 uModelInverted;
uniform mat4 uMVP;
uniform Wave uWaves[MAX_WAVES];
uniform int uNumWaves;

out VS_OUT {
    vec3 normal;
    vec2 uv;
    vec3 fragPosition;
    mat4 mvp;
} vs_out;

#define PI 3.14159265359

//https://catlikecoding.com/unity/tutorials/flow/waves/
vec3 gerstner_wave(vec3 point, vec2 direction, float steepness, float wavelength, float amplitude, float speed) {
    float k = 2 * PI / wavelength;
    float c = sqrt(9.8 / k);
    vec2 d = normalize(direction);
    float f = k * (dot(d, point.xz) - c * uWorld.time * speed);
    float a = steepness / k;
    
    return vec3(d.x * (a * cos(f)), a * sin(f), d.y * (a * cos(f))) * amplitude;
}

void main() {
    vec3 pos = aPosition;

    for(int i = 0; i < uNumWaves; i++) {
        vec2 dir = normalize(uWaves[i].direction);
        float st = uWaves[i].steepness;
        float wl = uWaves[i].waveLength;
        float am = uWaves[i].amplitude;
        float sp = uWaves[i].speed;
        pos += gerstner_wave(pos, dir, st, wl, am, sp);
    }
    
    vs_out.normal = aNormal;
    vs_out.uv = aUV;
    vs_out.fragPosition = vec3(uModel * vec4(pos, 1.0));
    vs_out.mvp = uMVP;

    gl_Position = vec4(pos, 1.0);
})";

	static std::string geometrySource = R"(#version 330 core
#include <Core>

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 normal;
    vec2 uv;
    vec3 fragPosition;
    mat4 mvp;
} gs_in[];

out GS_OUT {
    vec3 normal;
    vec2 uv;
    vec3 fragPosition;
} gs_out;

void main() {
    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;

    vec3 edge1 = pos1 - pos0;
    vec3 edge2 = pos2 - pos0;
    vec3 normal = normalize(cross(edge1, edge2));

    for(int i = 0; i < 3; i++) {
        gs_out.normal       = normal;
        gs_out.uv           = gs_in[i].uv;
        gs_out.fragPosition = gs_in[i].fragPosition;
        gl_Position = gs_in[i].mvp * vec4(gl_in[i].gl_Position.xyz, 1.0);
        EmitVertex();
    }

    EndPrimitive();
})";

	static std::string fragmentSource = R"(#version 330 core
#include <Core>

uniform sampler2D uDiffuseTexture;
uniform vec4 uDiffuseColor;
uniform float uAmbientStrength;
uniform float uShininess;
uniform vec2 uUVScale;
uniform vec2 uUVOffset;

in GS_OUT {
    vec3 normal;
    vec2 uv;
    vec3 fragPosition;
} fs_in;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uDiffuseTexture, (fs_in.uv + uUVOffset) * uUVScale);
    vec3 normal = normalize(fs_in.normal);
    vec3 lighting = calculate_lighting(fs_in.fragPosition, uCamera.position.xyz, normal, texColor.rgb, uDiffuseColor.rgb, uAmbientStrength, uShininess);

    if(uWorld.fogEnabled > 0) {
        float visibility = calculate_fog(uWorld.fogDensity, uWorld.fogGradient, uCamera.position.xyz, fs_in.fragPosition);
        lighting = mix(uWorld.fogColor.rgb, lighting, visibility);
    }

    vec4 outputColor = tone_map(vec4(lighting, texColor.a * uDiffuseColor.a));
    FragColor = gamma_correction(outputColor);

})";

	Shader WaterShader::Create()
	{
		return Shader(vertexSource, fragmentSource, geometrySource);
	}
	
	std::string WaterShader::GetVertexSource()
	{
		return vertexSource;
	}
	
	std::string WaterShader::GetGeometrySource()
	{
		return geometrySource;
	}
	
	std::string WaterShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}