#ifndef GFX_PROCEDURALSKYBOX2SHADER_HPP
#define GFX_PROCEDURALSKYBOX2SHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class ProceduralSkybox2Shader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif