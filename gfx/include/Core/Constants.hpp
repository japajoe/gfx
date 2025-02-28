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
		MeshCylinder,
		MeshPlane,
		MeshQuad,
		MeshSphere,
		MeshSkybox,
		ShaderHorizontalBlur,
		ShaderVerticalBlur,
		ShaderDepth,
		ShaderDiffuse,
		ShaderDiffuseInstanced,
		ShaderGrayscale,
		ShaderLine,
		ShaderParticle,
		ShaderPostProcessing,
		ShaderProceduralSkybox,
		ShaderProceduralSkybox2,
		ShaderSkybox,
		ShaderTerrain,
		ShaderWater,
		TextureDefault,
		TextureDefaultCubeMap,
		TextureDefaultGrass,
		TextureDepth,
		UniformBufferCamera,
		UniformBufferLights,
		UniformBufferShadow,
		UniformBufferWorld
	};

	enum class ConstantFloat
	{
		MaterialAmbientStrength
	};

	class Constants
	{
	public:
		static std::string GetString(ConstantString n);
		static float GetFloat(ConstantFloat n);
	};
}

#endif