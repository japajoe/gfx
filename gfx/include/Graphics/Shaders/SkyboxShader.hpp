#ifndef GFX_SKYBOXSHADER_HPP
#define GFX_SKYBOXSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class SkyboxShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif