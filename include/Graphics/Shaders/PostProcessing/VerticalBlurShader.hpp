#ifndef GFX_VERTICALBLURSHADER_HPP
#define GFX_VERTICALBLURSHADER_HPP

#include "../../Shader.hpp"
#include <string>

namespace GFX
{
	class VerticalBlurShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif