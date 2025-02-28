#ifndef GFX_HORIZONTALBLURSHADER_HPP
#define GFX_HORIZONTALBLURSHADER_HPP

#include "../../Shader.hpp"
#include <string>

namespace GFX
{
	class HorizontalBlurShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif