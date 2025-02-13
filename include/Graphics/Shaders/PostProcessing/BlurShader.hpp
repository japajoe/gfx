#ifndef GFX_BLURSHADER_HPP
#define GFX_BLURSHADER_HPP

#include "../../Shader.hpp"
#include <string>

namespace GFX
{
	class BlurShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif