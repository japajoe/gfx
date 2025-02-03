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
			case ConstantString::MeshPlane:
				return "Plane";
			case ConstantString::MeshQuad:
				return "Quad";
			case ConstantString::MeshSphere:
				return "Sphere";
			case ConstantString::MeshSkybox:
				return "Skybox";
			case ConstantString::ShaderDepth:
				return "Depth";
			case ConstantString::ShaderDiffuse:
				return "Diffuse";
			case ConstantString::ShaderDiffuseInstanced:
				return "DiffuseInstanced";
			case ConstantString::ShaderParticle:
				return "Particle";
			case ConstantString::ShaderProceduralSkybox:
				return "ProceduralSkybox";
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
			case ConstantString::TextureDepth:
				return "Depth";
			default:
				return "Unknown";
		}
	}
}