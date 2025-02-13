#ifndef GFX_POSTPROCESSINGSHADER_HPP
#define GFX_POSTPROCESSINGSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class PostProcessingShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif