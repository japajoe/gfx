#ifndef GFX_GRAYSCALESHADER_HPP
#define GFX_GRAYSCALESHADER_HPP

#include "../../Shader.hpp"
#include <string>

namespace GFX
{
	class GrayscaleShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif