#ifndef GFX_DEPTHSHADER_HPP
#define GFX_DEPTHSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class DepthShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetGeometrySource();
		static std::string GetFragmentSource();
	};
}

#endif