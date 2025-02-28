#include "Constants.hpp"

namespace GFX
{
	std::string Constants::GetString(ConstantString n)
	{
		switch(n)
		{
			case ConstantString::FontDefault:
				return "Default";
			case ConstantString::UniformBufferCamera:
				return "Camera";
			case ConstantString::UniformBufferLights:
				return "Lights";
			case ConstantString::UniformBufferShadow:
				return "Shadow";
			case ConstantString::UniformBufferWorld:
				return "World";
			case ConstantString::MeshCapsule:
				return "Capsule";
			case ConstantString::MeshCube:
				return "Cube";
			case ConstantString::MeshCylinder:
				return "Cylinder";
			case ConstantString::MeshPlane:
				return "Plane";
			case ConstantString::MeshQuad:
				return "Quad";
			case ConstantString::MeshSphere:
				return "Sphere";
			case ConstantString::MeshSkybox:
				return "Skybox";
			case ConstantString::ShaderHorizontalBlur:
				return "HorizontalBlur";
			case ConstantString::ShaderVerticalBlur:
				return "VerticalBlur";
			case ConstantString::ShaderDepth:
				return "Depth";
			case ConstantString::ShaderDiffuse:
				return "Diffuse";
			case ConstantString::ShaderDiffuseInstanced:
				return "DiffuseInstanced";
			case ConstantString::ShaderGrayscale:
				return "Grayscale";
			case ConstantString::ShaderLine:
				return "Line";
			case ConstantString::ShaderParticle:
				return "Particle";
			case ConstantString::ShaderPostProcessing:
				return "PostProcessing";
			case ConstantString::ShaderProceduralSkybox:
				return "ProceduralSkybox";
			case ConstantString::ShaderProceduralSkybox2:
				return "ProceduralSkybox2";
			case ConstantString::ShaderSkybox:
				return "Skybox";
			case ConstantString::ShaderWater:
				return "Water";
			case ConstantString::ShaderTerrain:
				return "Terrain";
			case ConstantString::TextureDefault:
				return "Default";
			case ConstantString::TextureDefaultCubeMap:
				return "DefaultCubeMap";
			case ConstantString::TextureDefaultGrass:
				return "DefaultGrass";
			case ConstantString::TextureDepth:
				return "Depth";
			default:
				return "Unknown";
		}
	}

	float Constants::GetFloat(ConstantFloat n)
	{
		switch(n)
		{
			case ConstantFloat::MaterialAmbientStrength:
				return 0.5f;
			default:
				return 0.0f;
		}
	}
}