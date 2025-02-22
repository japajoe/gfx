#ifndef GFX_PARTICLESHADER_HPP
#define GFX_PARTICLESHADER_HPP

#include "../Shader.hpp"
#include <string>

namespace GFX
{
	class ParticleShader
	{
	public:
		static Shader Create();
		static std::string GetVertexSource();
		static std::string GetFragmentSource();
	};
}

#endif