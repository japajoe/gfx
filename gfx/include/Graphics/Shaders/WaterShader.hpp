#ifndef GFX_WATERSHADER_HPP
#define GFX_WATERSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class WaterShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetGeometrySource();
		static std::string GetFragmentSource();
	};
}

#endif