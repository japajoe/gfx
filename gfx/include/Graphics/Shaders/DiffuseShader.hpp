#ifndef GFX_DIFFUSESHADER_HPP
#define GFX_DIFFUSESHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class DiffuseShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif