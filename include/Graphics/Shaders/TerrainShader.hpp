#ifndef GFX_TERRAINSHADER_HPP
#define GFX_TERRAINSHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class TerrainShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif