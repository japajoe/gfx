#ifndef GFX_CONSTANTS_HPP
#define GFX_CONSTANTS_HPP

#include <cstdint>
#include <string>

namespace GFX
{
	enum UniformBindingIndex : uint32_t
	{
		UniformBindingIndex_Camera = 0,
		UniformBindingIndex_Lights = 1,
		UniformBindingIndex_World = 2,
		UniformBindingIndex_Shadow = 3
	};

	enum class ConstantString
	{
		FontDefault,
		MeshCapsule,
		MeshCube,
		MeshPlane,
		MeshQuad,
		MeshSphere,
		MeshSkybox,
		ShaderDepth,
		ShaderDiffuse,
		ShaderDiffuseInstanced,
		ShaderLine,
		ShaderParticle,
		ShaderProceduralSkybox,
		ShaderSkybox,
		ShaderTerrain,
		ShaderWater,
		TextureDefault,
		TextureDefaultCubeMap,
		TextureDepth,
		UniformBufferCamera,
		UniformBufferLights,
		UniformBufferShadow,
		UniformBufferWorld
	};

	class Constants
	{
	public:
		static std::string GetString(ConstantString n);
	};
}

#endif