#ifndef GFX_PROCEDURALSKYBOXSHADER_HPP
#define GFX_PROCEDURALSKYBOXSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class ProceduralSkyboxShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif