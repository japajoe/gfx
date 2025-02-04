#include "Resources.hpp"
#include "../Core/Debug.hpp"

namespace GFX
{
	std::unordered_map<std::string,UniformBufferObject> Resources::uniformBuffers;
	std::unordered_map<std::string,Font> Resources::fonts;
	std::unordered_map<std::string,Shader> Resources::shaders;
	std::unordered_map<std::string,Texture2D> Resources::textures2D;
	std::unordered_map<std::string,Texture3D> Resources::textures3D;
	std::unordered_map<std::string,TextureCubeMap> Resources::texturesCubemap;
	std::unordered_map<std::string,Mesh> Resources::meshes;

	static void LogAdd(const std::string &type, const std::string &name)
	{
		Debug::WriteLine("[" + type + "] " + name + " successfully added");
	}

	static void LogError(const std::string &type, const std::string &name)
	{
		Debug::WriteLine("[" + type + "] " + name + " failed to add");
	}

	UniformBufferObject *Resources::AddUniformBuffer(const std::string &name, const UniformBufferObject &ubo)
	{
		if(uniformBuffers.contains(name))
		{
			LogError("UBO", name);
			return nullptr;
		}
		uniformBuffers[name] = ubo;
		LogAdd("UBO", name);
		return &uniformBuffers[name];
	}

	UniformBufferObject *Resources::FindUniformBuffer(const std::string &name)
	{
		if(!uniformBuffers.contains(name))
			return nullptr;
		return &uniformBuffers[name];
	}

	Font *Resources::AddFont(const std::string &name, const Font &font)
	{
		if(fonts.contains(name))
		{
			LogError("FONT", name);
			return nullptr;
		}
		fonts[name] = font;
		LogAdd("FONT", name);
		return &fonts[name];
	}

	Font *Resources::FindFont(const std::string &name)
	{
		if(!fonts.contains(name))
			return nullptr;
		return &fonts[name];
	}

	Shader *Resources::AddShader(const std::string &name, const Shader &shader)
	{
		if(shaders.contains(name))
		{
			LogError("SHADER", name);
			return nullptr;
		}
		shaders[name] = shader;
		LogAdd("SHADER", name);
		return &shaders[name];
	}

	Shader *Resources::FindShader(const std::string &name)
	{
		if(!shaders.contains(name))
			return nullptr;
		return &shaders[name];
	}

	Texture2D *Resources::AddTexture2D(const std::string &name, const Texture2D &texture)
	{
		if(textures2D.contains(name))
		{
			LogError("TEXTURE2D", name);
			return nullptr;
		}
		textures2D[name] = texture;
		LogAdd("TEXTURE2D", name);
		return &textures2D[name];
	}

	Texture2D *Resources::FindTexture2D(const std::string &name)
	{
		if(!textures2D.contains(name))
			return nullptr;
		return &textures2D[name];
	}

	Texture3D *Resources::AddTexture3D(const std::string &name, const Texture3D &texture)
	{
		if(textures3D.contains(name))
		{
			LogError("TEXTURE3D", name);
			return nullptr;
		}
		textures3D[name] = texture;
		LogAdd("TEXTURE3D", name);
		return &textures3D[name];
	}

	Texture3D *Resources::FindTexture3D(const std::string &name)
	{
		if(!textures3D.contains(name))
			return nullptr;
		return &textures3D[name];
	}

	TextureCubeMap *Resources::AddTextureCubeMap(const std::string &name, const TextureCubeMap &texture)
	{
		if(texturesCubemap.contains(name))
		{
			LogError("TEXTURECUBEMAP", name);
			return nullptr;
		}
		texturesCubemap[name] = texture;
		LogAdd("TEXTURECUBEMAP", name);
		return &texturesCubemap[name];
	}
	
	TextureCubeMap *Resources::FindTextureCubeMap(const std::string &name)
	{
		if(!texturesCubemap.contains(name))
			return nullptr;
		return &texturesCubemap[name];
	}

	Mesh *Resources::AddMesh(const std::string &name, const Mesh &mesh)
	{
		if(meshes.contains(name))
		{
			LogError("MESH", name);
			return nullptr;
		}
		meshes[name] = mesh;
		LogAdd("MESH", name);
		return &meshes[name];
	}

	Mesh *Resources::FindMesh(const std::string &name)
	{
		if(!meshes.contains(name))
			return nullptr;
		return &meshes[name];
	}
}