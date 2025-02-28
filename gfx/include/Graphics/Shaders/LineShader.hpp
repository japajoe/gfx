#ifndef GFX_LINESHADER_HPP
#define GFX_LINESHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class LineShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif