#include "DepthShader.hpp"

namespace GFX
{
	static std::string vertexSource = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 3) in mat4 aInstanceModel;

uniform mat4 uModel;
uniform int uHasInstanceData;

void main() {
    if(uHasInstanceData > 0)
        gl_Position = aInstanceModel * vec4(aPosition, 1.0);
    else
        gl_Position = uModel * vec4(aPosition, 1.0);
})";

	static std::string geometrySource = R"(#version 420 core
layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

#include <Core>

void main() {          
    gl_Position = uShadow.lightSpaceMatrices[gl_InvocationID] * gl_in[0].gl_Position;
    gl_Layer = gl_InvocationID;
    EmitVertex();

    gl_Position = uShadow.lightSpaceMatrices[gl_InvocationID] * gl_in[1].gl_Position;
    gl_Layer = gl_InvocationID;
    EmitVertex();

    gl_Position = uShadow.lightSpaceMatrices[gl_InvocationID] * gl_in[2].gl_Position;
    gl_Layer = gl_InvocationID;
    EmitVertex();

	EndPrimitive();
})";

	static std::string fragmentSource = R"(#version 330 core
void main() {
})";

	Shader DepthShader::Create()
	{
		return Shader(vertexSource, fragmentSource, geometrySource);
	}

	std::string DepthShader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string DepthShader::GetGeometrySource()
	{
		return geometrySource;
	}

	std::string DepthShader::GetFragmentSource()
	{
		return fragmentSource;
	}
}